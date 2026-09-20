#ifndef RTEXTURES_DS_H
#define RTEXTURES_DS_H

#include "rcore_ds.h"
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
} Glyph;

typedef struct
{
    char* face;
    int size;
    bool bold;
    bool italic;
    char* fileName;
    int chars;
} FontInfo;

typedef struct
{
    Glyph *glyph;
    FontInfo f;
    Texture2D tex;
} Font;

Font LoadFont(char* name);
void UnloadFont(Font font);
int drawChar(Font* f, Vector2 pos, char c, Color col, float size);
void drawString(Font* f, Vector2 pos, char* s, Color col, int spacing, float size);
void DrawTextEx(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint);
void DrawText(const char *text, int posX, int posY, int fontSize, Color color);
void SetTextLineSpacing(int spacing);
int MeasureText(const char *text, int fontSize);
Vector2 MeasureTextEx(Font font, const char *text, float fontSize, float spacing);
int TextFindIndex(const char *text, const char *search);
unsigned int TextLength(const char *text);
char **LoadTextLines(const char *text, int *count);
void UnloadTextLines(char **lines, int lineCount);
const char *TextFormat(const char *text, ...);
int TextToInteger(const char *text);
float TextToFloat(const char *text);
int TextCopy(char *dst, const char *src);
bool TextIsEqual(const char *text1, const char *text2);
const char *TextSubtext(const char *text, int position, int length);
const char *TextRemoveSpaces(const char *text);
char *GetTextBetween(const char *text, const char *begin, const char *end);
char *TextReplace(const char *text, const char *search, const char *replacement);
char *TextReplaceAlloc(const char *text, const char *search, const char *replacement);
char *TextReplaceBetween(const char *text, const char *begin, const char *end, const char *replacement);
char *TextReplaceBetweenAlloc(const char *text, const char *begin,const char *end, const char *replacement);
char *TextInsert(const char *text, const char *insert, int position);
char *TextInsertAlloc(const char *text, const char *insert, int position);
char *TextJoin(char **textList, int count, const char *delimiter);
char **TextSplit(const char *text, char delimiter, int *count);
void TextAppend(char *text, const char *append, int *position);
char *TextToUpper(const char *text);
char *TextToLower(const char *text);
char *TextToPascal(const char *text);
char *TextToSnake(const char *text);
char *TextToCamel(const char *text);

#endif