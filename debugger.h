#ifndef debugger_h
#define debugger_h
#include "chunk.h"
#include "memory.h"
#include "value.h"
#include "common.h"

void disassembleCHUNK(CHUNK* chunk, const char* name);
int disassembleInstruction(CHUNK* chunk, int offset);
int instructionType1(char* opcode_name, int offset);
int instructionType2(char* opcode_name, CHUNK* chunk, int offset);
void printVal(VALUE value);

void disassembleCHUNK(CHUNK* chunk, const char* name)
{
    printf("%s\n", name);
    int i = 0;
    while(i < chunk->occupied)
    {
        i = disassembleInstruction(chunk, i);
    }
}

int disassembleInstruction(CHUNK* chunk, int offset)
{
    printf("%04d ", offset);
    uint8_t instruction = chunk->stuff[offset];
    switch(instruction)
    {
        case OPCODE_CONSTANT:
        return instructionType2("OPCODE_CONSTANT", chunk, offset);
        case OPCODE_ADD:
        return instructionType1("OPCODE_ADD", offset);
        case OPCODE_SUB:
        return instructionType1("OPCODE_SUB", offset);
        case OPCODE_MUL:
        return instructionType1("OPCODE_MUL", offset);
        case OPCODE_DIV:
        return instructionType1("OPCODE_DIV", offset);
        case OPCODE_NEGATE:
        return instructionType1("OPCODE_NEGATE", offset);
        case OPCODE_RETURN:
        return instructionType1("OPCODE_RETURN", offset);
        case OPCODE_NIL:
        return instructionType1("OPCODE_NIL", offset);
        case OPCODE_TRUE:
        return instructionType1("OPCODE_TRUE", offset);
        case OPCODE_FALSE:
        return instructionType1("OPCODE_FALSE", offset);
        case OPCODE_NOT:
        return instructionType1("OPCODE_NOT", offset);
        default:
        return instructionType1("Unknown OPCODE", offset);
    }
}

int instructionType1(char* opcode_name, int offset)
{
    printf("%s\n", opcode_name);
    return offset + 1;
}
int instructionType2(char* opcode_name, CHUNK* chunk, int offset)
{
    uint8_t constant = chunk->stuff[offset + 1];
    printf("%-16s %4d",opcode_name, constant);
    printVal(chunk->constants_array.stuff[constant]);
    printf("\n");
    return offset + 2;
}

void printVal(VALUE value)
{
    switch (value.valuetype) 
    {
    case VAL_BOOL:
    printf(GET_BOOL(value) ? "true" : "false");break;
    case VAL_NIL: printf("nil"); break;
    case VAL_NUM: printf("%g", GET_NUM(value)); break;
    }
}
#endif