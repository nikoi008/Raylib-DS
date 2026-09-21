#include "raudio_ds.h"
#include "rcore_ds.h"
#include "lodepng.h"
#include <string.h>
#include <stdio.h>
#define BLACK (Color){0,0,0}
#define TO5BITS >>3 //useless macro go brrr
#include "rtext_ds.h"

/*
void DrawTextEx(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint)
{
    drawString(&font,position,text,tint,(int)spacing,fontSize);


}
*/
int main()
{
    InitWindow(256,192,"w");
    Font f = LoadFont("nitro:/default.fnt");
    //Font m = LoadFont("nitro/default.fnt");
    //Font j = LoadFont("mono.fnt");

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextEx(f,"hello",(Vector2){10,10},3.2,1,(Color){255,0,0});
        DrawText("hello world",100,100,1,(Color){80,80,80});
        EndDrawing();
    }

}