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
    int gfxWidth = isPowerOfTwo(width) ? width : nextPO2(width);
    int gfxHeight = isPowerOfTwo(height) ? height : nextPO2(height);
    i.size.x = (float)gfxWidth;
    i.size.y = (float)gfxHeight;
    i.frames = 1;
    i.colors = 1;
    i.gfx = malloc(sizeof(u8) * gfxHeight * gfxWidth);
    i.pal = malloc(sizeof(u16));
    i.pal[0] = ARGB16(1,color.r TO5BITS, color.g TO5BITS, color.b TO5BITS);
    for (int y = 0; y < gfxHeight; y++)
    {
        for (int x = 0; x < gfxWidth; x++)
        {
            i.gfx[y * gfxWidth + x] = 0;
        }
    }

    return i;
};
Image GenImageGradientLinear(int width, int height, int direction, Color start, Color end)
{
    Color *pixels = (Color *)RL_MALLOC(width*height*sizeof(Color));

    float radianDirection = (float)(90 - direction)/180.f*3.14159f;
    float cosDir = cosf(radianDirection);
    float sinDir = sinf(radianDirection);

    // Calculate how far the top-left pixel is along the gradient direction from the center of said gradient
    float startingPos = 0.5f - (cosDir*width/2) - (sinDir*height/2);

    // With directions that lie in the first or third quadrant (i.e. from top-left to
    // bottom-right or vice-versa), pixel (0, 0) is the farthest point on the gradient
    // (i.e. the pixel which should become one of the gradient's ends color); while for
    // directions that lie in the second or fourth quadrant, that point is pixel (width, 0)
    float maxPosValue = ((signbit(sinDir) != 0) == (signbit(cosDir) != 0))? fabsf(startingPos) : fabsf(startingPos + width*cosDir);
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            // Calculate the relative position of the pixel along the gradient direction
            float pos = (startingPos + (i*cosDir + j*sinDir))/maxPosValue;

            float factor = pos;
            factor = (factor > 1.0f)? 1.0f : factor;  // Clamp to [-1,1]
            factor = (factor < -1.0f)? -1.0f : factor;  // Clamp to [-1,1]
            factor = factor/2.0f + 0.5f;

            // Generate the color for this pixel
            pixels[j*width + i].r = (int)((float)end.r*factor + (float)start.r*(1.0f - factor));
            pixels[j*width + i].g = (int)((float)end.g*factor + (float)start.g*(1.0f - factor));
            pixels[j*width + i].b = (int)((float)end.b*factor + (float)start.b*(1.0f - factor));
            pixels[j*width + i].a = (int)((float)end.a*factor + (float)start.a*(1.0f - factor));
        }
    }

    Image image = {
        .data = pixels,
        .width = width,
        .height = height,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

    return image;
}
int main()
{
    InitWindow(256,192,"w");
    Font f = LoadFont("nitro:/default.fnt");
    //Font m = LoadFont("nitro/default.fnt");
    //Font j = LoadFont("mono.fnt");
    Image i = GenImageColor(12,12,(Color){255,255,20});
    Texture2D t = LoadTextureFromImage(i);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextEx(f,"hello",(Vector2){10,10},3.2,1,(Color){255,0,0});
        //DrawText("hello world",100,100,1,(Color){80,80,80});
        DrawTexture(t,100,100,(Color){255,255,255});
        EndDrawing();
    }

}