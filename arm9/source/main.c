
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



Texture2D person;
Texture2D tile;
Texture2D bug;
void loadTextures()
{

    Image i = LoadImageAnim("nitro:/player.png",10);
    person = LoadTextureAnimFromImage(i);
    //tile = LoadTexture("nitro:/tileSprites.png");
    bug = LoadTexture("nitro:/aphid.png");

}


void rtexture()
{

    static int i,j;
    j++;

   // DrawTexture(bug, 100,100, RED);

    DrawTextureAnim(person,i % 10,10,10,WHITE);
    DrawTextureAnim(person,5,100,50,WHITE);
    DrawTextureEx(bug,(Vector2){50,26},0,2,WHITE);
    //DrawTextureAnim(player,8,50,50,WHITE);
}

Camera2D camera;
Rectangle player = {0};
void cameraInput()
{

    if (IsKeyDown(KEY_RIGHT)) player.x += 2;
    else if (IsKeyDown(KEY_LEFT)) player.x -= 2;
    if (IsKeyDown(KEY_DOWN)) player.y += 2;
    else if (IsKeyDown(KEY_UP)) player.y  -= 2;

    // Camera target follows player
    camera.target = (Vector2){ player.x + 20, player.y + 20 };

    // Camera rotation controls
    if (IsKeyDown(KEY_A)) camera.rotation--;
    else if (IsKeyDown(KEY_B)) camera.rotation++;

    // Limit camera rotation to 80 degrees (-40 to 40)
    if (camera.rotation > 40) camera.rotation = 40;
    else if (camera.rotation < -40) camera.rotation = -40;

    // Camera zoom controls
    // Uses log scaling to provide consistent zoom speed
    //camera.zoom = expf(logf(camera.zoom) + ((float)GetMouseWheelMove()*0.1f));

    if (camera.zoom > 3.0f) camera.zoom = 3.0f;
    else if (camera.zoom < 0.1f) camera.zoom = 0.1f;
    // Camera reset (zoom and rotation)

    if (IsKeyPressed(KEY_Y))
    {
        camera.zoom = 1.0f;
        camera.rotation = 0.0f;
    }
    
    if (IsKeyPressed(KEY_R))
    {
        camera.zoom += 0.1f;
    }

    if (IsKeyPressed(KEY_L))
    {
        camera.zoom -= 0.1f;
    }
}
const int screenWidth = 256;
const int screenHeight = 192;
#define MAX_BUILDINGS   100
Rectangle buildings[MAX_BUILDINGS] = { 0 };
Color buildColors[MAX_BUILDINGS] = { 0 };
void initCamera2Dexample(){
    int spacing = 0;

    for (int i = 0; i < MAX_BUILDINGS; i++)
    {
        buildings[i].width = GetRandomValue(50, 200);
        buildings[i].height = GetRandomValue(100, 800);
        buildings[i].y = screenHeight - 130 - buildings[i].height;
        buildings[i].x = -6000 + spacing;

        spacing += (int)buildings[i].width;

        buildColors[i] = (Color){
            (unsigned char)GetRandomValue(200, 240),
            (unsigned char)GetRandomValue(200, 240),
            (unsigned char)GetRandomValue(200, 250),
            255};
    }

    //Camera2D camera = { 0 };
    player.width = 8;
    player.height = 8;
    camera.target = (Vector2){ player.x + 20.0f, player.y + 20.0f };
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;}
void camera2D()
{
    BeginMode2D(camera);
    cameraInput();


    DrawRectangle(-6000, 320, 13000, 8000, DARKGRAY);

    for (int i = 0; i < MAX_BUILDINGS; i++) DrawRectangleRec(buildings[i], buildColors[i]);

    DrawRectangleRec(player, RED);

    DrawLine((int)camera.target.x, -screenHeight*10, (int)camera.target.x, screenHeight*10, GREEN);
    DrawLine(-screenWidth*10, (int)camera.target.y, screenWidth*10, (int)camera.target.y, GREEN);

    EndMode2D();

    DrawText("SCREEN AREA", 640, 10, 1, RED);

    DrawRectangle(0, 0, screenWidth, 5, RED);
    DrawRectangle(0, 5, 5, screenHeight - 10, RED);
    DrawRectangle(screenWidth - 5, 5, 5, screenHeight - 10, RED);
    DrawRectangle(0, screenHeight - 5, screenWidth, 5, RED);

   // DrawRectangle( 10, 10, 250, 113, SKYBLUE);
   // DrawRectangleLines( 10, 10, 250, 113, BLUE);

    DrawTextEx(DS.fontDefault,"Free 2D camera controls:",(Vector2){10,20}, 0.5, 0.5, GRAY);
    DrawTextEx(DS.fontDefault,"- Right/Left to move player",(Vector2){10,30}, 0.5, 0.5, GRAY);
    DrawTextEx(DS.fontDefault,"- Triggers for zoom in and out",(Vector2){10,40}, 0.5, 0.5, GRAY);
    DrawTextEx(DS.fontDefault,"A/B to Rotate",(Vector2){10,50}, 0.5, 0.5, GRAY);
    DrawTextEx(DS.fontDefault,"X to reset zoom and rotation",(Vector2){10,60}, 0.5, 0.5, GRAY);

  //  DrawText("- Right/Left to move player", 10, 40, 0.5, DARKGRAY);
   // DrawText("- Mouse Wheel to Zoom in-out", 10, 60, 0.5, DARKGRAY);
   // DrawText("- A / S to Rotate", 40, 80, 10, DARKGRAY);
   // DrawText("- R to reset Zoom and Rotation", 40, 100, 0.5, DARKGRAY);


}
#include "raudio_ds.h"
int main()
{

    InitWindow(256,192,"w");
    InitAudioDevice();
    char* dat;
    int size;
    dat = LoadFileData("fat:/test.wv",&size);
    AS_MP3DirectPlay(dat,size);
    //PlayMusicStream(&m);
    //PlaySound(s);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        //rshapes();
        //rtexture();
        //camera2D();
        EndDrawing();
    }

} 