//@category Export

import java.io.File;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.util.List;

import ghidra.app.util.Option;

import ghidra.app.util.exporter.CppExporter;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.pcode.PcodeOp;

public class ExportDecompiled extends GhidraScript {

    @Override
    public void run() throws Exception {

        String[] args = getScriptArgs();
        File dcmpFile = new File(args[0]);
        File asmFile = new File(args[1]);
        File pcodeFile = new File(args[2]);

        CppExporter exporter = new CppExporter();
        exporter.setOptions(List.of(
            new Option(CppExporter.EMIT_TYPE_DEFINITONS, false),
            new Option(CppExporter.EMIT_REFERENCED_GLOBALS, true)));
        if (!exporter.export(dcmpFile, currentProgram, null, monitor)) {
            throw new IllegalStateException("C/C++ export failed: " + exporter.getMessageLog());
        }

        // Supply FR60 scalar aliases without exporting unrelated program structures.
        String typedefs = """
            typedef unsigned char byte;
            typedef unsigned short ushort;
            typedef unsigned int uint;
            typedef unsigned long ulong;
            typedef long long longlong;
            typedef unsigned long long ulonglong;
            typedef unsigned short word;
            typedef unsigned int dword;
            typedef unsigned long long qword;
            typedef unsigned char undefined;
            typedef unsigned char undefined1;
            typedef unsigned short undefined2;
            typedef unsigned int undefined4;
            typedef unsigned long long undefined8;

            """;
        String code = Files.readString(dcmpFile.toPath(), StandardCharsets.UTF_8);
        Files.writeString(dcmpFile.toPath(), typedefs + code, StandardCharsets.UTF_8);

        try (PrintWriter asm = new PrintWriter(asmFile);
             PrintWriter pcode = new PrintWriter(pcodeFile)) {

            // Dump all defined instructions in address order, including outside functions.
            for (Instruction instruction : currentProgram.getListing().getInstructions(true)) {
                String line = instruction.getAddress() + ": " + instruction;
                asm.println(line);
                pcode.println(line);

                PcodeOp[] ops = instruction.getPcode();
                for (int i = 0; i < ops.length; i++) {
                    pcode.println("    " + i + ": " + ops[i]);
                }
            }
        }

        System.out.println("Wrote decompiler output to: " + dcmpFile.getAbsolutePath());
        System.out.println("Wrote assembly output to: " + asmFile.getAbsolutePath());
        System.out.println("Wrote raw p-code output to: " + pcodeFile.getAbsolutePath());
    }
}
