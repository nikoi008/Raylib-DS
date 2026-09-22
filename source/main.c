#include "raudio_ds.h"
#include "rcore_ds.h"
#include "lodepng.h"
#include <string.h>
#include <stdio.h>
#define BLACK (Color){0,0,0}
#define TO5BITS >>3 //useless macro go brrr
#include "rtext_ds.h"


bool isPowerOfTwo(unsigned int n)
{
    return n != 0 && (n & (n - 1)) == 0;
}

unsigned int nextPO2(unsigned int n)
{
    n--;
    n |= n  >> 1;
    n  |= n  >> 2;
    n  |= n  >> 4;
    n  |= n  >> 8;
    n  |= n  >> 16;
    n ++;
    return n;
}


Image GenImageColor(int width, int height, Color color)
{
    Image i;

    if (!isPowerOfTwo(width) || !isPowerOfTwo(height))
    {
        i.size.x = (float)nextPO2(width);
        i.size.y = (float)nextPO2(height);
        i.frames = 1;
        i.colors = 1;
        i.pal = malloc(sizeof(u16));
        i.pal[0] = ARGB16(1,color.r TO5BITS,color.g TO5BITS,color.b TO5BITS);
        i.gfx = malloc(sizeof(u8)* width * height);
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                i.gfx[y * width + x] = 0;
            }
        }

    }
    return i;
};

int isColInPal(Image *img, Color color) // returns index of color
{
    int convColor = ARGB16(1,color.r TO5BITS, color.g TO5BITS, color.b TO5BITS);
    bool hasCol = false;
    int colIndex = 0;
    for (int i = 0; i < img->colors || hasCol == true; i++)
    {
        if (convColor == img->pal[i])
        {
            hasCol = true;
            colIndex = i;
        }
    }
    if (!hasCol)
    {
        img->pal[img->colors] = convColor;
        img->colors++;
        colIndex = img->colors;
    }
    return colIndex;
}
void ImageClearBackground(Image *dst, Color color)
{

    int colIndex = isColInPal(dst,color);
    for (int y = 0; y < dst->size.y; y++)
    {
        for (int x = 0; x < dst->size.x; x++)
        {
            dst->gfx[y * (int)dst->size.x + x] = colIndex;
        }
    }
};                                                // Clear image background with given color
void ImageDrawPixel(Image *dst, int posX, int posY, Color color)
{
    int colIndex = isColInPal(dst,color);
    if (posX > 0 || posX > (int)dst->size.x || posY > 0 || posY > (int)dst->size.y )return;
    dst->gfx[posY * (int)dst->size.x + posX] = colIndex;
};
// Draw pixel within an image

void ImageDrawPixelV(Image *dst, Vector2 position, Color color)
{
    if (position.x > 0 || position.x > (int)dst->size.x || position.y > 0 || position.y > (int)dst->size.y )return;
    int colIndex = isColInPal(dst,color);
    dst->gfx[(int)position.y * (int)dst->size.x + (int)position.x] = colIndex;
};                                   // Draw pixel within an image (Vector version)
void ImageDrawLine(Image *dst, int x0, int x1, int y0, int y1, Color color)
{
    //https://gist.github.com/bert/1085538
    int colIndex = isColInPal(dst,color);
    int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */

    for (;;){  /* loop */
        //setPixel (x0,y0);
        if (y0 < 0 || y0 > (int)dst->size.y || x0 < 0 || x0 > (int)dst->size.x) continue;
        dst->gfx[y0 * (int)dst->size.x + (int)x0] = colIndex;
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
        if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
    }
} // Draw line within an image
    void ImageDrawLineV(Image *dst, Vector2 start, Vector2 end, Color color)
{
    ImageDrawLine(dst,start.x,start.y,end.x,end.y,color);
};                          // Draw line within an image (Vector version)
    void ImageDrawLineEx(Image *dst, Vector2 start, Vector2 end, int thick, Color color);              // Draw a line defining thickness within an image
    void ImageDrawCircle(Image *dst, int centerX, int centerY, int radius, Color color)
    {
            //DrawCircleSector((Vector2){ (float)centerX, (float)centerY }, radius, 0, 360, 36, color);
            int x = 0;
            int y = (int)(radius);
            int d = 3 - 2 * (int)(radius);

            while (x <= y)
            {

                ImageDrawLine(dst,centerX - x, centerY + y,centerX + x, centerY + y,color);
                ImageDrawLine(dst,centerX - x, centerY - y, centerX + x, centerY - y,color);
                ImageDrawLine(dst,centerX - y, centerY + x, centerX + y, centerY + x,color);
                ImageDrawLine(dst,centerX -y , centerY - x, centerX + y,centerY - x,color);

                x++;
                if(d< 0)
                {
                    d = d + 4 * x + 6;
                }
                else
                {
                    y = y - 1;
                    d = d + 4 * (x - y) + 10;
                }
            }
    };              // Draw a filled circle within an image
    void ImageDrawCircleV(Image *dst, Vector2 center, int radius, Color color)
    {
        ImageDrawCircle(dst,(int)center.x,(int)center.y,radius,color);
    };                        // Draw a filled circle within an image (Vector version)
    void ImageDrawCircleLines(Image *dst, int centerX, int centerY, int radius, Color color)
    {
            int x = 0;
            int y = (int)(radius);
            int d = 3 - 2 * (int)(radius);

            while (x <= y)
            {
                ImageDrawPixel(dst,centerX + x, centerY + y,color);
                ImageDrawPixel(dst,centerX - x, centerY + y,color);
                ImageDrawPixel(dst,centerX + x, centerY - y,color);
                ImageDrawPixel(dst,centerX - x, centerY - y,color);
                ImageDrawPixel(dst,centerX + y, centerY + x,color);
                ImageDrawPixel(dst,centerX - y, centerY + x,color);
                ImageDrawPixel(dst,centerX + y, centerY - x,color);
                ImageDrawPixel(dst,centerX - y, centerY - x,color);

                x++;
                if(d< 0)
                {
                    d = d + 4 * x + 6;
                }
                else
                {
                    y = y - 1;
                    d = d + 4 * (x - y) + 10;
                }
            }
    };          // Draw circle outline within an image
    void ImageDrawCircleLinesV(Image *dst, Vector2 center, int radius, Color color)
    {
        ImageDrawCircleLines(dst,(int)center.x,(int)center.y,radius,color);
    };                   // Draw circle outline within an image (Vector version)
    void ImageDrawRectangle(Image *dst, int posX, int posY, int width, int height, Color color)
    {
        for (int i = 0; i < height; i++)
        {
            ImageDrawLine(dst,posX,posY,posX + width,posY,color);
        }
    };       // Draw rectangle within an image
    void ImageDrawRectangleV(Image *dst, Vector2 position, Vector2 size, Color color)
    {
        ImageDrawRectangle(dst,(int)position.x,(int)position.y,(int)size.x,(int)size.y,color);
    };                 // Draw rectangle within an image (Vector version)
    void ImageDrawRectangleRec(Image *dst, Rectangle rec, Color color)
    {
        ImageDrawRectangle(dst,rec.x,rec.y,rec.x + rec.width,rec.y + rec.height,color);
    };                                // Draw rectangle within an image
    void ImageDrawRectangleLines(Image *dst, Rectangle rec, int thick, Color color)
    {
        ImageDrawLine(dst,rec.x,rec.y,rec.x + rec.width,rec.y,color);
        ImageDrawLine(dst,rec.x,rec.y,rec.x,rec.y + rec.height,color);
        ImageDrawLine(dst,rec.x + rec.width,rec.y,rec.x + rec.width,rec.y + rec.height,color);
        ImageDrawLine(dst,rec.x,rec.y + rec.height,rec.x + rec.width,rec.y + rec.height,color);
    }
    void ImageDrawTriangle(Image *dst, Vector2 v1, Vector2 v2, Vector2 v3, Color color);               // Draw triangle within an image
    void ImageDrawTriangleEx(Image *dst, Vector2 v1, Vector2 v2, Vector2 v3, Color c1, Color c2, Color c3); // Draw triangle with interpolated colors within an image
    void ImageDrawTriangleLines(Image *dst, Vector2 v1, Vector2 v2, Vector2 v3, Color color);          // Draw triangle outline within an image
    void ImageDrawTriangleFan(Image *dst, const Vector2 *points, int pointCount, Color color);         // Draw a triangle fan defined by points within an image (first vertex is the center)
    void ImageDrawTriangleStrip(Image *dst, const Vector2 *points, int pointCount, Color color);       // Draw a triangle strip defined by points within an image
    void ImageDraw(Image *dst, Image src, Rectangle srcRec, Rectangle dstRec, Color tint);             // Draw a source image within a destination image (tint applied to source)
    void ImageDrawText(Image *dst, const char *text, int posX, int posY, int fontSize, Color color);   // Draw text (using default font) within an image (destination)
    void ImageDrawTextEx(Image *dst, Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint);
int main()
{
    InitWindow(256,192,"w");
    Font f = LoadFont("nitro:/default.fnt");
    //Font m = LoadFont("nitro/default.fnt");
    //Font j = LoadFont("mono.fnt");
    Image i = GenImageColor(16,16,(Color){255,255,20});
    Texture2D t = LoadTextureFromImage(i);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextEx(f,"hello",(Vector2){10,10},3.2,1,(Color){255,0,0});
        DrawText("hello world",100,100,1,(Color){80,80,80});
        DrawTexture(t,100,100,(Color){255,255,255});
        EndDrawing();
    }

}