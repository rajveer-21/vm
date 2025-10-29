#ifndef virtual_machine_h
#define virtual_machine_h
#include <stdarg.h>
#include "chunk.h"
#include "debugger.h"
#include "compiler.h"
#include "common.h"
#define STACK_MAX 256
typedef struct 
{
    CHUNK* chunk;
    uint8_t* ip;
    VALUE stack[STACK_MAX];
    VALUE* stackTop;
}VirtualMachine;
typedef enum 
{
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
    INTERPRET_OK,
}IResult;
void initializeVM();
void resetStack();
void push(VALUE value);
VALUE pop();
void freeUpVM();
IResult interpret(const char* source);
IResult run();
void runtimeError(const char* format, ...);
VALUE peekPARSER(int fall);
bool equalityEE(VALUE a, VALUE b);
bool equalityGE(VALUE a, VALUE b);
bool equalityG(VALUE a, VALUE b);
bool equalityLE(VALUE a, VALUE b);
bool equalityL(VALUE a, VALUE b);
bool equalityNE(VALUE a, VALUE b);
bool isFalse(VALUE value);
VirtualMachine vm;
void initializeVM()
{
    resetStack();
}
void freeUpVM()
{

}
void push(VALUE value)
{
    *vm.stackTop = value;
    vm.stackTop = vm.stackTop + 1;
}
VALUE pop()
{
    vm.stackTop--;
    return *vm.stackTop;
}
void resetStack()
{
    vm.stackTop = vm.stack;
}
void runtimeError(const char* format, ...) 
{
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  size_t instruction = vm.ip - vm.chunk->stuff - 1;
  int line = vm.chunk->lines[instruction];
  fprintf(stderr, "[line %d] in script\n", line);
  resetStack();
}
IResult interpret(const char* source)
{
    CHUNK chunk;
    initializeCHUNK(&chunk);
    if(compile(source, &chunk) == false)
    {
        freeUpCHUNK(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }
    vm.chunk = &chunk;
    vm.ip = vm.chunk->stuff;
    IResult interpret_result = run();
    freeUpCHUNK(&chunk);
    return interpret_result;
}

IResult run()
{
    #define GET_BYTE()(*vm.ip++)
    #define GET_CONSTANT()(vm.chunk->constants_array.stuff[GET_BYTE()])
    #define BINARY_OP(valuetype, optype) do {if(!IS_NUM(peekPARSER(0)) || !IS_NUM(peekPARSER(1))){runtimeError("Operands must be numbers."); return INTERPRET_RUNTIME_ERROR;}double b = GET_NUM(pop()); double a = GET_NUM(pop()); push(valuetype(a optype b));}while(false)
    while(true)
    {
        uint8_t instruction = GET_BYTE();
        switch(instruction)
        {
            case OPCODE_CONSTANT:
            {
                VALUE value = GET_CONSTANT();
                push(value);
                break;
            }
            case OPCODE_ADD: BINARY_OP(NUM_VAL, +); break;
            case OPCODE_SUB: BINARY_OP(NUM_VAL, -); break;
            case OPCODE_MUL: BINARY_OP(NUM_VAL, *); break;
            case OPCODE_DIV: BINARY_OP(NUM_VAL, /); break;
            case OPCODE_NEGATE:
            {
                if(!IS_NUM(peekPARSER(0)))
                {
                    runtimeError("Needs to be a number.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(NUM_VAL(-GET_NUM(pop())));
                break;
            }
            case OPCODE_NIL: push(NIL_VAL()); break;
            case OPCODE_TRUE: push(BOOL_VAL(true)); break;
            case OPCODE_FALSE: push(BOOL_VAL(false)); break;
            case OPCODE_RETURN:
            {
                printVal(pop());
                printf("\n");
                return INTERPRET_OK;
            }
            case OPCODE_NOT:
            {
                push(BOOL_VAL(isFalse(pop())));
                break;
            }
            case OPCODE_EQUAL_EQUAL:
            {
                VALUE b = pop();
                VALUE a = pop();
                push(BOOL_VAL(equalityEE(a, b)));
                break;
            }
            case OPCODE_NOT_EQUAL:
            {
                VALUE b = pop();
                VALUE a = pop();
                push(BOOL_VAL(equalityNE(a, b)));
                break;
            }
            case OPCODE_GREATER_EQUAL:
            {
                VALUE b = pop();
                VALUE a = pop();
                if(a.valuetype != VAL_NUM || b.valuetype != VAL_NUM)
                {
                    runtimeError("Operands must be numbers!");
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(BOOL_VAL(equalityGE(a, b)));
                break;
            }
            case OPCODE_GREATER:
            {
                VALUE b = pop();
                VALUE a = pop();
                if(a.valuetype != VAL_NUM || b.valuetype != VAL_NUM)
                {
                    runtimeError("Operands must be numbers!");
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(BOOL_VAL(equalityG(a, b)));
                break;
            }
            case OPCODE_LESSER_EQUAL:
            {
                VALUE b = pop();
                VALUE a = pop();
                if(a.valuetype != VAL_NUM || b.valuetype != VAL_NUM)
                {
                    runtimeError("Operands must be numbers!");
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(BOOL_VAL(equalityLE(a, b)));
                break;
            }
            case OPCODE_LESSER:
            {
                VALUE b = pop();
                VALUE a = pop();
                if(a.valuetype != VAL_NUM || b.valuetype != VAL_NUM)
                {
                    runtimeError("Operands must be numbers!");
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(BOOL_VAL(equalityL(a, b)));
                break;
            }
        }
    }
    #undef GET_BYTE
    #undef GET_CONSTANT
    #undef BINARY_OP
}
bool equalityEE(VALUE a, VALUE b)
{
    if(a.valuetype != b.valuetype)
    return false;
    switch(a.valuetype)
    {
        case VAL_BOOL : return GET_BOOL(a) == GET_BOOL(b);
        case VAL_NIL : return true;
        case VAL_NUM : return GET_NUM(a) == GET_NUM(b);
        default: return false;
    }
}
bool equalityNE(VALUE a, VALUE b)
{
    if(a.valuetype != b.valuetype)
    return true;
    switch(a.valuetype)
    {
        case VAL_BOOL : return GET_BOOL(a) != GET_BOOL(b);
        case VAL_NIL : return false; 
        case VAL_NUM : return GET_NUM(a) != GET_NUM(b);
        default : return true;
    }
}
bool equalityGE(VALUE a, VALUE b)
{
    return GET_NUM(a) >= GET_NUM(b);
}
bool equalityG(VALUE a, VALUE b)
{
    return GET_NUM(a) > GET_NUM(b);
}
bool equalityLE(VALUE a, VALUE b)
{
    return GET_NUM(a) <= GET_NUM(b);
}
bool equalityL(VALUE a, VALUE b)
{
    return GET_NUM(a) < GET_NUM(b);
}
VALUE peekPARSER(int fall)
{
    return vm.stackTop[-1 - fall];
}
bool isFalse(VALUE value)
{
    if(IS_NIL(value) || (IS_BOOL(value) && !GET_BOOL(value)))
    return true;
    return false;
}
#endif