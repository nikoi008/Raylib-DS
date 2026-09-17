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
    int chars;
}FontInfo;

typedef struct
{
    Glyph *glyph;
    FontInfo f;
    Texture2D tex;
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
    while (stringLocation[index] != ' ' && stringLocation[index] != '\n' && stringLocation[index] != '\0')
    {
        buffer[index] = stringLocation[index];
        index++;
    }
    *offset = (stringLocation - data) + index;
    buffer[index] = '\n';
    return atoi(buffer);


}
//pretty slow, best to load at start
Font LoadFont(char* name)
{
    Font f;
    char* fileData = LoadFileText(name);
    int offset = 0;
    f.f.face = findStringRetString(fileData, "face=\"",&offset);
    
    f.f.fileName = findStringRetString(fileData,"file=\"",&offset);
    f.tex = LoadTexture(f.f.fileName);
    f.f.chars = findValInString(fileData,"chars count=",&offset);

    f.glyph = malloc(sizeof(Glyph) * f.f.chars);

    for (int i = 0 ; i < f.f.chars; i++)
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

    return f;
}
int drawChar(Font* f,Vector2 pos,char c,Color col) // returns position of character drawn
{
    int i;
    for (i = 0; i < f->f.chars; i++)
    {
        if (f->glyph[i].id == c) break;
    }
    pos.x += f->glyph[i].xOffset;
    pos.y += f->glyph[i].yOffset;
    DrawTextureRec(f->tex,(Rectangle){f->glyph[i].x ,f->glyph[i].y, f->glyph[i].width, f->glyph[i].height},pos, col);
    return i;

}
void drawString(Font* f, Vector2 pos, char* s,Color col)
{

    int cursorX = pos.x;
    int cursorY = pos.y;
    for (int i = 0; i < strlen(s); i++)
    {
        int id = drawChar(f,(Vector2){cursorX,cursorY},s[i],col);
        cursorX += f->glyph[id].xAdvance;
    }
}
int main()
{
    InitWindow(256,192,"w");

    Font f = LoadFont("test.fnt");
    Font m = LoadFont("mono.fnt");
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        //DrawTexture(f.tex,0,0,(Color){0,0,0});
        drawChar(&f,(Vector2){10,10},'c',(Color){0,0,0});
        drawString(&f,(Vector2){10,40},"tHe quick brown fox",(Color){255,0,0});
        drawString(&f,(Vector2){10,80},"jumps over tHe lazY",(Color){255,255,255});
        drawString(&f,(Vector2){10,120},"dog",(Color){0,255,120});
        drawString(&m,(Vector2){0,160},"jetbrains mono yummers",(Color){255,255,255});
        static long int i = 0;
        i++;
        EndDrawing();


    }


}