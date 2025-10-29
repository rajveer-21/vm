#ifndef compiler_h
#define compiler_h
#include "common.h"
#include "chunk.h"
#include "value.h"
#include "memory.h"
#include "scanner.h"
#ifdef DEBUG_PRINT_BYTECODE
#include "debugger.h"
#endif

typedef void (*PFN)();
typedef enum 
{
    P_NONE,
    P_ASSIGNMENT,
    P_OR,
    P_AND,
    P_EQUALITY,
    P_COMPARISON,
    P_TERM,
    P_FACTOR,
    P_UNARY,
    P_CALL,
    P_PRIMARY
}PRECEDENCE;
typedef struct 
{
    Token previous;
    Token current;
    bool hadError;
    bool panError;
}PARSER;
typedef struct 
{
    PFN prefix;
    PFN infix;
    PRECEDENCE precedence;
}PRULE;
PARSER parser;
CHUNK* currentCChunk;
void advancePARSER();
CHUNK* currentCCHUNK();
bool compile(const char* source, CHUNK* chunk);
void consume(TokenType tokentype, const char* emessage);
void reportErrorAt1(const char* emessage);
void reportErrorAt2(const char* emessage);
void reportErrorAt3(const char* emessage, Token* token);
void emitByteU(uint8_t byte1);
void emitByteR();
void emitByteB(uint8_t byte1, uint8_t byte2);
void emitByteC(VALUE value);
void numberPARSER();
void stringPARSER();
void expression();
void grouping();
void unary();
void binary();
void parseByPRECEDENCE(PRECEDENCE precedence);
void literal();
uint8_t createConstant(VALUE value);
PRULE* getRule(TokenType type);
PRULE rules[] = {
  [LEFT_PARENTHESIS]    = {grouping, NULL,   P_NONE},
  [RIGHT_PARENTHESIS]   = {NULL,     NULL,   P_NONE},
  [LEFT_BRACE]          = {NULL,     NULL,   P_NONE}, 
  [RIGHT_BRACE]         = {NULL,     NULL,   P_NONE},
  [COMMA]               = {NULL,     NULL,   P_NONE},
  [DOT]             = {NULL,     NULL,   P_NONE},
  [SUB]             = {unary,    binary, P_TERM},
  [ADD]             = {NULL,     binary, P_TERM},
  [SEMICOLON]       = {NULL,     NULL,   P_NONE},
  [DIV]             = {NULL,     binary, P_FACTOR},
  [MUL]             = {NULL,     binary, P_FACTOR},
  [NOT]             = {unary,     NULL,   P_NONE},
  [NOT_EQUAL]       = {NULL,     binary,   P_EQUALITY},
  [EQUAL]           = {NULL,     NULL,   P_NONE},
  [EQUAL_EQUAL]     = {NULL,     binary,   P_EQUALITY},
  [GREATER]         = {NULL,     binary,   P_COMPARISON},
  [GREATER_EQUAL]   = {NULL,     binary,   P_COMPARISON},
  [LESSER]          = {NULL,     binary,   P_COMPARISON},
  [LESSER_EQUAL]    = {NULL,     binary,   P_COMPARISON},
  [IDENTIFIER]    = {NULL,     NULL,   P_NONE},
  [STRING]        = {stringPARSER,     NULL,   P_NONE},
  [NUMBER]        = {numberPARSER,   NULL,   P_NONE},
  [AND]           = {NULL,     NULL,   P_NONE},
  [CLASS]         = {NULL,     NULL,   P_NONE},
  [ELSE]          = {NULL,     NULL,   P_NONE},
  [FALSE]         = {literal,     NULL,   P_NONE},
  [FOR]           = {NULL,     NULL,   P_NONE},
  [FUN]           = {NULL,     NULL,   P_NONE},
  [IF]            = {NULL,     NULL,   P_NONE},
  [NIL]           = {literal,     NULL,   P_NONE},
  [OR]            = {NULL,     NULL,   P_NONE},
  [PRINT]         = {NULL,     NULL,   P_NONE},
  [RETURN]        = {NULL,     NULL,   P_NONE},
  [SUPER]         = {NULL,     NULL,   P_NONE},
  [THIS]          = {NULL,     NULL,   P_NONE},
  [TRUE]          = {literal,     NULL,   P_NONE},
  [VAR]           = {NULL,     NULL,   P_NONE},
  [WHILE]         = {NULL,     NULL,   P_NONE},
  [ERROR]         = {NULL,     NULL,   P_NONE},
  [EOF_TOKEN]           = {NULL,     NULL,   P_NONE},
};

bool compile(const char* source, CHUNK* chunk)
{
    initializeSCANNER(source);
    currentCChunk = chunk;
    parser.hadError = false;
    parser.panError = false;
    advancePARSER();
    expression();
    consume(EOF_TOKEN, "Expected end of expression.");
    emitByteR();
    #ifdef DEBUG_PRINT_BYTECODE
    {
    if(parser.hadError == false)
    disassembleCHUNK(currentCCHUNK(), "TEST CHONKUS");
    }
    #endif
    return !parser.hadError;
}
CHUNK* currentCCHUNK()
{
    return currentCChunk;
}
void advancePARSER()
{
    parser.previous = parser.current;
    while(true)
    {
        parser.current = scanToken();
        if(parser.current.type != ERROR) 
        break;
        reportErrorAt1(parser.current.start);
    }
}
void consume(TokenType tokentype, const char* emessage)
{
    if(parser.current.type == tokentype)
    {
        advancePARSER();
        return;
    }
    reportErrorAt1(emessage);
}
void reportErrorAt1(const char* emessage)
{
    reportErrorAt3(emessage, &parser.current);
}
void reportErrorAt2(const char* emessage)
{
    reportErrorAt3(emessage, &parser.previous);
}
void reportErrorAt3(const char* emessage, Token* token)
{
    if(parser.panError == true)
    return;
    fprintf(stderr, "Error at line %d.", token->line);
    parser.panError = true;
    if(token->type == EOF_TOKEN)
    {
        fprintf(stderr, "AT END of source.");
    }
    else if(token->type == ERROR)
    {

    }
    else 
    {
        fprintf(stderr, " of token name : '%.*s'", token->length, token->start);
    }
    fprintf(stderr, " of type : %s\n", emessage);
    parser.hadError = true;
}
void emitByteU(uint8_t byte1)
{
    writeToCHUNK(currentCCHUNK(), byte1, parser.previous.line);
}
void emitByteB(uint8_t byte1, uint8_t byte2)
{
    emitByteU(byte1);
    emitByteU(byte2);
}
void emitByteC(VALUE value)
{
    emitByteB(OPCODE_CONSTANT, createConstant(value));
}
void emitByteR()
{
    emitByteU(OPCODE_RETURN);
}
void numberPARSER()
{
    double number_value = strtod(parser.previous.start, NULL);
    emitByteC(NUM_VAL(number_value));
}
void stringPARSER()
{
    
}
uint8_t createConstant(VALUE value)
{
    int constant = addConstant(currentCCHUNK(), value);
    if(constant > UINT8_MAX)
    {
        reportErrorAt1("Can't add any more constants to given chunk.");
        return 0;
    }
    return (uint8_t)constant;
}
void grouping()
{
    expression();
    consume(RIGHT_PARENTHESIS, "Expected ')' at end of parenthesized expression.");
}
void unary()
{
    TokenType operatorType = parser.previous.type;
    parseByPRECEDENCE(P_UNARY);
    switch(operatorType)
    {
        case SUB:
        {
            emitByteU(OPCODE_NEGATE);
            break;
        }
        case NOT:
        {
            emitByteU(OPCODE_NOT);
            break;
        }
        default:
        return;
    }
}
void binary()
{
    TokenType operatorType = parser.previous.type;
    PRULE* rule = getRule(operatorType);
    parseByPRECEDENCE((PRECEDENCE)(rule->precedence + 1));
    switch(operatorType)
    {
        case ADD:
        emitByteU(OPCODE_ADD); break;
        case SUB:
        emitByteU(OPCODE_SUB); break;
        case DIV:
        emitByteU(OPCODE_DIV); break;
        case MUL:
        emitByteU(OPCODE_MUL); break;
        case EQUAL_EQUAL:
        emitByteU(OPCODE_EQUAL_EQUAL); break;
        case NOT_EQUAL:
        emitByteU(OPCODE_NOT_EQUAL); break;
        case LESSER_EQUAL:
        emitByteU(OPCODE_LESSER_EQUAL); break;
        case GREATER_EQUAL:
        emitByteU(OPCODE_GREATER_EQUAL); break;
        case GREATER:
        emitByteU(OPCODE_GREATER); break;
        case LESSER:
        emitByteU(OPCODE_LESSER); break;
        default:
        return;
    }
}
void literal()
{
    switch(parser.previous.type)
    {
        case OPCODE_TRUE : emitByteU(OPCODE_TRUE); break;
        case OPCODE_FALSE : emitByteU(OPCODE_FALSE); break;
        case OPCODE_NIL : emitByteU(OPCODE_NIL); break;
        default: return;
    }
}
void expression()
{
    parseByPRECEDENCE(P_ASSIGNMENT);
}
PRULE* getRule(TokenType type) 
{
  return &rules[type];
}
void parseByPRECEDENCE(PRECEDENCE precedence)
{
    advancePARSER();
    PFN prefix_rule = getRule(parser.previous.type)->prefix;
    if(prefix_rule == NULL)
    {
        reportErrorAt1("Expected Expression.");
        return;
    }
    prefix_rule();
    while(precedence <= getRule(parser.current.type)->precedence)
    {
        advancePARSER();
        PFN infix_rule = getRule(parser.previous.type)->infix;
        infix_rule();
    }
}
#endif