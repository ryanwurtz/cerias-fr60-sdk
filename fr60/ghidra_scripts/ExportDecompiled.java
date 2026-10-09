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

            for (Function function : fm.getFunctions(true)) {

                DecompileResults result = decompiler.decompileFunction(function, 60, monitor);

                dcmp.println("// " + function.getName() + " @ 0x" + function.getEntryPoint());
                if (result.decompileCompleted()) {
                    String code = result.getDecompiledFunction().getC();
                    dcmp.print(code.replace("\n\n{", "\n{"));
                }
                else {
                    dcmp.println("//Decompilation failed: " + result.getErrorMessage());
                }
                dcmp.println();

                String header = "# " + function.getName() + " @ 0x" + function.getEntryPoint();
                asm.println(header);
                pcode.println(header);
                for (Instruction instruction : currentProgram.getListing()
                        .getInstructions(function.getBody(), true)) {
                    String line = instruction.getAddress() + ": " + instruction;
                    asm.println(line);
                    pcode.println(line);
                    PcodeOp[] ops = instruction.getPcode();
                    for (int i = 0; i < ops.length; i++) {
                        pcode.println("    " + i + ": " + ops[i]);
                    }
                    if (ops.length == 0) {
                        pcode.println("    # No p-code operations");
                    }
                }
                asm.println();
                pcode.println();
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
