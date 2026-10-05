//@category Export

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;

public class ExportDecompiled extends GhidraScript {

    @Override
    public void run() throws Exception {

        String[] args = getScriptArgs();
        File outputFile = new File(getScriptArgs()[0]);

        DecompInterface decompiler = new DecompInterface();
        decompiler.openProgram(currentProgram);

        FunctionManager fm = currentProgram.getFunctionManager();

        try (PrintWriter out = new PrintWriter(outputFile)) {

            for (Function function : fm.getFunctions(true)) {

                if (monitor.isCancelled()) {
                    break;
                }

                DecompileResults result = decompiler.decompileFunction(function, 60, monitor);

                out.print("// " + function.getName() + " @ 0x" + function.getEntryPoint());

                if (result.decompileCompleted()) {
                    String code = result.getDecompiledFunction().getC();
                    out.print(code.replace("\n\n{", "\n{"));
                }
                else {
                    out.println("//Decompilation failed: " + result.getErrorMessage());
                }
            }
        }

        decompiler.dispose();

        System.out.println("Wrote decompiler output to: " + outputFile.getAbsolutePath());
    }
}
