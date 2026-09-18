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
#include "fontDefault.h"
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
}// ie something="bananas" returns "bananas"

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
    buffer[index] = '\0';
    return atoi(buffer);
}// ie input "something=" returns 42
/*Font LoadFontDefault()
{
    Font f;
    int offset = 0;
    f.f.face = findStringRetString(defaultFontData, "face=\"",&offset);

    f.f.fileName = findStringRetString(defaultFontData,"file=\"",&offset);
    Image i = LoadImageFromMemory(".png",defaultFontPng,defaultFontpngSize);
    f.tex = LoadTextureFromImage(i);
    f.f.chars = findValInString(defaultFontData,"chars count=",&offset);

    f.glyph = malloc(sizeof(Glyph) * f.f.chars);

    for (int i = 0 ; i < f.f.chars; i++)
    {

        f.glyph[i].id = findValInString(defaultFontData,"char id=",&offset);
        f.glyph[i].x = findValInString(defaultFontData,"x=",&offset);
        f.glyph[i].y = findValInString(defaultFontData,"y=",&offset);
        f.glyph[i].width = findValInString(defaultFontData,"width=",&offset);
        f.glyph[i].height = findValInString(defaultFontData,"height=",&offset);
        f.glyph[i].xOffset = findValInString(defaultFontData,"xoffset=",&offset);
        f.glyph[i].yOffset = findValInString(defaultFontData,"yoffset=",&offset);
        f.glyph[i].xAdvance = findValInString(defaultFontData,"xadvance=",&offset);
    }

    return f;
}*/
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
}//pretty slow, best to load at start

int drawChar(Font* f,Vector2 pos,char c,Color col,float size)
{
    int i;
    for (i = 0; i < f->f.chars; i++)
    {
        if (f->glyph[i].id == c) break;
    }
    pos.x += f->glyph[i].xOffset;
    pos.y += f->glyph[i].yOffset;

    DrawTextureRecAndScale(f->tex,(Rectangle){f->glyph[i].x ,f->glyph[i].y, f->glyph[i].width, f->glyph[i].height},pos, col,floattof32(size),floattof32(size));
    return i;

}// returns position of character drawn
void drawString(Font* f, Vector2 pos, char* s,Color col, int spacing,float size)
{

    int cursorX = pos.x;
    int cursorY = pos.y;
    int len =  strlen(s);
    for (int i = 0; i < len ; i++)
    {
        int id = drawChar(f,(Vector2){cursorX,cursorY},s[i],col,size);
        cursorX += f->glyph[id].xAdvance;
        cursorX += spacing;
    }
}


void DrawTextEx(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint)
{
    drawString(&font,position,text,tint,(int)spacing,fontSize);


}

int main()
{
    InitWindow(256,192,"w");
    Font f = LoadFont("test.fnt");
    Font m = LoadFont("default.fnt");
    Font j = LoadFont("mono.fnt");

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextEx(f,"hello",(Vector2){10,10},3.2f,1,(Color){255,0,0});

        EndDrawing();

    }

}