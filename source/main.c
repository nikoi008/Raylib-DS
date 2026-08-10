#include <nds.h>
#include "rcore_ds.h"
#include <math.h>
#include <gl2d.h>
#include "rshapes_ds.h"
int textureID;



Vector2 getImageSize(char* buffer)
{
    Vector2 size;
    int bufferPlace = 0;
    char bH[16];
    int bHPlace = 0;

    while (buffer[bufferPlace] != ' ')
    {
        bH[bHPlace] = buffer[bufferPlace];
        bufferPlace++;
        bHPlace++;
    }


    size.x = atoi(bH);
    //printf("\n\n%f \n\n",size.x);


    char bW[16];
    int bWPlace = 0;

    while (buffer[bufferPlace] != '\n')
    {
        bW[bWPlace] = buffer[bufferPlace];
        bufferPlace++;
        bWPlace++;
    }
    size.y = atoi(bW);
    //printf("\n\n%f \n\n",size.y);

    return size;
}
Image LoadImage(char* loc)
{
    glImage *image = malloc(sizeof(glImage));
    int dataSize = 0;
    u8 *gfx;
    u16 *pal;
    Vector2 size;

    unsigned char *fileData = LoadFileData(loc, &dataSize);
    if (fileData != NULL)
    {

        pal = malloc(sizeof(u16) * 255);


        int idx = 0;
        int newlines = 0;
        while (newlines < 1 && idx < dataSize) {
            if (fileData[idx] == '\n') newlines++;
            idx++;
        }
        char buffer[64];
        int bIdx = 0;
        while (newlines < 2 && idx < dataSize)
        {
            buffer[bIdx] = fileData[idx];
            bIdx++;
            if (fileData[idx] == '\n') newlines++;
            idx++;
        }
        size = getImageSize(buffer);

        gfx = malloc(sizeof(u8) * (int)size.x * (int)size.y);
        while (newlines < 3 && idx < dataSize) {
            if (fileData[idx] == '\n') newlines++;
            idx++;
        }



        int palTotal = 0;
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
        int gfxPos = 0;
        for (int i = idx; i < dataSize;i+= 3)
        {

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


        glGenTextures(1, &textureID);
        glBindTexture(0, textureID);
        glColorTableNtr(palTotal, pal);

        //free(pal);
        UnloadFileData(fileData);
        uint16_t texcoords[4] = {0, 0, (int)size.x,(int)size.y};
        //glImage image[1];
        glLoadSpriteSet(image,1,texcoords,GL_RGB256,size.x, size.y,TEXGEN_TEXCOORD,256,pal,gfx);
        return (Image){1,pal,gfx,size,image};
    }
    else
    {
        TRACELOG(LOG_ALL, "failed to load");
        //return (Image){NULL};
    }



}
Image LoadImageRaw(const char *fileName, int width, int height, int format, int headerSize)
{
    return LoadImage(fileName); // todo later
}

Image LoadImageAnim(const char* filename, int frames)
{
    int dataSize = 0;
    u8 *gfx;
    u16 *pal;
    Vector2 size;
    unsigned char *fileData = LoadFileData(filename, &dataSize);
    if (fileData != NULL)
    {

        pal = malloc(sizeof(u16) * 255);


        int idx = 0;
        int newlines = 0;
        while (newlines < 1 && idx < dataSize) {
            if (fileData[idx] == '\n') newlines++;
            idx++;
        }
        char buffer[64];
        int bIdx = 0;
        while (newlines < 2 && idx < dataSize)
        {
            buffer[bIdx] = fileData[idx];
            bIdx++;
            if (fileData[idx] == '\n') newlines++;
            idx++;
        }
        size = getImageSize(buffer);

        gfx = malloc(sizeof(u8) * (int)size.x * (int)size.y);
        while (newlines < 3 && idx < dataSize) {
            if (fileData[idx] == '\n') newlines++;
            idx++;
        }



        int palTotal = 0;
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
        int gfxPos = 0;
        for (int i = idx; i < dataSize;i+= 3)
        {

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

        glImage* images = malloc(sizeof(glImage) * frames);
        uint16_t *texcoords = malloc(sizeof(uint16_t) * 4 * frames);

        for (int i = 0 ; i < frames; i++)
        {
            texcoords[i * 4 + 0] = 0;
            texcoords[i * 4 + 1] = i * (size.y / frames);
            texcoords[i * 4 + 2] = size.x;
            texcoords[i *4 + 3] = (size.y / frames) * (i + 1);
        }

        glGenTextures(1, &textureID);
        glBindTexture(0, textureID);
        glColorTableNtr(palTotal, pal);

        textureID =glLoadSpriteSet(images,frames,texcoords,GL_RGB256,size.x,size.y,TEXGEN_TEXCOORD,palTotal,pal,gfx);

        UnloadFileData(fileData);
        return (Image){1,pal,gfx,size,images};
    }
    else
    {
        TRACELOG(LOG_ALL, "failed to load");
        //return (Image){NULL};
    }
}

Image LoadImageFromMemory(const char *fileType, const unsigned char *fileData, int dataSize)
{
    glImage image[1];
    u8 *gfx;
    u16 *pal;
    Vector2 size;
    if (fileData != NULL)
    {

        pal = malloc(sizeof(u16) * 255);


        int idx = 0;
        int newlines = 0;
        while (newlines < 1 && idx < dataSize) {
            if (fileData[idx] == '\n') newlines++;
            idx++;
        }
        char buffer[64];
        int bIdx = 0;
        while (newlines < 2 && idx < dataSize)
        {
            buffer[bIdx] = fileData[idx];
            bIdx++;
            if (fileData[idx] == '\n') newlines++;
            idx++;
        }
        size = getImageSize(buffer);

        gfx = malloc(sizeof(u8) * (int)size.x * (int)size.y);
        while (newlines < 3 && idx < dataSize) {
            if (fileData[idx] == '\n') newlines++;
            idx++;
        }



        int palTotal = 0;
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
        int gfxPos = 0;
        for (int i = idx; i < dataSize;i+= 3)
        {

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


        glGenTextures(1, &textureID);
        glBindTexture(0, textureID);
        glColorTableNtr(palTotal, pal);

        //free(pal);
        UnloadFileData(fileData);
        uint16_t texcoords[4] = {0, 0, (int)size.x,(int)size.y};
        //glImage image[1];
        // glLoadSpriteSet(image,1,texcoords,GL_RGB256,size.x, size.y,TEXGEN_TEXCOORD,256,pal,gfx); pushes to vram
        free(pal);
        free(gfx);
        return (Image){1,pal,gfx,size,image};
    }
}

Image LoadImageFromTexture(Texture2D texture);//todo see how this can be done

//Image LoadImageFromScreen(void); screenshot then as tileset?

void UnloadImage(Image image)
{
    free(image.gfx);
    free(image.pal);
    image.size.x = 0; image.size.y = 0;
    free(image.image);
}

bool ExportImage(Image image)
{
    //just reverse everything that i did in loadimage
    return false;
}

bool ExportImageAsCode(Image image, const char* filename)
{
    for (int i = 0; i < sizeof(image.gfx) / sizeof(u8); i++)
    {

    }
}
int main(void)
{
    InitWindow(256, 192, "");
    Image  sprite = LoadImageAnim("ass.ppm",2);
    Image sprite2 = LoadImage("e.ppm");
    while (!WindowShouldClose())
    {
        static int x = 0;
        static int y = 0;
        BeginDrawing();

        if (IsKeyDown(KEY_RIGHT)){x+= 2;}
        if (IsKeyDown(KEY_LEFT)){x-= 2;}
        if (IsKeyDown(KEY_DOWN)){ y+= 2;}
        if (IsKeyDown(KEY_UP)){y-= 2;}
        ClearBackground((Color){15, 15, 25, 255});
        glEnable(GL_TEXTURE_2D);
        glSprite(x, y, GL_FLIP_NONE, &sprite.image[0]);
        glSprite(50,50,GL_FLIP_NONE,&sprite.image[1]);
        glSprite(150, 50, GL_FLIP_NONE, &sprite2.image[0]);

        EndDrawing();
    }
    return 0;
}