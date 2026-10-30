//@category FR60

import ghidra.app.script.GhidraScript;

public class ConfigureAnalysis extends GhidraScript {
    @Override
    public void run() throws Exception {
        setAnalysisOption(currentProgram, "Decompiler Parameter ID", "true");
        setAnalysisOption(currentProgram, "Decompiler Parameter ID.Commit Data Types", "true");
        setAnalysisOption(currentProgram, "Decompiler Parameter ID.Analysis Clear Level", "DEFAULT");
        setAnalysisOption(currentProgram, "DWARF", "false");
    }
}
