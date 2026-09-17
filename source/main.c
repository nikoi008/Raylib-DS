#include "raudio_ds.h"
#include "rcore_ds.h"
#include "lodepng.h"
#include <string.h>
#include <stdio.h>
#define BLACK (Color){0,0,0}
#define TO5BITS >>3 //useless macro go brrr
typedef struct
{
    int id;
    int x;
    int y;
    int width;
    int height;
    int xOffset;
    int yOffset;
    int xAdvance;
}Glyph;

typedef struct
{
    char* face;
    int size;
    bool bold,italic;
    char* fileName;

}FontInfo;

typedef struct
{
    Glyph *glyph;
    FontInfo f;
}Font;
// ie something="bananas" returns "bananas"
char* findStringRetString(char *data,char *find,int *offset)
{
    char* stringLocation = strstr(&data[*offset],find);
    stringLocation += strlen(find);

    char buffer[64];
    int index = 0;

    while (stringLocation[index] != '"')
    {
        buffer[index] = stringLocation[index];
        index++;
    }
    *offset = *offset + index + strlen(find);
    buffer[index] = '\0';
    char* result = malloc(strlen(buffer)+1);
    strcpy(result,buffer);
    //printf("%s",result);
    return result;
}
// ie input "something=" returns 42
int findValInString(char *data, char *find, int *offset)
{
    char* stringLocation = strstr(data + *offset,find);
    stringLocation += strlen(find);
    char buffer[64];
    int index = 0;
    while (stringLocation[index] != ' ' && stringLocation[index] != '\n')
    {
        buffer[index] = stringLocation[index];
        index++;
    }
    *offset = (stringLocation - data) + index;
    return atoi(buffer);


}
//pretty slow, best to load at start
void parse()
{
    Font f;
    int dataSize;
    char* fileData = LoadFileText("test.fnt");
    int offset = 0;
    f.f.face = findStringRetString(fileData, "face=\"",&offset);
    
    f.f.fileName = findStringRetString(fileData,"file=\"",&offset);
    int chars = findValInString(fileData,"chars count=",&offset);

    f.glyph = malloc(sizeof(Glyph) * chars);

    for (int i = 0 ; i < chars; i++)
    {

        f.glyph[i].id = findValInString(fileData,"char id=",&offset);
        f.glyph[i].x = findValInString(fileData,"x=",&offset);
        f.glyph[i].y = findValInString(fileData,"y=",&offset);
        f.glyph[i].width = findValInString(fileData,"width=",&offset);
        f.glyph[i].height = findValInString(fileData,"height=",&offset);
        f.glyph[i].xOffset = findValInString(fileData,"xoffset=",&offset);
        f.glyph[i].yOffset = findValInString(fileData,"yoffset=",&offset);
        f.glyph[i].xAdvance = findValInString(fileData,"xadvance=",&offset);
    }
}

int main()
{
    InitWindow(256,192,"w");
    printf("parsing \n");
    parse();
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);



        EndDrawing();


    }


}