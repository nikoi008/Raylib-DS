#include <nds.h>
#include "rcore_ds.h"
#include <math.h>
#include <gl2d.h>
int textureID;
typedef struct
{
    int id;
    u16 *pal;
    u8* gfx;

};
void createTexture()
{
     int dataSize = 0;
    unsigned char *fileData = LoadFileData("e.ppm", &dataSize);
    u16 *pal = malloc(sizeof(u16) * 255);
    u8 gfx[64 * 64];
    if (fileData != NULL)
    {
        //TRACELOG(LOG_ALL, "ppm size %d ", dataSize);

        int idx = 0;
        int newlines = 0;
        while (newlines < 3 && idx < dataSize) {
            if (fileData[idx] == '\n') newlines++;
            idx++;
        }

        int palTotal = 0;
        //u16 *pal = malloc(256 * sizeof(u16));
        //u16 pal[256];
        for (int i = idx; i < dataSize; i += 3)
        {

            u16 r5 = fileData[i] >> 3;
            u16 g5 = fileData[i + 1] >> 3;
            u16 b5 = fileData[i + 2] >> 3;

            u16 col = ARGB16(1, r5, g5, b5);

            bool found = false;
            for (int p = 0; p < palTotal; p++)
            {
                if (pal[p] == col)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                if (palTotal <= 256)
                {
                    pal[palTotal] = col;
                    palTotal++; //todo

                }
            }
        }
        //TRACELOG(LOG_ALL, "%d cols", palTotal);

        for (int i = 0; i < palTotal; i++ )
        {
            //printf("%x\n", pal[i]);
        }

        //u8 *gfx = malloc(sizeof(u8) * 16 * 16); // 16 is image height/width todo dont hardcode
        //u8 gfx[16 * 16];
        for (int i = idx; i < dataSize;i+= 3)
        {
            static int gfxPos = 0;
            u16 r5 = fileData[i] >> 3;
            u16 g5 = fileData[i + 1] >> 3;
            u16 b5 = fileData[i + 2] >> 3;

            u16 col = ARGB16(1, r5, g5, b5);

            for (int j = 0; j < palTotal;j++)
            {
                if (col == pal[j])
                {
                    gfx[gfxPos] =j;
                    gfxPos++;
                    break;
                }
            }
        }
        for (int i = 0; i < 16 * 16; i++)
        {
            //printf("%d", gfx[i]);
        }

        glGenTextures(1, &textureID);
        glBindTexture(0, textureID);
        glColorTableNtr(palTotal, pal);

        //free(pal);
        //UnloadFileData(fileData);
    }
    else
    {
        TRACELOG(LOG_ALL, "failed to load");
    }


    uint16_t texcoords[4] = {0, 0, 64, 64};
    glImage image[1];
    glLoadSpriteSet(image,1,texcoords,GL_RGB256,64, 64,TEXGEN_TEXCOORD,256,pal,gfx);

}

int main(void)
{
    InitWindow(256, 192, "");


    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground((Color){15, 15, 25, 255});
        glEnable(GL_TEXTURE_2D);
        glSprite(50, 50, GL_FLIP_NONE, &image[0]);

        EndDrawing();
    }
    return 0;
}