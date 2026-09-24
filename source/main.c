#include "raudio_ds.h"
#include "rcore_ds.h"
#include "lodepng.h"
#include <string.h>
#include <stdio.h>
#define BLACK (Color){0,0,0}
#define TO5BITS >>3 //useless macro go brrr
#include "rtext_ds.h"

void rshapes()
{
    for (int y = 10; y < 40; y += 2)
    {
        for (int x = 10; x < 40; x += 2)
        {
            Color c = {x * 5,y * 6,128};
            DrawPixel(x,y,c);
        }
    }
    int r = 10;
    for (int y = 10; y < 40; y += 3)
    {

        DrawLine(50,y,120,y,(Color){ r,y * 3, 64});
        r += 2;
    }

    DrawLineDashed((Vector2){130,10},(Vector2){180,10},4,2,RED);
    DrawLineEx((Vector2){190,10},(Vector2){240,15},5,YELLOW);

    Vector2 points[7] = {{130, 35},{145, 20},{160, 35},{175, 20},{190, 35},{205, 20},{220, 35}};
    DrawLineStrip(points,7,PURPLE);

    DrawCircle(10 + 16,45 + 16,16,GREEN);
    DrawCircleLines(50 + 16, 45 + 16,16, ORANGE);
    DrawCircleGradient((Vector2){90 + 16, 45 + 16},16,YELLOW,RED);

    DrawEllipse(130 + 24, 45 + 14, 24,14,PINK);
    DrawEllipseLines(190 + 24, 45 + 14, 24,14,BLUE);

    DrawRectangle(10,85,50,25,RED);
    DrawRectangleGradientH(65,85,50,25,WHITE,PURPLE);
    DrawRectangleGradientEx((Rectangle){120,85,50,25,},RED,WHITE,GREEN,BLUE);
    DrawRectangleLines(175,85,50,25,MAGENTA);
    
    DrawTriangle((Vector2){22,118},(Vector2){5,152},(Vector2){40,152},RED);

    DrawTriangleLines((Vector2){62,118},(Vector2){45,152}, (Vector2){80,152},YELLOW);

    Vector2 fan[5] = {{105,135},{85,152},{85,118},{125,118},{125,152}};
    DrawTriangleFan(fan,5,GREEN);

    Vector2 strip[7] = {{130,152},{137,118},{145,152},{152,118},{160,152},{167,118},{175,152}};
    DrawTriangleStrip(strip,7,BLUE);

    DrawPoly((Vector2){195,135},6,16,0,ORANGE);
    DrawPolyLines((Vector2){232,135},5,16,30,MAGENTA);

    DrawText("Shapes demo",10,160,1,WHITE);
}

int main()
{
    InitWindow(256,192,"w");
    InitAudioDevice();
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        rshapes();
        EndDrawing();
    }

}