#include "raudio_ds.h"
#include "rcore_ds.h"
#include "lodepng.h"
#define BLACK (Color){0,0,0}
void processPng(char* image, int height, int width)
{
    if ((height > 0 && (height & (height - 1)) != 0)){TRACELOG(LOG_INFO,"IMAGE HEIGHT MUST BE A POWER OF 2"); return;}
    if ((width > 0 && (width & (width - 1)) != 0)){TRACELOG(LOG_INFO,"IMAGE width MUST BE A POWER OF 2"); return;}


    u16* pal = malloc(sizeof(u16) * 255);
    pal[0] = ARGB16(1,image[0],image[1],image[2]);
    int palTotal = 1;

    u8* gfx = malloc(sizeof(u8) * width * height);

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            int index = (y * width + x) * 3;
            int R = image[index];
            int G = image[index + 1];
            int B = image[index + 2];
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
    /*printf("palette");
    FILE* f;
    f = fopen("palette log.txt", "w");
    if (f == NULL)
    {
        printf("fat erra");
    }
    for (int i = 0; i < palTotal; i++)
    {
        char log[64];
        snprintf(log,sizeof(log),"pal %d, %u\n",i,pal[i]);
        fputs(log,f);
    }
    fclose(f);*/

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            int index = (y * width + x) * 3;
            bool uniqueCol = true;
            int R = image[index];
            int G = image[index + 1];
            int B = image[index + 2];
            u16 col15 = ARGB16(1, R, G, B);

            for (int p = 0; p < palTotal; p++)
            {
                if (pal[p] == col15) { uniqueCol = false; break; }
            }
            if (uniqueCol)
            {
                if (palTotal >= 255) { TRACELOG(LOG_WARNING, "too many colors"); continue; }
                pal[palTotal++] = col15;
            }
        }
    }
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            int index = (y * width + x) * 3;
            int R = image[index];
            int G = image[index + 1];
            int B = image[index + 2];
            u16 col15 = ARGB16(1, R, G, B);
            for (int i = 0; i < palTotal; i++)
            {
                if (col15 == pal[i]) gfx[y * width + x] = i;
            }

        }
    }

   /* FILE* f;
    f = fopen("gfx log.txt", "w");
    if (f == NULL)
    {
        printf("fat erra");
    }


    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
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
*/
    free(pal);
    free(gfx);

}
int main()
{
    InitWindow(256,192,"w");

    unsigned error;
  unsigned char* image = 0;
  unsigned width, height;

  error = lodepng_decode24_file(&image, &width, &height, "test.png");
  if(error) TRACELOG(LOG_ERROR,"error %u: %s\n", error, lodepng_error_text(error));
  TRACELOG(LOG_ALL,"%c",image);
  for (int i = 0; i < 12; i++) {
    TRACELOG(LOG_ALL, " %d: %d", i, image[i]);
}


    processPng(image, height, width);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        TRACELOG(LOG_ALL,"smth");

        static bool ran = false;
        if(!ran){
            ran = true;
              for (int i = 0; i < 12; i++) {
    //TRACELOG(LOG_ALL, " %d: %d", i, image[i]);
}
        }
        //TRACELOG(LOG_ALL,"channel %d",S.id);
        EndDrawing();


    }

    
}
