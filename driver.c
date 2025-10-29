#include "common.h"
#include "compiler.h"
#include "memory.h"
#include "chunk.h"
#include "debugger.h"
#include "virtual_machine.h"
void runREPL();
void runFILE(const char* fileSource);
char* readFILE(const char* fileSource);
int main(int argc, const char* argv[])
{
    initializeVM();
    if(argc == 1)
    {
        runREPL();
    }
    else if(argc == 2)
    {
        runFILE(argv[1]);
    }
    else 
    {
        fprintf(stderr, "Invalid file path.");
        exit(64);
    }
    freeUpVM();
    return 0;
}
void runREPL()
{
    char line[2048];
    while(true)
    {
        printf("> ");
        if(!fgets(line, sizeof(line), stdin))
        {
            printf("\nEOF");
            break;
        }
        interpret(line);
    }
}
void runFILE(const char* fileSource)
{
    char* source = readFILE(fileSource);
    IResult result = interpret(source);
    free(source);
    if(result == INTERPRET_COMPILE_ERROR) exit(65);
    if(result == INTERPRET_RUNTIME_ERROR) exit(70);
}

char* readFILE(const char* fileSource)
{
    FILE* file = fopen(fileSource, "rb");
    if(file == NULL)
    {
        fprintf(stderr, "Could not open file.");
        exit(74);
    }
    fseek(file , 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);
    char* buffer = (char*)malloc(fileSize + 1);
    if(buffer == NULL)
    {
        fprintf(stderr, "Not enough memory to read.");
        exit(74);
    }
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    buffer[bytesRead] = '\0';
    fclose(file);
    return buffer;
}