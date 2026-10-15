//@category FR60

import ghidra.app.script.GhidraScript;

public class DisableDwarf extends GhidraScript {
    @Override
    public void run() throws Exception {
        setAnalysisOption(currentProgram, "DWARF", "false");
    }
}
