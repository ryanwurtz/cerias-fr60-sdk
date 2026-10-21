//@category Analysis

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;

public class SeedBootEntry extends GhidraScript {
    @Override
    public void run() throws Exception {
        Address entry = toAddr(getScriptArgs()[0]);
        if (!currentProgram.getMemory().contains(entry) ||
            !currentProgram.getMemory().getBlock(entry).isInitialized()) {
            throw new IllegalStateException("Boot entry is outside the imported image: " + entry);
        }
        currentProgram.getSymbolTable().addExternalEntryPoint(entry);
        if (!disassemble(entry)) {
            throw new IllegalStateException("Cannot disassemble boot entry: " + entry);
        }
        if (getFunctionAt(entry) == null && createFunction(entry, null) == null) {
            throw new IllegalStateException("Cannot create boot entry function: " + entry);
        }
    }
}
