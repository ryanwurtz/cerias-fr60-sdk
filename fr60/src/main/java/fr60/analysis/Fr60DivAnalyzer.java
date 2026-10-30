package fr60.analysis;

import ghidra.app.services.*;
import ghidra.app.util.importer.MessageLog;
import ghidra.program.model.address.AddressSetView;
import ghidra.program.model.listing.Program;
import ghidra.util.exception.CancelledException;
import ghidra.util.task.TaskMonitor;

public class Fr60DivAnalyzer extends AbstractAnalyzer {

    public Fr60DivAnalyzer() {
        super("FR60 Division Helpers", "Recognizes GCC division helpers", AnalyzerType.INSTRUCTION_ANALYZER);
        setDefaultEnablement(true);
        setPriority(AnalysisPriority.CODE_ANALYSIS.after());
    }

    @Override
    public boolean canAnalyze(Program program) {
        return program.getLanguageID().toString().equals("FR:BE:32:60");
    }

    @Override
    public boolean added(Program program, AddressSetView set, TaskMonitor monitor, MessageLog log) throws CancelledException {

        

        return true;
    }
}