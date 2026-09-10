#include "raudio_ds.h"
#include "rcore_ds.h"
#include "lodepng.h"
#define BLACK (Color){0,0,0}
#define TO5BITS >>3 //useless macro go brrr
    
int main()
{
    InitWindow(256,192,"w");




    /*Image i = LoadImage("ass.png");
    Texture2D t = LoadTextureFromImage(i);

    Image r = LoadImage("raylib.png");
    Texture2D rt = LoadTextureFromImage(r);*/

    Image p = LoadImage("1.ppm");
    Texture2D pe = LoadTextureFromImage(p);
    unsigned char* imgR;
    unsigned  width,height;
    //error = lodepng_decode24_file(&imgR, &width, &height, "raylib.png");
    //if (error) TRACELOG(LOG_ERROR, "error %u: %s\n", error, lodepng_error_text(error));
    //Image r = processPng(imgR,height,width);
    //Texture2D tR = LoadTextureFromImage(r);
    printf("texture loaded");
    printf("\n\n\n%s\n\n\n",GetFileExtension("raylib.png")); //INCLUDES THE DOT (".png")
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        //DrawTexture(t,0,0,BLACK);
        //DrawTexture(rt,100,0,BLACK);
        DrawTexture(pe,100,100,BLACK);
        printf("\n raylib.png exists: %d",FileExists("raylib.png"));
        static bool ran = false;


        EndDrawing();


    }

    
}
