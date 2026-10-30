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

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.address.AddressSetView;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.DataTypeManager;
import ghidra.program.model.data.DataTypeWriter;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.pcode.PcodeOp;
import ghidra.program.model.pcode.HighSymbol;
import ghidra.program.model.symbol.SourceType;

public class ExportDecompiled extends GhidraScript {

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

                bodies.append(result.getDecompiledFunction().getC()).append("\n\n");
                
                //parse global defs
                var symbols = result.getHighFunction().getGlobalSymbolMap().getSymbols();
                while (symbols.hasNext()) {
                    monitor.checkCancelled();
                    var symbol = symbols.next();
                    String name = symbol.getName();

                    //emit macro if this is an MMIO symbol
                    String macro = emit_mmio_macro(symbol);
                    if (macro != null) {
                        macros.put(name, macro);
                        continue;
                    }

                    String type = symbol.getDataType().getDisplayName();
                    declarations.add(type + " " + name + ";");
                }
            }
        }
        finally {
            decompiler.dispose();
        }

        return String.join("\n", declarations) + "\n\n" + bodies;
    }

    @Override
    public void run() throws Exception {

        String[] args = getScriptArgs();
        File dcmpFile = new File(args[0]);
        File asmFile = new File(args[1]);
        File pcodeFile = new File(args[2]);

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

        Map<String, String> mmioMacros = new LinkedHashMap<>();
        String code = emit_c(cExportAddresses, mmioMacros);

        String macros = String.join("", mmioMacros.values());
        Files.writeString(dcmpFile.toPath(), declarations + "\n\n" + macros + "\n" + code, StandardCharsets.UTF_8);

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
