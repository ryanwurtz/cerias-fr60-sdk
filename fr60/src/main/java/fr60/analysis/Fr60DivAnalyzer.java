package fr60.analysis;

import ghidra.app.services.*;
import ghidra.app.util.importer.MessageLog;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.address.AddressSetView;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.listing.Function.FunctionUpdateType;
import ghidra.program.model.listing.ParameterImpl;
import ghidra.program.model.listing.ReturnParameterImpl;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.IntegerDataType;
import ghidra.program.model.data.UnsignedIntegerDataType;
import ghidra.program.database.function.OverlappingFunctionException;
import ghidra.program.model.symbol.SourceType;
import ghidra.util.exception.DuplicateNameException;
import ghidra.util.exception.InvalidInputException;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
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

    //divsi3 = 39, udivsi3 = 36
    private static final int MAX_SEQUENCE_LENGTH = 39;

    private boolean matches(Instruction ins, String mnemonic, String... registers) {
        if (ins == null || !mnemonic.equalsIgnoreCase(ins.getMnemonicString()) || ins.getNumOperands() != registers.length) {
            return false;
        }
        for (int i = 0; i < registers.length; i++) {
            if (ins.getRegister(i) == null || !registers[i].equalsIgnoreCase(ins.getRegister(i).getName())) {
                return false;
            }
        }
        return true;
    }

    private Instruction nextInstruction(Instruction ins) {
        Instruction next = ins.getNext();
        return next != null && next.getAddress().equals(ins.getMaxAddress().next()) ? next : null;
    }

    private void buildFunction(Program program, Instruction start, TaskMonitor monitor, MessageLog log) throws CancelledException {
        Instruction ins = nextInstruction(start);
        boolean signed = matches(ins, "DIV0S", "r5");
        if (!signed && !matches(ins, "DIV0U", "r5")) {
            return;
        }

        for (int i = 0; i < 32; i++) {
            monitor.checkCancelled();
            ins = nextInstruction(ins);
            if (!matches(ins, "DIV1", "r5")) {
                return;
            }
        }

        if (signed) {
            ins = nextInstruction(ins);
            if (!matches(ins, "DIV2", "r5")) {
                return;
            }
            ins = nextInstruction(ins);
            if (!matches(ins, "DIV3")) {
                return;
            }
            ins = nextInstruction(ins);
            if (!matches(ins, "DIV4S")) {
                return;
            }
        }
        ins = nextInstruction(ins);
        if (!matches(ins, "MOV", "MDL", "r4")) {
            return;
        }
        ins = nextInstruction(ins);
        if (!matches(ins, "RET")) {
            return;
        }

        Address entry = start.getAddress();
        AddressSet body = new AddressSet(entry, ins.getMaxAddress());
        String name = signed ? "__divsi3" : "__udivsi3";
        FunctionManager functions = program.getFunctionManager();
        Function function = functions.getFunctionAt(entry);
        try {
            if (function == null) {
                //dont split an existing function
                if (functions.getFunctionsOverlapping(body).hasNext()) {
                    return;
                }
                function = functions.createFunction(name, entry, body, SourceType.ANALYSIS);
            }
            else if (function.getSymbol().getSource() == SourceType.DEFAULT) {
                //dont overwrite existing contents
                function.setName(name, SourceType.ANALYSIS);
            }
            if (function != null) {
                function.setCallFixup(signed ? "fr60_divsi3" : "fr60_udivsi3");
                DataType type = signed ? IntegerDataType.dataType : UnsignedIntegerDataType.dataType;
                function.updateFunction(
                    program.getCompilerSpec().getDefaultCallingConvention().getName(),
                    new ReturnParameterImpl(type, program),
                    FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS, true, SourceType.ANALYSIS,
                    new ParameterImpl("dividend", type, program),
                    new ParameterImpl("divisor", type, program));
            }
        }
        catch (InvalidInputException | OverlappingFunctionException | DuplicateNameException e) {
            log.appendMsg(getName(), "Could not configure helper at " + entry + ": " + e.getMessage());
        }
    }

    @Override
    public boolean added(Program program, AddressSetView set, TaskMonitor monitor, MessageLog log) throws CancelledException {
        InstructionIterator it = program.getListing().getInstructions(set, true);
        AddressSet checkedStarts = new AddressSet();
        Instruction previousInSet = null;

        //iterate through newly created instructions
        while (it.hasNext()) {
            monitor.checkCancelled();
            Instruction current = it.next();

            //look for a sequence start before this run's start
            if (previousInSet == null || !current.getAddress().equals(previousInSet.getMaxAddress().next())) {
                Instruction candidate = current;

                for (int i = 1; i < MAX_SEQUENCE_LENGTH; i++) {
                    monitor.checkCancelled();
                    Instruction previous = candidate.getPrevious();

                    if (previous == null || !candidate.getAddress().equals(previous.getMaxAddress().next())) {
                        break;
                    }

                    candidate = previous;
                    if (matches(candidate, "MOV", "r4", "MDL") && !checkedStarts.contains(candidate.getAddress())) {
                        checkedStarts.add(candidate.getAddress());
                        buildFunction(program, candidate, monitor, log);
                    }
                }
            }

            //check current instruction
            if (matches(current, "MOV", "r4", "MDL") && !checkedStarts.contains(current.getAddress())) {
                checkedStarts.add(current.getAddress());
                buildFunction(program, current, monitor, log);
            }
            previousInSet = current;
        }
        return true;
    }
}
