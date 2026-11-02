//@category Export

import java.io.File;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.List;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.Map;
import java.util.Set;
import java.util.regex.Pattern;

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.address.AddressSetView;
import ghidra.program.model.data.Array;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.DataTypeManager;
import ghidra.program.model.data.DataTypeWriter;
import ghidra.program.model.data.LongLongDataType;
import ghidra.program.model.data.SignedByteDataType;
import ghidra.program.model.data.StringDataType;
import ghidra.program.model.data.TerminatedStringDataType;
import ghidra.program.model.data.Undefined1DataType;
import ghidra.program.model.data.Undefined2DataType;
import ghidra.program.model.data.Undefined4DataType;
import ghidra.program.model.data.Undefined8DataType;
import ghidra.program.model.data.UnsignedLongLongDataType;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.pcode.PcodeOp;
import ghidra.program.model.pcode.HighSymbol;
import ghidra.program.model.symbol.SourceType;

public class ExportDecompiled extends GhidraScript {

    private String unsigned_type(int bytes) {
        if (bytes <= 1) {return "unsigned char";}
        if (bytes <= 2) {return "unsigned short";}
        if (bytes <= 4) {return "unsigned int";}
        return "unsigned long long";
    }

    private String emit_concat_helpers(String code) {
        Set<String> emitted = new LinkedHashSet<>();
        StringBuilder helpers = new StringBuilder();
        var matches = Pattern.compile("\\bCONCAT([1-8])([1-8])\\s*\\(").matcher(code);
        while (matches.find()) {
            String name = "CONCAT" + matches.group(1) + matches.group(2);
            if (!emitted.add(name)) {continue;}
            int highBytes = Integer.parseInt(matches.group(1));
            int lowBytes = Integer.parseInt(matches.group(2));
            if (highBytes + lowBytes > 8) {
                throw new IllegalStateException("Unsupported concatenation wider than 64 bits: " + name);
            }
            String resultType = unsigned_type(highBytes + lowBytes);
            String highMask = Long.toUnsignedString(-1L >>> (64 - highBytes * 8), 16);
            String lowMask = Long.toUnsignedString(-1L >>> (64 - lowBytes * 8), 16);
            helpers.append("static inline ").append(resultType).append(" ").append(name)
                .append("(").append(unsigned_type(highBytes)).append(" high, ")
                .append(unsigned_type(lowBytes)).append(" low)\n{\n")
                .append("    return (").append(resultType).append(")")
                .append("((((unsigned long long)high & 0x").append(highMask)
                .append("ULL) << ").append(lowBytes * 8).append(") | ")
                .append("((unsigned long long)low & 0x").append(lowMask).append("ULL));\n}\n\n");
        }
        return helpers.toString();
    }

    private boolean is_byte_string(Data data) {
        return data != null && (data.getDataType() instanceof StringDataType || data.getDataType() instanceof TerminatedStringDataType);
    }

    private String emit_string_declaration(HighSymbol symbol) throws Exception {
        if (!symbol.getStorage().isMemoryStorage()) {return null;}

        Data data = currentProgram.getListing().getDataAt( symbol.getStorage().getFirstVarnode().getAddress());
        if (!is_byte_string(data)) {return null;}

        //escape raw bytes
        byte[] bytes = data.getBytes();
        StringBuilder literal = new StringBuilder();
        for (byte value : bytes) {
            int b = value & 0xff;
            if (b == '"' || b == '\\') {
                literal.append('\\').append((char) b);
            }
            else if (b >= 32 && b < 127 && b != '?') {
                literal.append((char) b);
            }
            else {
                literal.append(String.format("\\%03o", b));
            }
        }

        //an explicit bound excludes the C literal's extra implicit terminator
        String name = symbol.getName();
        if (!name.matches("[A-Za-z_][A-Za-z0-9_]*")) {
            name = name.replaceAll("[^A-Za-z0-9_]", "_");
        }
        return "char " + name + "[" + bytes.length + "] = \"" + literal + "\";";
    }

    private String emit_global_declaration(DataType type, String name) {
        //place size declaration after name instead of after type
        StringBuilder declarator = new StringBuilder(name);
        while (type instanceof Array) {
            Array array = (Array) type;
            declarator.append("[").append(array.getNumElements()).append("]");
            type = array.getDataType();
        }
        return type.getDisplayName() + " " + declarator + ";";
    }

    private String emit_mmio_macro(HighSymbol symbol) {
        var storage = symbol.getStorage();
        if (!storage.isMemoryStorage()) {return null;}

        var address = storage.getFirstVarnode().getAddress();
        MemoryBlock block = currentProgram.getMemory().getBlock(address);
        if (block == null || !block.isVolatile() || block.isExecute()) {return null;}

        Data register = currentProgram.getListing().getDataAt(address);
        if (register == null || register.getLength() != symbol.getSize()) {return null;}

        //imported registers have user-defined labels prefixed by the peripheral name
        boolean importedRegister = false;
        for (var label : currentProgram.getSymbolTable().getSymbols(address)) {
            if (label.getSource() == SourceType.USER_DEFINED && label.getName().startsWith(block.getName() + "_")) {
                importedRegister = true;
                break;
            }
        }
        if (!importedRegister) {return null;}

        //use the imported register width and the name emitted by the decompiler
        String type;
        switch (register.getLength()) {
            case 1:
                type = "unsigned char";
                break;
            case 2:
                type = "unsigned short";
                break;
            case 4:
                type = "unsigned int";
                break;
            default:
                return null;
        }

        String literal = "0x" + Long.toUnsignedString(address.getOffset(), 16);
        return "#define " + symbol.getName() + " (*(volatile " + type + " *)" + literal + ")\n";
    }

    private String emit_c(AddressSetView addresses, Map<String, String> macros) throws Exception {
        Set<String> declarations = new LinkedHashSet<>();
        Set<String> prototypes = new LinkedHashSet<>();
        StringBuilder bodies = new StringBuilder();
        DecompileOptions options = new DecompileOptions();
        options.grabFromProgram(currentProgram);
        options.setCommentStyle(DecompileOptions.CommentStyleEnum.CPPStyle);
        DecompInterface decompiler = new DecompInterface();

        try {
            decompiler.setOptions(options);
            if (!decompiler.openProgram(currentProgram)) {
                throw new IllegalStateException("Cannot open program for global export: " + decompiler.getLastMessage());
            }

            //collect globals and function bodies
            for (Function function : currentProgram.getFunctionManager().getFunctions(addresses, true)) {
                monitor.checkCancelled();
                if (currentProgram.getListing().getInstructionAt(function.getEntryPoint()) == null) {continue;}

                var result = decompiler.decompileFunction(function, options.getDefaultTimeout(), monitor);

                if (!result.decompileCompleted() || result.getHighFunction() == null) {
                    throw new IllegalStateException("Cannot decompile " + function.getName() + ": " + result.getErrorMessage());
                }

                var decompiled = result.getDecompiledFunction();
                //use the recovered signature so prototypes agree with the exported definitions
                prototypes.add(decompiled.getSignature().strip().replaceFirst(";\\s*$", "") + ";");
                bodies.append(decompiled.getC()).append("\n\n");

                //parse global defs
                var symbols = result.getHighFunction().getGlobalSymbolMap().getSymbols();
                while (symbols.hasNext()) {
                    monitor.checkCancelled();
                    var symbol = symbols.next();
                    String name = symbol.getName();

                    //function addresses are declarations above, not byte-sized global objects
                    var storage = symbol.getStorage();
                    if (storage.isMemoryStorage() && currentProgram.getFunctionManager().getFunctionAt(
                        storage.getFirstVarnode().getAddress()) != null) {continue;}

                    //emit macro if this is an MMIO symbol
                    String macro = emit_mmio_macro(symbol);
                    if (macro != null) {
                        macros.put(name, macro);
                        continue;
                    }

                    String stringDeclaration = emit_string_declaration(symbol);
                    declarations.add(stringDeclaration != null ? stringDeclaration :
                        emit_global_declaration(symbol.getDataType(), name));
                }
            }
        }
        finally {
            decompiler.dispose();
        }

        return String.join("\n", prototypes) + "\n\n" +
            String.join("\n", declarations) + "\n\n" + bodies;
    }

    @Override
    public void run() throws Exception {

        String[] args = getScriptArgs();
        File dcmpFile = new File(args[0]);
        File asmFile = new File(args[1]);
        File pcodeFile = new File(args[2]);

        //rename generated string labels before decomp
        var definedData = currentProgram.getListing().getDefinedData(true);
        while (definedData.hasNext()) {
            monitor.checkCancelled();
            Data data = definedData.next();
            if (!is_byte_string(data)) {continue;}

            for (var symbol : currentProgram.getSymbolTable().getSymbols(data.getAddress())) {
                if (symbol != null && (symbol.getSource() == SourceType.DEFAULT || symbol.getSource() == SourceType.ANALYSIS || !symbol.getName().matches("[A-Za-z_][A-Za-z0-9_]*"))) {
                    symbol.setName("str_" + Long.toUnsignedString(data.getAddress().getOffset(), 16) + (symbol.isPrimary() ? "" : "_" + symbol.getID()), SourceType.ANALYSIS);
                }
            }
        }

        //print functions but skip inline and callfixup functions
        AddressSet cExportAddresses = new AddressSet(currentProgram.getMemory());
        for (Function function : currentProgram.getFunctionManager().getFunctions(true)) {
            monitor.checkCancelled();
            String fixup = function.getCallFixup();

            if (function.isInline() || (fixup != null && !fixup.isEmpty())) {
                cExportAddresses.delete(function.getBody());
            }
        }

        //export types without elf metadata
        DataTypeManager types = currentProgram.getDataTypeManager();
        List<DataType> exportedTypes = new ArrayList<>();
        //decompiler aliases may be used without being stored in the program's type manager
        exportedTypes.add(Undefined1DataType.dataType);
        exportedTypes.add(Undefined2DataType.dataType);
        exportedTypes.add(Undefined4DataType.dataType);
        exportedTypes.add(Undefined8DataType.dataType);
        exportedTypes.add(SignedByteDataType.dataType);
        exportedTypes.add(LongLongDataType.dataType);
        exportedTypes.add(UnsignedLongLongDataType.dataType);
        var allTypes = types.getAllDataTypes();

        while (allTypes.hasNext()) {
            monitor.checkCancelled();
            DataType type = allTypes.next();
            String category = type.getCategoryPath().getPath();

            if (!category.equals("/ELF") && !category.startsWith("/ELF/")) {
                exportedTypes.add(type);
            }
        }

        StringWriter declarations = new StringWriter();
        new DataTypeWriter(types, declarations, true).write(exportedTypes, monitor);
        //stdbool.h supplies bool, true and false; omit Ghidra's conflicting typedef
        String typeDeclarations = declarations.toString().replaceAll(
            "(?m)^typedef\\s+unsigned\\s+char\\s+bool\\s*;\\h*\\R?", "");

        Map<String, String> mmioMacros = new LinkedHashMap<>();
        String code = emit_c(cExportAddresses, mmioMacros);
        String concatHelpers = emit_concat_helpers(code);

        String macros = String.join("", mmioMacros.values());
        Files.writeString(dcmpFile.toPath(), "#include <stdbool.h>\n\n" + typeDeclarations +
            "\n\n" + concatHelpers + macros + "\n" + code, StandardCharsets.UTF_8);

        //export all instructions and raw pcode
        try (PrintWriter asm = new PrintWriter(asmFile); PrintWriter pcode = new PrintWriter(pcodeFile)) {

            for (Instruction instruction : currentProgram.getListing().getInstructions(true)) {
                String line = instruction.getAddress() + ": " + instruction;
                asm.println(line);
                pcode.println(line);

                PcodeOp[] ops = instruction.getPcode();

                for (int i = 0; i < ops.length; i++) {
                    pcode.println("    " + i + ": " + ops[i]);
                }
            }
        }

        System.out.println("Wrote decompiler output to: " + dcmpFile.getAbsolutePath());
        System.out.println("Wrote assembly output to: " + asmFile.getAbsolutePath());
        System.out.println("Wrote raw p-code output to: " + pcodeFile.getAbsolutePath());
    }
}
