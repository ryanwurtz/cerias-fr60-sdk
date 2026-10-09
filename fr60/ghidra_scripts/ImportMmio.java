//@category FR60

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.UnsignedIntegerDataType;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.SourceType;

public class ImportMmio extends GhidraScript {
    @Override
    public void run() throws Exception {
        // Example addresses, not an actual FR60 peripheral map.
        Address base = toAddr(0x40000000L);
        Memory memory = currentProgram.getMemory();

        // Fresh-import example: fails if this range already overlaps a block.
        MemoryBlock block = memory.createUninitializedBlock(
            "UART0", base, 0x100, false);

        block.setRead(true);
        block.setWrite(true);
        block.setExecute(false);
        block.setVolatile(true);

        register(base, "UART0_DR",
            UnsignedIntegerDataType.dataType,
            "UART data register");

        register(base.add(4), "UART0_STATUS",
            UnsignedIntegerDataType.dataType,
            "UART status register");
    }

    private void register(Address address, String name,
                          DataType type, String description)
            throws Exception {
        createData(address, type);
        currentProgram.getSymbolTable().createLabel(
            address, name, SourceType.USER_DEFINED);
        setEOLComment(address, description);
    }
}
