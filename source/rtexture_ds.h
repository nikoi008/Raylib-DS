//
// Created by Nmsou on 22/08/2026.
//
#include <nds.h>
#include "rcore_ds.h"
#include <math.h>
#include <gl2d.h>
#include "rshapes_ds.h"
#ifndef RAYLIB_DS_RTEXTURE_DS_H
#define RAYLIB_DS_RTEXTURE_DS_H


typedef struct
{
    u16 *pal;
    u8* gfx;
    Vector2 size;
    glImage* image;
    int frames;
    int colors;
}Image;

typedef struct
{
    Image i;
    int id;
    int rot;
    int scale;
    int pos;

}Texture2D;
void UnloadTextureAnim(Texture2D texture);
Texture2D LoadTextureAnimFromImage(Image im);
void DrawTextureAnim(Texture2D texture, int frame, int posX, int posY, Color tint);
Vector2 getImageSize(const char* buffer);
Image LoadImage(const char* loc);
Image LoadImageAnim(const char* filename, int frames);
Image LoadImageFromMemory(const unsigned char *fileType, const unsigned char *fileData, int dataSize);

void UnloadImage(Image image);
bool ExportImage(Image image);
Texture2D LoadTexture(const char* filename);
Texture2D LoadTextureFromImage(Image i);
void DrawTexture(Texture2D texture, int posX, int posY, Color tint);
void UnloadTexture(Texture2D texture);
void DrawTextureRec(Texture2D texture, Rectangle source, Vector2 position, Color tint);

Image LoadImageRaw(const char *fileName, int width, int height, int format, int headerSize);

#endif //RAYLIB_DS_RTEXTURE_DS_H
