//@category FR60

import java.nio.file.Path;
import java.util.HashMap;
import java.util.Map;

import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.address.AddressRange;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.ByteDataType;
import ghidra.program.model.data.UnsignedShortDataType;
import ghidra.program.model.data.UnsignedIntegerDataType;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.SourceType;

public class ImportMmio extends GhidraScript {

    @Override
    public void run() throws Exception {

        if (getScriptArgs().length != 1) {
            throw new IllegalArgumentException("Usage: ImportMmio.java <board.xml>");
        }

        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        Document document = factory.newDocumentBuilder().parse(Path.of(getScriptArgs()[0]).toFile());
        Element device = document.getDocumentElement();

        //parsing loop
        Map<String, Element> types = new HashMap<>();
        for (Node node = device.getFirstChild(); node != null; node = node.getNextSibling()) {
            monitor.checkCancelled();

            if (node instanceof Element element && element.getTagName().equals("memoryRegion")) {
                parse_memory_region(element);
            }

            if (node instanceof Element element && (element.getTagName().equals("peripheralType") || element.getTagName().equals("peripheral"))) {

                if (element.getTagName().equals("peripheralType")) {
                    String typeName = element.getAttribute("name");

                    if (typeName.isBlank() || types.putIfAbsent(typeName, element) != null) {
                        throw new IllegalArgumentException("Missing or duplicate peripheral type: " + typeName);
                    }
                }

                if (element.getTagName().equals("peripheral")) {
                    parse_peripheral(types, element);
                }
            }
        }
    }

    private void parse_memory_region(Element region) throws Exception {
        String name = region.getAttribute("name");
        Address base = toAddr(Long.decode(region.getAttribute("base")));
        long size = Long.decode(region.getAttribute("size"));
        String permissions = region.getAttribute("permissions");

        if (name.isBlank() || size <= 0 || !permissions.matches("[rwx]+")) {
            throw new IllegalArgumentException("Invalid memory region: " + name);
        }

        //fill gaps around existing elf sections
        AddressSet missing = new AddressSet(base, base.addNoWrap(size - 1));
        missing = missing.subtract(currentProgram.getMemory());
        int part = 0;

        for (AddressRange range : missing.getAddressRanges()) {
            MemoryBlock block = currentProgram.getMemory().createUninitializedBlock(part == 0 ? name : name + "_" + part, range.getMinAddress(), range.getLength(), false);

            block.setRead(permissions.contains("r"));
            block.setWrite(permissions.contains("w"));
            block.setExecute(permissions.contains("x"));
            block.setVolatile(false);
            part++;
        }
    }

    private void createLabel(String name, Address addr, DataType type, String note) throws Exception {
        createData(addr, type);
        currentProgram.getSymbolTable().createLabel(addr, name, SourceType.USER_DEFINED);
        setEOLComment(addr, note);
    }

    private void parse_register(Element register, String block_name, Address blockAddress) throws Exception {
        long offset = Long.decode(register.getAttribute("offset"));
        int size = Integer.parseInt(register.getAttribute("size"));

        String label = (block_name + "_" + register.getAttribute("name")).replaceAll("[^A-Za-z0-9_]", "_");
        String description = register.getAttribute("desc");

        DataType type;
        switch (size) {
            case 1:
                type = ByteDataType.dataType;
                break;

            case 2:
                type = UnsignedShortDataType.dataType;
                break;

            case 4:
                type = UnsignedIntegerDataType.dataType;
                break;

            default:
                throw new IllegalArgumentException("Unsupported register size " + size + " for " + label);
        }

        createLabel(label, blockAddress.addNoWrap(offset), type, description);
    }

    private void parse_peripheral(Map<String, Element> types, Element peripheral) throws Exception {
        //attribute parsing
        String name = peripheral.getAttribute("name");

        String baseText = peripheral.getAttribute("base");
        long base = baseText.isEmpty() ? 0 : Long.decode(baseText);

        String typeName = peripheral.getAttribute("type");
        Element definition = types.get(typeName);

        if (definition == null) {
            throw new IllegalArgumentException("Unknown peripheral type " + typeName + " for " + name);
        }

        int blockSize = Integer.parseInt(definition.getAttribute("size"));

        //create peripheral memoryblock
        Address blockAddress = toAddr(base);
        blockAddress.addNoWrap(blockSize - 1L);
        MemoryBlock block = currentProgram.getMemory().createUninitializedBlock(name.replaceAll("[^A-Za-z0-9_]", "_"), blockAddress, blockSize, false);

        block.setExecute(false);
        block.setVolatile(true);

        //register parsing
        for (Node child = definition.getFirstChild(); child != null; child = child.getNextSibling()) {
            if (!(child instanceof Element register) || !register.getTagName().equals("register")) {
                continue;
            }

            monitor.checkCancelled();
            parse_register(register, name, blockAddress);
        }
    }
}
