#include <stdlib.h>
#include <stdio.h>
#define buffer_len 500
void RFF();
void WFF();
int main()
{
    WFF();
    RFF();
    return 0;
}
void WFF()
{
    FILE* file = fopen("scanner.txt", "w");
    if(file != NULL)
    {
        fprintf(file, "1st Line.\n");
        fprintf(file, "2nd Line.\n");
        fprintf(file, "3rd Line.\n");
        fprintf(file, "Ok we're done here.😭😭");
    }
    fclose(file);
    return;
}
void RFF()
{
    FILE* file = fopen("scanner.txt", "r");
    if(file == NULL)
    {
        printf("Unable to open file.");
        exit(EXIT_FAILURE);
    }
    char buffer[buffer_len];
    while(fgets(buffer, buffer_len, file) != NULL)
    {
        printf("%s", buffer);
    }
    fclose(file);
    return;
}