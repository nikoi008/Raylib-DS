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
    int frames;
    int colors;
}Image;

typedef struct
{
    //Image i;
    glImage* image;
    int frames;
    int id;
    int rot;
    int scale;

}Texture2D;
void DrawTextureRecAndScale(Texture2D texture, Rectangle source, Vector2 position, Color tint,s32 sx, s32 sy);

void UnloadTextureAnim(Texture2D texture);
Texture2D LoadTextureAnimFromImage(Image im);
void DrawTextureAnim(Texture2D texture, int frame, int posX, int posY, Color tint);
Vector2 getImageSize(const char* buffer);
Image LoadImage(const unsigned char* loc);
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


void ImageClearBackground(Image *dst, Color color);                                                // Clear image background with given color
    void ImageDrawPixel(Image *dst, int posX, int posY, Color color);                                  // Draw pixel within an image
    void ImageDrawPixelV(Image *dst, Vector2 position, Color color);                                   // Draw pixel within an image (Vector version)
    void ImageDrawLine(Image *dst, int startPosX, int startPosY, int endPosX, int endPosY, Color color); // Draw line within an image
    void ImageDrawLineV(Image *dst, Vector2 start, Vector2 end, Color color);                          // Draw line within an image (Vector version)
    void ImageDrawLineEx(Image *dst, Vector2 start, Vector2 end, int thick, Color color);              // Draw a line defining thickness within an image
    void ImageDrawCircle(Image *dst, int centerX, int centerY, int radius, Color color);               // Draw a filled circle within an image
    void ImageDrawCircleV(Image *dst, Vector2 center, int radius, Color color);                        // Draw a filled circle within an image (Vector version)
    void ImageDrawCircleLines(Image *dst, int centerX, int centerY, int radius, Color color);          // Draw circle outline within an image
    void ImageDrawCircleLinesV(Image *dst, Vector2 center, int radius, Color color);                   // Draw circle outline within an image (Vector version)
    void ImageDrawRectangle(Image *dst, int posX, int posY, int width, int height, Color color);       // Draw rectangle within an image
    void ImageDrawRectangleV(Image *dst, Vector2 position, Vector2 size, Color color);                 // Draw rectangle within an image (Vector version)
    void ImageDrawRectangleRec(Image *dst, Rectangle rec, Color color);                                // Draw rectangle within an image
    void ImageDrawRectangleLines(Image *dst, Rectangle rec, int thick, Color color);                   // Draw rectangle lines within an image
    void ImageDrawTriangle(Image *dst, Vector2 v1, Vector2 v2, Vector2 v3, Color color);               // Draw triangle within an image
    void ImageDrawTriangleEx(Image *dst, Vector2 v1, Vector2 v2, Vector2 v3, Color c1, Color c2, Color c3); // Draw triangle with interpolated colors within an image
    void ImageDrawTriangleLines(Image *dst, Vector2 v1, Vector2 v2, Vector2 v3, Color color);          // Draw triangle outline within an image
    void ImageDrawTriangleFan(Image *dst, const Vector2 *points, int pointCount, Color color);         // Draw a triangle fan defined by points within an image (first vertex is the center)
    void ImageDrawTriangleStrip(Image *dst, const Vector2 *points, int pointCount, Color color);       // Draw a triangle strip defined by points within an image
    void ImageDraw(Image *dst, Image src, Rectangle srcRec, Rectangle dstRec, Color tint);             // Draw a source image within a destination image (tint applied to source)
    void ImageDrawText(Image *dst, const char *text, int posX, int posY, int fontSize, Color color);   // Draw text (using default font) within an image (destination)
   // void ImageDrawTextEx(Image *dst, Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint); // Draw text (custom sprite font) within an image (destination) todo there is some funkiness with includes


#endif //RAYLIB_DS_RTEXTURE_DS_H
