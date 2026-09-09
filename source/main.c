#include "raudio_ds.h"
#include "rcore_ds.h"
#include "lodepng.h"
#define BLACK (Color){0,0,0}
#define TO5BITS >>3 //useless macro go brrr
Image processPng(char* image, int height, int width)
{
    if ((height > 0 && (height & (height - 1)) != 0)){TRACELOG(LOG_INFO,"IMAGE HEIGHT MUST BE A POWER OF 2"); return (Image){0};}
    if ((width > 0 && (width & (width - 1)) != 0)){TRACELOG(LOG_INFO,"IMAGE width MUST BE A POWER OF 2"); return (Image){0} ;}

    glImage *immage = malloc(sizeof(glImage));
    u16* pal = malloc(sizeof(u16) * 256);
    //pal[0] = ARGB16(1,image[0],image[1],image[2]);
    pal[0] = ARGB16(1, image[0]TO5BITS, image[1]TO5BITS, image[2]TO5BITS);
    int palTotal = 1;

    u8* gfx = malloc(sizeof(u8) * width * height);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = (y * width + x) * 3;
            int R = image[index]TO5BITS;
            int G = image[index + 1]TO5BITS;
            int B = image[index + 2] TO5BITS;
            u16 col15 = ARGB16(1, R, G, B);

            bool uniqueCol = true;
            for (int p = 0; p < palTotal; p++)
            {
                if (pal[p] == col15) { uniqueCol = false; break; }
            }
            if (uniqueCol)
            {
                if (palTotal >= 255) { TRACELOG(LOG_WARNING, "IMAGE: TOO MANY COLOURS -- MAX 255"); continue; }
                pal[palTotal++] = col15;
            }
        }
    }
    printf("palette");
    FILE* fa;
    fa = fopen("palette log.txt", "w");
    if (fa == NULL)
    {
        printf("fat erra");
    }
    for (int i = 0; i < palTotal; i++)
    {
        char log[64];
        snprintf(log,sizeof(log),"pal %d, %u\n",i,pal[i]);
        fputs(log,fa);
    }
    fclose(fa);


    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = (y * width + x) * 3;
            int R = image[index] TO5BITS;
            int G = image[index + 1] TO5BITS;
            int B = image[index + 2] TO5BITS;
            u16 col15 = ARGB16(1, R, G, B);
            for (int i = 0; i < palTotal; i++)
            {
                if (col15 == pal[i]) gfx[y * width + x] = i;
            }

        }
    }

   FILE* f;
    f = fopen("gfx log.txt", "w");
    if (f == NULL)
    {
        printf("fat erra");
    }


    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = (y * width + x) * 3;
            char log[64];
            snprintf(log,sizeof(log),"%u",gfx[y * width + x]);
            fputs(log,f);
            if (x == width - 1)
            {
                fputs("\n",f);
            }
        }
    }
    fclose(f);



    return (Image){1,pal,gfx,(Vector2){width,height},immage,1};

}
int main()
{
    InitWindow(256,192,"w");

    unsigned error;
  unsigned char* image = 0;
  unsigned width, height;

  error = lodepng_decode24_file(&image, &width, &height, "ass.png");
  if(error) TRACELOG(LOG_ERROR,"error %u: %s\n", error, lodepng_error_text(error));



    Image i = processPng(image, height, width);
    Texture2D t = LoadTextureFromImage(i);
    printf("texture loaded");
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        TRACELOG(LOG_ALL,"smth");
        DrawTexture(t,100,100,BLACK);
        static bool ran = false;

    //TRACELOG(LOG_ALL, " %d: %d", i, image[i]);

        
        //TRACELOG(LOG_ALL,"channel %d",S.id);
        EndDrawing();


    }

    
}
