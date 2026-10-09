//@category Export

import java.io.File;
import java.io.PrintWriter;

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.pcode.PcodeOp;

public class ExportDecompiled extends GhidraScript {

    @Override
    public void run() throws Exception {

        String[] args = getScriptArgs();
        File dcmpFile = new File(args[0]);
        File asmFile = new File(args[1]);
        File pcodeFile = new File(args[2]);

        DecompInterface decompiler = new DecompInterface();
        FunctionManager fm = currentProgram.getFunctionManager();

        try (PrintWriter dcmp = new PrintWriter(dcmpFile);
             PrintWriter asm = new PrintWriter(asmFile);
             PrintWriter pcode = new PrintWriter(pcodeFile)) {

            if (!decompiler.openProgram(currentProgram)) {
                throw new IllegalStateException(
                    "Cannot initialize decompiler: " + decompiler.getLastMessage());
            }
                
            //dump decompiler output by function
            for (Function function : fm.getFunctions(true)) {

                DecompileResults result = decompiler.decompileFunction(function, 60, monitor);

                dcmp.println("// " + function.getName() + " @ 0x" + function.getEntryPoint());
                if (result.decompileCompleted()) {
                    String code = result.getDecompiledFunction().getC();
                    code = code.strip();
                    code = code.replaceFirst("\\R\\s*\\{", " {");
                    dcmp.println(code);
                }
                else {
                    dcmp.println("//Decompilation failed: " + result.getErrorMessage());
                }
                dcmp.println();
            }

            // Dump all defined instructions in address order, including outside functions.
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
        finally {
            decompiler.dispose();
        }

        System.out.println("Wrote decompiler output to: " + dcmpFile.getAbsolutePath());
        System.out.println("Wrote assembly output to: " + asmFile.getAbsolutePath());
        System.out.println("Wrote raw p-code output to: " + pcodeFile.getAbsolutePath());
    }
}
