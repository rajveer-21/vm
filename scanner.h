#ifndef scanner_h
#define scanner_h
#include <string.h>
typedef enum 
{
    LEFT_PARENTHESIS, RIGHT_PARENTHESIS,
    LEFT_BRACE, RIGHT_BRACE,
    ADD, SUB, MUL, DIV,
    SEMICOLON, DOT, COMMA,
    IF, ELSE, FOR, WHILE, 
    FUN, RETURN,
    IDENTIFIER, NUMBER, STRING, VAR,
    GREATER, GREATER_EQUAL,
    LESSER, LESSER_EQUAL,
    EQUAL, EQUAL_EQUAL,
    NOT, NOT_EQUAL,
    AND, OR,
    NIL, THIS,
    TRUE, FALSE,
    ERROR, EOF_TOKEN,
    CLASS, SUPER, PRINT,

}TokenType;
typedef struct 
{
    const char* start;
    const char* current;
    int line;
}SCANNER;
typedef struct 
{
    TokenType type;
    const char* start;
    int length;
    int line;
}Token;
SCANNER scanner;
void initializeSCANNER(const char* source);
Token scanToken();
Token errorToken(const char* emessage);
bool isAtEnd();
Token makeToken(TokenType tokentype);
bool match(char req);
char advance();
void skipWHITESPACE();
char peek();
char peekFurther();
bool isDigit(char ch);
bool isAlpha(char ch);
Token string();
Token number();
Token identifier();
TokenType checkKey(int start, int length, const char* rest, TokenType type);
TokenType keywordsANDidentifiers();
void initializeSCANNER(const char* source)
{
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}
Token scanToken()
{
    skipWHITESPACE();
    scanner.start = scanner.current;
    if(isAtEnd() == true)
    return makeToken(EOF_TOKEN);
    char c = advance();
    if(isAlpha(c))
    return identifier();
    if(isDigit(c)) 
    return number();
    switch (c) 
    {
        case '(': return makeToken(LEFT_PARENTHESIS);
        case ')': return makeToken(RIGHT_PARENTHESIS);
        case '{': return makeToken(LEFT_BRACE);
        case '}': return makeToken(RIGHT_BRACE);
        case ';': return makeToken(SEMICOLON);
        case ',': return makeToken(COMMA);
        case '.': return makeToken(DOT);
        case '-': return makeToken(SUB);
        case '+': return makeToken(ADD);
        case '/': return makeToken(DIV);
        case '*': return makeToken(MUL);
        case '!':
        {
            if(match(EQUAL) == true)
            return makeToken(NOT_EQUAL);
            return makeToken(NOT);
        }
        case '=':
        {
            if(match(EQUAL) == true)
            return makeToken(EQUAL_EQUAL);
            return makeToken(EQUAL);
        }
        case '>':
        {
            if(match(EQUAL) == true)
            return makeToken(GREATER_EQUAL);
            return makeToken(GREATER);
        }
        case '<':
        {
            if(match(EQUAL) == true)
            return makeToken(LESSER_EQUAL);
            return makeToken(LESSER);
        }
        case '\n':
        {
            advance();
            scanner.line = scanner.line + 1;
            break;
        }
        case '"':
        {
            return string();
            break;
        }
    }
    return errorToken("Unexpected Character at position.");
}
bool isAtEnd()
{
    if(*scanner.current == '\0')
    return true;
    return false;
}
Token makeToken(TokenType tokentype)
{
    Token token;
    token.type = tokentype;
    token.length = scanner.current - scanner.start;
    token.start = scanner.start;
    token.line = scanner.line;
    return token;
}
Token errorToken(const char* emessage)
{
    Token token;
    token.type = ERROR;
    token.start = emessage;
    token.length = (int)strlen(emessage);
    token.line = scanner.line;
    return token;
}
char advance()
{
    scanner.current++;
    return *(scanner.current - 1);
}
bool match(char req)
{
    if(isAtEnd() == true) return false;
    if(*scanner.current != req) return false;
    scanner.current = scanner.current + 1;
    return true;
}
void skipWHITESPACE()
{
    while(true)
    {
        char ch = peek();
        if(ch == ' ' || ch == '\t' || ch == '\v' || ch == '\r' || ch == '\n')
        advance();
        else 
        break;
    }
}
char peek()
{
    return *(scanner.current);
}
char peekFurther()
{
    if(isAtEnd() == false)
    return *(scanner.current + 1);
}
Token string()
{
    while(peek() != '"' && isAtEnd() == false)
    {
        if(*scanner.current == '\n')
        scanner.line = scanner.line + 1;
        advance();
    }
    if(isAtEnd() == true)
    return errorToken("Unterminated String.");
    advance();
    return makeToken(STRING);
}
bool isDigit(char ch)
{
    if(ch >= '0' && ch <='9')
    return true;
    return false;
}
bool isAlpha(char ch)
{
    if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')
    return true;
    return false;
}
Token number()
{
    while (isDigit(peek()))
    advance();
    if (peek() == '.') 
    {
        advance();
        if (isDigit(peek()))
        {
            while (isDigit(peek()))
            advance();
        }
    }
    return makeToken(NUMBER);
}
Token identifier()
{
    while(isAlpha(peek()) || isDigit(peek()))
    advance();
    return makeToken(keywordsANDidentifiers());
} 
TokenType keywordsANDidentifiers()
{
    switch(scanner.start[0])
    {
        case 'a': return checkKey(1, 2, "nd", AND);
        case 'c': return checkKey(1, 4, "lass", CLASS);
        case 'e': return checkKey(1, 3, "lse", ELSE);
        case 'i': return checkKey(1, 1, "f", IF);
        case 'n': return checkKey(1, 2, "il", NIL);
        case 'o': return checkKey(1, 1, "r", OR);
        case 'p': return checkKey(1, 4, "rint", PRINT);
        case 'r': return checkKey(1, 5, "eturn", RETURN);
        case 's': return checkKey(1, 4, "uper", SUPER);
        case 'v': return checkKey(1, 2, "ar", VAR);
        case 'w': return checkKey(1, 4, "hile", WHILE);
        case 'f':
        switch (scanner.start[1])
        {
          case 'a': return checkKey(2, 3, "lse", FALSE);
          case 'o': return checkKey(2, 1, "r", FOR);
          case 'u': return checkKey(2, 1, "n", FUN);
        }
        case 't':
        if (scanner.current - scanner.start > 1) 
        {
            switch (scanner.start[1]) 
            {
                case 'h': return checkKey(2, 2, "is", THIS);
                case 'r': return checkKey(2, 2, "ue", TRUE);
            }
        }
        break;
    }
    return IDENTIFIER;
}
TokenType checkKey(int start, int length, const char* rest, TokenType type) 
{
  if (scanner.current - scanner.start == start + length && memcmp(scanner.start + start, rest, length) == 0) 
  {
    return type;
  }
  return IDENTIFIER;
}
#endif