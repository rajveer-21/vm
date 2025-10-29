#ifndef chunk_h
#define chunk_h
#include "common.h"
#include "memory.h"
#include "value.h"
typedef enum
{
    OPCODE_CONSTANT,
    OPCODE_ADD,
    OPCODE_MUL,
    OPCODE_DIV,
    OPCODE_SUB,
    OPCODE_TRUE,
    OPCODE_FALSE,
    OPCODE_NIL,
    OPCODE_NOT,
    OPCODE_GREATER,
    OPCODE_LESSER,
    OPCODE_EQUAL_EQUAL,
    OPCODE_NOT_EQUAL,
    OPCODE_LESSER_EQUAL,
    OPCODE_GREATER_EQUAL,
    OPCODE_NEGATE,
    OPCODE_RETURN,
}OPCODE;
typedef struct 
{
    int occupied;
    int capacity;
    uint8_t* stuff;
    int* lines;
    ValueArray constants_array;
}CHUNK;
void initializeCHUNK(CHUNK* chunk);
void writeToCHUNK(CHUNK* chunk, uint8_t current, int line);
void freeUpCHUNK(CHUNK* chunk);
int addConstant(CHUNK* chunk, VALUE value);
void initializeCHUNK(CHUNK* chunk)
{
    chunk->occupied = 0;
    chunk->capacity = 0;
    chunk->stuff = NULL;
    chunk->lines = NULL;
    initializeValArr(&chunk->constants_array);
}
void writeToCHUNK(CHUNK* chunk, uint8_t value, int line)
{
    if(chunk->occupied + 1 > chunk->capacity)
    {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->stuff = GROW_DYNAMIC_ARRAY(uint8_t, chunk->stuff, oldCapacity, chunk->capacity);
        chunk->lines = GROW_DYNAMIC_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
    }
    chunk->stuff[chunk->occupied] = value;
    chunk->lines[chunk->occupied] = line;
    chunk->occupied = chunk->occupied + 1;
}
void freeUpCHUNK(CHUNK* chunk)
{
    FREE_DYNAMIC_ARRAY(uint8_t, chunk->stuff, chunk->capacity);
    FREE_DYNAMIC_ARRAY(int, chunk->lines, chunk->capacity);
    freeUpValArr(&chunk->constants_array);
    initializeCHUNK(chunk);
}
int addConstant(CHUNK* chunk, VALUE value)
{
    writeToValArr(&chunk->constants_array, value);
    return chunk->constants_array.occupied - 1;
}
#endif