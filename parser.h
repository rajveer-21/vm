#ifndef vm_compiler_h
#define vm_compiler_h
#include "scanner.h"

bool compile(const char* input, CHUNK* chunk);

typedef void (*ParseFn)();
typedef struct
{
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
}PARSER;

typedef enum 
{
    PRECEDENCE_NONE,
    PRECEDENCE_ASSIGNMENT, 
    PRECEDENCE_OR,
    PRECEDENCE_AND,
    PRECEDENCE_EQUALITY,
    PRECEDENCE_COMPARISON,
    PRECEDENCE_TERM,
    PRECEDENCE_FACTOR,
    PRECEDENCE_UNARY,
    PRECEDENCE_CALL,
    PRECEDENCE_PRIMARY
}PRECEDENCE;

typedef struct 
{
    ParseFn prefix;
    ParseFn infix;
    PRECEDENCE precedence;
}PARSERULE;
PARSER parser;
CHUNK* compilingChunk;

PARSERULE rules[] = {
  [TOKEN_LEFT_PAREN]    = {grouping, NULL,   PRECEDENCE_NONE},
  [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PRECEDENCE_NONE}, 
  [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_COMMA]         = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_DOT]           = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_MINUS]         = {unary,    binary, PRECEDENCE_TERM},
  [TOKEN_PLUS]          = {NULL,     binary, PRECEDENCE_TERM},
  [TOKEN_SLASH]         = {NULL,     binary, PRECEDENCE_FACTOR},
  [TOKEN_STAR]          = {NULL,     binary, PRECEDENCE_FACTOR},
  [TOKEN_NOT]          = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_NOT_EQUAL]    = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_EQUAL]         = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_EQUAL_EQUAL]   = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_GREATER]       = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_GREATER_EQUAL] = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_LESSER]          = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_LESSER_EQUAL]    = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_IDENTIFIER]    = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_STRINGTYPE]        = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_NUMBERTYPE]        = {number,   NULL,   PRECEDENCE_NONE},
  [TOKEN_AND]           = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_CLASS]         = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_ELSE]          = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_FALSE]         = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_FOR]           = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_FUN]           = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_IF]            = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_NIL]           = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_OR]            = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_PRINT]         = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_RETURN]        = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_SUPER]         = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_THIS]          = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_TRUE]          = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_VAR]           = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_WHILE]         = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_ERROR]         = {NULL,     NULL,   PRECEDENCE_NONE},
  [TOKEN_EOF]           = {NULL,     NULL,   PRECEDENCE_NONE},
};

static PARSERULE* getRule(TokenType token)
{
    return &rules[token];
}

bool compile(const char* input, CHUNK* chunk)
{
    initializeSCANNER(input);
    parser.hadError = false;
    parser.panicMode = false;
    compilingChunk = chunk;
    advance();
    expression();
    consume(TOKEN_EOF, "End of expression was expected");
    emitByte(OPCODE_RETURN);
    return !parser.hadError;
}

CHUNK* currentChunk()
{
    return compilingChunk;
}

static void advance()
{
    parser.previous = parser.current;
    while(true)
    {
        parser.current = scanToken();
        if(parser.current.type != TOKEN_ERROR) break;
        errorAtCurrent("Expected end of expression.");
    }
}

static void consume(TokenType type, const char* message)
{
    if(parser.current.type == type)
    {
        advance();
        return;
    }
    errorAtCurrent(message);
}

static void errorAtCurrent(char* message)
{
    errorAt(&parser.current, message);
}

static void error(char* message)
{
    errorAt(&parser.previous, message);
}

static void errorAt(Token* token, const char* message)
{
    if(parser.panicMode == true) return;
    parser.panicMode = true;
    fprintf(stderr, "Error at line[%d]", token->line);
    if(token->type == TOKEN_EOF)
    {
        fprintf(stderr, "At the end of input.");
    }
    else if(token->type == TOKEN_ERROR)
    {
        fprintf(stderr, "Invalid Token");
    }
    else
    {
        fprintf(stderr, "Error at %d of length %d", token->start, token->length);
    }
    fprintf(stderr, "%s", message);
    parser.hadError = true;
}

static void emitByte(uint8_t byte)
{
    writetotheCHUNK(currentChunk(), byte, parser.previous.line);
}

static void emitBytes(uint8_t byte1, uint8_t byte2)
{
    emitByte(byte1);
    emitByte(byte2);
}

static void number()
{
    double value = strtod(parser.previous.start, NULL);
    emitBytes(OPCODE_CONSTANT, createConstant(value));
}

static uint8_t createConstant(double value)
{
    int constant = addConstant(currentChunk(), value);
    if(constant > UINT8_MAX)
    {
        error("Too many constants in one chunk.");
        return NULL;
    }
    return (uint8_t)constant;
}

static void expression()
{
    paresePrecedenceWise(PRECEDENCE_ASSIGNMENT);
}

static void grouping()
{
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expected ')' afterwards.");
}

static void unary()
{
    TokenType unaryOPERATOR = parser.previous.type;
    parsePrecedenceWise(PRECEDENCE_UNARY);
    switch(unaryOPERATOR)
    {
        case TOKEN_MINUS:
        {
            emitByte(OPCODE_NEGATE);
            break;
        }
        default:
        return;
    }
}

static void binary()
{
    TokenType binaryOPERATOR = parser.previous.type;
    PARSERULE* rule = getRule(binaryOPERATOR);
    parsePrecedenceWise((PRECEDENCE)rule->precedence + 1);
    switch(binaryOPERATOR)
    {
        case TOKEN_PLUS:
        emitByte(OPCODE_ADDITION);
        case TOKEN_MINUS:
        emitByte(OPCODE_SUBTRACT);
        case TOKEN_SLASH:
        emitByte(OPCODE_DIVISION);
        case TOKEN_STAR:
        emitByte(OPCODE_MULTIPLY);
        default:
        return;
    }
}

static void parsePrecedenceWise(PRECEDENCE precedence)
{
    advance();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if(prefixRule == NULL)
    {
        error("Expected an expression after the infix operator.");
        return;
    }
    prefixRule();
    while(precedence <= getRule(parser.current.type)->precedence)
    {
        advance();
        ParseFn infixRule = getRule(parser.current.type)->infix;
        infixRule();
    }
}
#endif
