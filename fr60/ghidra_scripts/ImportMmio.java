//@category FR60

import java.io.BufferedReader;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.ByteDataType;
import ghidra.program.model.data.UnsignedShortDataType;
import ghidra.program.model.data.UnsignedIntegerDataType;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.SourceType;

public class ImportMmio extends GhidraScript {
    @Override
    public void run() throws Exception {

        String[] args = getScriptArgs();
        Path csv = Path.of(args[0]);
        Memory memory = currentProgram.getMemory();

        try (BufferedReader reader = Files.newBufferedReader(csv, StandardCharsets.UTF_8)) {
            reader.readLine();

            String line;
            while ((line = reader.readLine()) != null) {
                monitor.checkCancelled();

                String[] fields = line.split(",", 6);
                String name = fields[0].trim();
                long address = Long.decode(fields[1].trim());
                int size = Integer.parseInt(fields[2].trim());
                DataType type;
                switch (size) {
                    case 1:
                        type = ByteDataType.dataType;
                        break;
                    case 2:
                        type = UnsignedShortDataType.dataType;
                        break;
                    case 3:
                        type = UnsignedIntegerDataType.dataType;
                        break;
                    default:
                        type = ByteDataType.dataType;
                }
                boolean read = Boolean.parseBoolean(fields[3].trim());
                boolean write = Boolean.parseBoolean(fields[4].trim());
                String note = fields[5].trim();

                create_register(memory, name, toAddr(address), type, note, read, write);
            }
        }
    }

    private void create_register(Memory mem, String name, Address addr, DataType type, String note, Boolean read, Boolean write) throws Exception {
        MemoryBlock block = mem.createUninitializedBlock(name, addr, type.getLength(), false);
        block.setRead(read);
        block.setWrite(write);
        block.setExecute(false);
        block.setVolatile(true);

        createData(addr, type);
        currentProgram.getSymbolTable().createLabel(addr, name, SourceType.USER_DEFINED);
        setEOLComment(addr, note);
    }
}
