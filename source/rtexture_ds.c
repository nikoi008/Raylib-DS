
 #include <nds.h>
#include "rcore_ds.h"
//#include <math.h>
#include <gl2d.h>
#include "rshapes_ds.h"

Vector2 getImageSize(const char* buffer)
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


    size.x = (float)atoi(bH);
    //printf("\n\n%f \n\n",size.x);


    char bW[16];
    int bWPlace = 0;

    while (buffer[bufferPlace] != '\n')
    {
        bW[bWPlace] = buffer[bufferPlace];
        bufferPlace++;
        bWPlace++;
    }
    size.y = (float)atoi(bW);
    //printf("\n\n%f \n\n",size.y);

    return size;
}
Image LoadImage(const char* loc)
{
    int datasize;
    unsigned char* data = LoadFileData(loc,&datasize);

    return LoadImageFromMemory(GetFileExtension(loc),data,datasize);


}
Image LoadImageRaw(const char *fileName, int width, int height, int format, int headerSize)
{

    return LoadImage(fileName); // todo later
}
void printPalette(u16* pal, int count)
{
    printf("\n [PRINTPALETTE]: count %d",count);
    for (int i = 0; i < count; i++)
    {
        printf("\nPAL %d %04X",i,pal[i]);
    }
    printf("\n");
}

void printGfx(u8* gfx, Vector2 size)
{
    for (int height = 0; height < (int)size.y; height++)
    {
        for (int width = 0; width < (int)size.x; width++)
        {
            printf("%d",gfx[width * height + width]);
        }

    }

}
Image LoadImageAnim(const char* filename, int frames)
{
    int dataSize = 0;
    //u8 *gfx;
    //u16 *pal;
    //Vector2 size;
    unsigned char *fileData = LoadFileData(filename, &dataSize);
    if (fileData != NULL)
    {

        Image I = LoadImageFromMemory(GetFileExtension(filename),fileData,dataSize);
        glImage* fImages = malloc(sizeof(glImage) * frames);
        UnloadFileData(fileData);
        // /free(gfx);
        printPalette(I.pal,I.colors);
        printGfx(I.gfx, I.size);
        return (Image){I.pal,I.gfx,I.size,fImages,frames};
    }
    else
    {
        TRACELOG(LOG_ALL, "failed to load");
        return (Image){0};
    }
}
Image processPPM(unsigned char* fileData,int dataSize)
{
    glImage *image;
    image = malloc(sizeof(glImage));
    u8 *gfx;
    u16 *pal;
    Vector2 size;
    int palTotal = 0;
   if (fileData != NULL)
    {
        printf("data ok");
        pal = malloc(sizeof(u16) * 255);


        int idx = 0;
        int newlines = 0;
        while (newlines < 1 && idx < dataSize)
        {
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


    }

    /*FILE* fa;
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


       FILE* f;
        f = fopen("gfx log.txt", "w");
        if (f == NULL)
        {
            printf("fat erra");
        }


        for (int y = 0; y < size.y; y++)
        {
            for (int x = 0; x < size.y; x++)
            {
                int index = (y * size.x + x) * 3;
                char log[64];
                snprintf(log,sizeof(log),"%u",gfx[y * (int)size.x + x]);
                fputs(log,f);
                if (x == size.x - 1)
                {
                    fputs("\n",f);
                }
            }
        }
        fclose(f);*/
    return (Image){pal,gfx,size,image,1,palTotal};

}
#include "lodepng.h"

#define TO5BITS >>3 //useless macro go brrr
Image processPng(unsigned char* image, int height, int width)
{
    if ((height > 0 && (height & (height - 1)) != 0)){TRACELOG(LOG_INFO,"IMAGE HEIGHT MUST BE A POWER OF 2"); /*return (Image){0};*/}
    if ((width > 0 && (width & (width - 1)) != 0)){TRACELOG(LOG_INFO,"IMAGE width MUST BE A POWER OF 2"); /*return (Image){0} ;*/}

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

            for (int i = 0; i < palTotal; i++)
            {
                if (col15 == pal[i]) gfx[y * width + x] = i;
            }
        }
    }
    return (Image){pal,gfx,(Vector2){width,height},immage,1,palTotal};
}
Image LoadImageFromMemory(const unsigned char *fileType,  const unsigned char *fileData, int dataSize)
{

        if (strcmp(fileType,".png") == 0)
        {
            unsigned error;
            unsigned char* image = 0;
            unsigned width, height;

            error = lodepng_decode24(&image, &width, &height,fileData,dataSize);
            if(error) TRACELOG(LOG_ERROR,"error %u: %s\n", error, lodepng_error_text(error));
            UnloadFileData(fileData);

            return processPng(image,(int)height,(int)width);
        }
        if(strcmp(fileType, ".ppm") == 0)
        {
            return processPPM(fileData,dataSize);
            UnloadFileData(fileData);
        }

        UnloadFileData(fileData);
}

Image LoadImageFromTexture(Texture2D texture);//todo see how this can be done


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


Texture2D LoadTexture(const char* filename)
{
    Texture2D t;
    t.i = LoadImage(filename);
    uint16_t texcoords[4] = {0, 0, t.i.size.x,t.i.size.y};
    glLoadSpriteSet(t.i.image,1,texcoords,GL_RGB256,t.i.size.x, t.i.size.y,TEXGEN_TEXCOORD,256,t.i.pal,t.i.gfx);
    return t;
}

Texture2D LoadTextureFromImage(Image i)
{
    Texture2D t;
    t.i = i;
    t.id = malloc(sizeof(int) * 1);
    uint16_t texcoords[4] = {0, 0, t.i.size.x,t.i.size.y};
    //glImage image[1];
    t.id[0] = glLoadSpriteSet(t.i.image,1,texcoords,GL_RGB256,t.i.size.x, t.i.size.y,TEXGEN_TEXCOORD,256,t.i.pal,t.i.gfx);
    TRACELOG(LOG_INFO,"LOADED TEXTURE ID %d FRAMES %d \n",t.id,i.frames);
    return t;
}

Texture2D LoadTextureAnimFromImage(Image im)
{
    Texture2D t;
    printf("\n\n\n TOTAL FRAMS %d",im.frames);
    t.id = malloc(sizeof(int) * im.frames);
    uint16_t texcoords[4] = {0, 0, (uint16_t)im.size.x, (uint16_t)im.size.y / (uint16_t)im.frames};
    for (int i = 0; i < im.frames; i++)
    {
        u8* minigfx = malloc(im.size.x * (im.size.y / im.frames));
        memcpy(minigfx, im.gfx + (int)(im.size.x * (im.size.y / im.frames) * i), im.size.x * (im.size.y / im.frames));


        t.id[i] = glLoadSpriteSet(&im.image[i], 1, texcoords, GL_RGB256, im.size.x, im.size.y / im.frames, TEXGEN_TEXCOORD, 256, im.pal, minigfx);
        printf("TEXTURE ID %d",*t.id);
        free(minigfx);
    }
    //t.i = im;
    return t;
}
void UnloadTexture(Texture2D texture)
{
    UnloadImage(texture.i);
    TRACELOG(LOG_INFO,"UNLOADING TEXUTRE %d \n",texture.id);
    glDeleteTextures(1,texture.id);

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
    for (int i = 0; i < texture.i.frames; i++)
    {
        TRACELOG(LOG_INFO,"DELETING TEXTURE ID %d",texture.id[i]);
        glDeleteTextures(texture.i.frames,&texture.id[i]);
    }

    free(texture.i.image);
    free(texture.i.gfx);
    free(texture.i.pal);
    free(texture.id);
}