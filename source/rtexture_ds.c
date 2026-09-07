
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
            printf("%x\n", pal[i]);
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


        //free(pal);
        UnloadFileData(fileData);
        uint16_t texcoords[4] = {0, 0, (int)size.x,(int)size.y};
        //glImage image[1];
       // glLoadSpriteSet(image,1,texcoords,GL_RGB256,size.x, size.y,TEXGEN_TEXCOORD,256,pal,gfx);
        return (Image){1,pal,gfx,size,image,1};
    }
    else
    {
        TRACELOG(LOG_ALL, "failed to load");
        return (Image){0};
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
            printf("%x\n", pal[i]);
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
        for (int i = 0; i < gfxPos; i++)
        {
            printf("%x",gfx[i]);
        }


        glImage* fImages = malloc(sizeof(glImage) * frames);


         //pushes to vram


        UnloadFileData(fileData);
        // /free(gfx);
        return (Image){1,pal,gfx,size,fImages,frames};
    }
    else
    {
        TRACELOG(LOG_ALL, "failed to load");
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



        //free(pal);
        UnloadFileData(fileData);
        uint16_t texcoords[4] = {0, 0, (int)size.x,(int)size.y};
        //glImage image[1];
        // glLoadSpriteSet(image,1,texcoords,GL_RGB256,size.x, size.y,TEXGEN_TEXCOORD,palTotal,pal,gfx); pushes to vram
        free(pal);
        free(gfx);
        return (Image){1,pal,gfx,size,image,1};
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




Image GenImageColor(int width, int height, Color color);                                           // Generate image: plain color
Image GenImageGradientLinear(int width, int height, int direction, Color start, Color end);        // Generate image: linear gradient, direction in degrees [0..360], 0=Vertical gradient
Image GenImageGradientRadial(int width, int height, float density, Color inner, Color outer);      // Generate image: radial gradient
Image GenImageGradientSquare(int width, int height, float density, Color inner, Color outer);      // Generate image: square gradient
Image GenImageChecked(int width, int height, int checksX, int checksY, Color col1, Color col2);    // Generate image: checked
Image GenImageWhiteNoise(int width, int height, float factor);                                     // Generate image: white noise
Image GenImagePerlinNoise(int width, int height, int offsetX, int offsetY, float scale);           // Generate image: perlin noise
Image GenImageCellular(int width, int height, int tileSize);                                       // Generate image: cellular algorithm, bigger tileSize means bigger cells
Image GenImageText(int width, int height, const char *text);

/*
 *texture2d
 *Image i
 *rotscalepos*/
Texture2D LoadTexture(const char* filename)
{
    Texture2D t;
    t.i = LoadImage(filename);
    uint16_t texcoords[4] = {0, 0, t.i.size.x,t.i.size.y};
    //glImage image[1];
    glLoadSpriteSet(t.i.image,1,texcoords,GL_RGB256,t.i.size.x, t.i.size.y,TEXGEN_TEXCOORD,256,t.i.pal,t.i.gfx);
    return t;
}

Texture2D LoadTextureFromImage(Image i)
{
    Texture2D t;
    t.i = i;

    uint16_t texcoords[4] = {0, 0, t.i.size.x,t.i.size.y};
    //glImage image[1];
    glLoadSpriteSet(t.i.image,1,texcoords,GL_RGB256,t.i.size.x, t.i.size.y,TEXGEN_TEXCOORD,256,t.i.pal,t.i.gfx);
    return t;
}

Texture2D LoadTextureAnimFromImage(Image im)
{
    Texture2D t;
    uint16_t texcoords[4] = {0, 0, im.size.x, im.size.y / im.frames};
    for (int i = 0; i < im.frames; i++)
    {
        u8* minigfx = malloc(im.size.x * (im.size.y / im.frames));
        memcpy(minigfx, im.gfx + (int)(im.size.x * (im.size.y / im.frames) * i), im.size.x * (im.size.y / im.frames));


        textureID = glLoadSpriteSet(&im.image[i], 1, texcoords, GL_RGB256, im.size.x, im.size.y / im.frames, TEXGEN_TEXCOORD, 256, im.pal, minigfx);

        free(minigfx);
    }
    t.i = im;
    return t;
}
void UnloadTexture(Texture2D texture)
{
    UnloadImage(texture.i);
}

void DrawTextureAnim(Texture2D texture, int frame, int posX, int posY, Color tint)
{
    if (frame < 0 || frame >= texture.i.frames) frame = 0;
    glSprite(posX, posY, GL_FLIP_NONE, &texture.i.image[frame]);
}



void DrawTexture(Texture2D texture, int posX, int posY, Color tint)
{
    glSprite(posX, posY, GL_FLIP_NONE, texture.i.image);
}
void DrawTextureRec(Texture2D texture, Rectangle source, Vector2 position, Color tint)
{
    uint16_t texcoords[4] = {source.x,source.y,source.x + source.width,source.y + source.height};

    glImage subImage = *texture.i.image;
    subImage.u_off = texcoords[0];
    subImage.v_off = texcoords[1];
    subImage.width = source.width;
    subImage.height = source.height;

    glSprite((int)position.x, (int)position.y, GL_FLIP_NONE, &subImage);
}

void UnloadTextureAnim(Texture2D texture)
{
    //glDeleteTextures(1, &ruins_texture_id); todo unload texture ids
    free(texture.i.image);
    free(texture.i.gfx);
    free(texture.i.pal);
}