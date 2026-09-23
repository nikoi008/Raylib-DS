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

int main()
{
    InitWindow(256,192,"w");
   /// Font f = LoadFont("nitro:/default.fnt");
    //Font m = LoadFont("nitro/default.fnt");
    //Font j = LoadFont("mono.fnt");
    Image i = GenImageColor(12,12,(Color){255,255,20});
    Texture2D t = LoadTextureFromImage(i);
    Sound s = LoadSound("fat:/test.wav");
    //Music m = LoadMusicStream("test.mp3");
    int ie = 0;
    PlaySound(&s);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        ie++;
        DrawText(TextFormat("running %d",ie),10,10,1,(Color){100,20,255});

        DrawTexture(t,100,100,(Color){255,255,255});
        EndDrawing();
    }

}