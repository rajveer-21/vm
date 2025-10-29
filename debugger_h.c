#ifndef debugger_h
#define debugger_h
#include "chunk.h"
#include "memory.h"
#include "common.h"

void disassembleCHUNK(CHUNK* chunk, const char* name);
int disassembleInstruction(CHUNK* chunk, int offset);

void disassembleCHUNK(CHUNK* chunk, const char* name)
{
    printf("%s\n", name);
    int i = 0;
    while(i < chunk->occupied)
    {
        i = disassembleInstruction(chunk, i);
    }
}

int diassembleInstruction(CHUNK* chunk, int offset)
{
    printf("%04d", offset);
    uint8_t instruction = chunk->stuff[offset];
    switch(instruction)
    {
        case OPCODE_RETURN:
        return instructionType1("OPCODE_RETURN", offset);
        default:
        return instructionType1("Unknown OPCODE", offset);
    }
}

int instructionType1(char* opcode_name, int offset)
{
    printf("%s", opcode_name);
    return offset + 1;
}
#endif