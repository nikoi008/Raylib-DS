
 #include <nds.h>
#include "rcore_ds.h"
//#include <math.h>
#include <gl2d.h>
#include <arm9/PA_General.h>

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
Image LoadImage(const unsigned char* loc)
{
    int datasize;

    unsigned char* data = LoadFileData((const char*)loc,&datasize);

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
    unsigned char *fileData = LoadFileData(filename, &dataSize);
    if (fileData != NULL)
    {

        Image I = LoadImageFromMemory(GetFileExtension(filename),fileData,dataSize);
        //UnloadFileData(fileData); uncommenting this crashes the program, but only for pngs
       printPalette(I.pal,I.colors);
       printGfx(I.gfx, I.size);

        return (Image){I.pal,I.gfx,I.size,frames};

    }
    else
    {
        TRACELOG(LOG_ALL, "failed to load");
        return (Image){0};
    }
}
Image processPPM(unsigned char* fileData,int dataSize)
{
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
    else
    {
        TRACELOG(LOG_ERROR,"PROCESSPPM: FILE DATA IS NULL");
        return (Image){0};
    }

    return (Image){pal,gfx,size,1,palTotal};

}
#include "lodepng.h"

#define TO5BITS >>3 //useless macro go brrr
Image processPng(unsigned char* image, int height, int width)
{
    if ((height > 0 && (height & (height - 1)) != 0)){TRACELOG(LOG_INFO,"IMAGE HEIGHT MUST BE A POWER OF 2"); /*return (Image){0};*/}
    if ((width > 0 && (width & (width - 1)) != 0)){TRACELOG(LOG_INFO,"IMAGE width MUST BE A POWER OF 2"); /*return (Image){0} ;*/}

    u16* pal = malloc(sizeof(u16) * 256);
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
    free(image);
    return (Image){pal,gfx,(Vector2){(float)width,(float)height},1};
}
Image LoadImageFromMemory(const unsigned char *fileType,  const unsigned char *fileData, int dataSize)
{

        if (strcmp(fileType,".png") == 0)
        {
            unsigned char* image = 0;
            unsigned width, height;

            unsigned error = lodepng_decode24(&image, &width, &height,fileData,dataSize);
            if(error) TRACELOG(LOG_ERROR,"error %u: %s\n", error, lodepng_error_text(error));
            UnloadFileData(fileData);

            return processPng(image,(int)height,(int)width);
        }
        if(strcmp(fileType, ".ppm") == 0)
        {
            Image image = processPPM(fileData,dataSize);
            UnloadFileData(fileData);
            return image;


        }

        UnloadFileData(fileData);
}

Image LoadImageFromTexture(Texture2D texture);


void UnloadImage(Image image)
{
    free(image.gfx);
    free(image.pal);
    image.size.x = 0; image.size.y = 0;
   // free(image.image);
}




                                      // Generate image: plain color// Generate image: linear gradient, direction in degrees [0..360], 0=Vertical gradient
Image GenImageGradientRadial(int width, int height, float density, Color inner, Color outer);      // Generate image: radial gradient
Image GenImageGradientSquare(int width, int height, float density, Color inner, Color outer);      // Generate image: square gradient
Image GenImageChecked(int width, int height, int checksX, int checksY, Color col1, Color col2);    // Generate image: checked
Image GenImageWhiteNoise(int width, int height, float factor);                                     // Generate image: white noise
Image GenImagePerlinNoise(int width, int height, int offsetX, int offsetY, float scale);           // Generate image: perlin noise
Image GenImageCellular(int width, int height, int tileSize);                                       // Generate image: cellular algorithm, bigger tileSize means bigger cells
//Image GenImageText(int width, int height, const char *text);


Texture2D LoadTexture(const char* filename)
{
    Texture2D t;
    Image i = LoadImage(filename);
    t = LoadTextureFromImage(i);
    uint16_t texcoords[4] = {0, 0, i.size.x,i.size.y};
    glLoadSpriteSet(t.image,1,texcoords,GL_RGB256,i.size.x, i.size.y,TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT,256,i.pal,i.gfx);
    return t;
}

Texture2D LoadTextureFromImage(Image i)
{   
    Texture2D t;
    //t.i = i;
    //t.id = malloc(sizeof(int) * 1);
    uint16_t texcoords[4] = {0, 0, i.size.x,i.size.y};
    //glImage image[1];
    t.image = malloc(sizeof(glImage) * i.frames);
    t.id = glLoadSpriteSet(t.image,1,texcoords,GL_RGB256,i.size.x, i.size.y,TEXGEN_TEXCOORD ,256,i.pal,i.gfx);
    TRACELOG(LOG_INFO,"LOADED TEXTURE ID %d FRAMES %d \n",t.id,i.frames);
    t.frames = 1;
    return t;
}


Texture2D LoadTextureAnimFromImage(Image im)
{
    Texture2D t = {0};
    t.frames = im.frames;
    t.image = malloc(sizeof(glImage) * im.frames);
    int spriteH = im.size.y / im.frames;

    //t.id = malloc(sizeof(int));
    TRACELOG(LOG_INFO,"BEFORE LOADTILESET \n");
    t.id = glLoadTileSet(t.image,im.size.x, spriteH,im.size.x, im.size.y,GL_RGB256,im.size.x, im.size.y,TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT,256, im.pal, im.gfx);
    TRACELOG(LOG_INFO,"ANIM TEXTURE ID %d\n",t.id);
    return t;
}
void UnloadTexture(Texture2D texture)
{
    //UnloadImage(texture.i);
    TRACELOG(LOG_INFO,"UNLOADING TEXUTRE %d \n",texture.id);
    glDeleteTextures(1,&texture.id);
}
void DrawTextureAnim(Texture2D texture, int frame, int posX, int posY, Color tint)
{
    if (frame < 0 || frame >= texture.frames) frame = 0;
    glSprite(posX, posY, GL_FLIP_NONE, &texture.image[frame]);
}



void DrawTexture(Texture2D texture, int posX, int posY, Color tint)
{
    //rgb = (tint.r,tint.g,tint.b);
    glColor(ARGB16(1,tint.r >> 3, tint.g >> 3, tint.b >> 3));
    glSprite(posX, posY, GL_FLIP_NONE, texture.image);
}




void DrawTextureRec(Texture2D texture, Rectangle source, Vector2 position, Color tint)
{

    glColor(ARGB16(1, tint.r >> 3, tint.g >> 3, tint.b >> 3));
    //todo there is a buffer that needs to be incremented in gl2d or else this goes in front of everything
    int x1 = (int)position.x;
    int y1 = (int)position.y;
    int x2 = x1 + (int)source.width;
    int y2 = y1 + (int)source.height;

    int u1 = texture.image->u_off + (int)source.x;
    int u2 = u1 + (int)source.width;
    int v1 = texture.image->v_off + (int)source.y;
    int v2 = v1 + (int)source.height;

    if (texture.image->textureID != gCurrentTexture)
    {
        glBindTexture(GL_TEXTURE_2D,texture.image->textureID);
        gCurrentTexture = texture.image->textureID;
    }

    glBegin(GL_QUADS);

    glTexCoord2i(u1, v1); glVertex2v16(x1, y1);
    glTexCoord2i(u1, v2); glVertex2v16(x1, y2);
    glTexCoord2i(u2, v2); glVertex2v16(x2, y2);
    glTexCoord2i(u2, v1); glVertex2v16(x2, y1);

    glEnd();
}


void DrawTextureRecAndScale(Texture2D texture, Rectangle source, Vector2 position, Color tint,s32 sx,s32 sy)
{

    glColor(ARGB16(1, tint.r >> 3, tint.g >> 3, tint.b >> 3));
    //todo there is a buffer that needs to be incremented in gl2d or else this goes in front of everything
    int x1 = (int)position.x;
    int y1 = (int)position.y;
    int x2 = x1 + (int)source.width;
    int y2 = y1 + (int)source.height;

    int u1 = texture.image->u_off + (int)source.x;
    int u2 = u1 + (int)source.width;
    int v1 = texture.image->v_off + (int)source.y;
    int v2 = v1 + (int)source.height;

    if (texture.image->textureID != gCurrentTexture)
    {
        glBindTexture(GL_TEXTURE_2D,texture.image->textureID);
        gCurrentTexture = texture.image->textureID;
    }
    glPushMatrix();

    //lTranslatef32(inttof32((int)position.x), inttof32((int)position.y), 0);
    glScalef32(sx, sx, 1 << 12);

    glBegin(GL_QUADS);

    glTexCoord2i(u1, v1); glVertex2v16(x1, y1);
    glTexCoord2i(u1, v2); glVertex2v16(x1, y2);
    glTexCoord2i(u2, v2); glVertex2v16(x2, y2);
    glTexCoord2i(u2, v1); glVertex2v16(x2, y1);

    glEnd();


    glPopMatrix(1);
}

void UnloadTextureAnim(Texture2D texture)
{
    //glDeleteTextures(1, &ruins_texture_id); todo unload texture ids
    for (int i = 0; i < texture.frames; i++)
    {
        TRACELOG(LOG_INFO,"DELETING TEXTURE ID %d",texture.id);
        glDeleteTextures(texture.frames,&texture.id);
    }
}

bool isPowerOfTwo(unsigned int n)
{
    return n != 0 && (n & (n - 1)) == 0;
}

unsigned int nextPO2(unsigned int n)
{
    n--;
    n |= n  >> 1;
    n  |= n  >> 2;
    n  |= n  >> 4;
    n  |= n  >> 8;
    n  |= n  >> 16;
    n ++;
    return n;
}


Image GenImageColor(int width, int height, Color color)
{
    Image i;
    int gfxWidth = isPowerOfTwo(width) ? width : nextPO2(width);
    int gfxHeight = isPowerOfTwo(height) ? height : nextPO2(height);
    i.size.x = (float)gfxWidth;
    i.size.y = (float)gfxHeight;
    i.frames = 1;
    i.colors = 1;
    i.gfx = malloc(sizeof(u8) * gfxHeight * gfxWidth);
    i.pal = malloc(sizeof(u16));
    i.pal[0] = ARGB16(1,color.r TO5BITS, color.g TO5BITS, color.b TO5BITS);
    for (int y = 0; y < gfxHeight; y++)
    {
        for (int x = 0; x < gfxWidth; x++)
        {
            i.gfx[y * gfxWidth + x] = 0;
        }
    }

    return i;
};


int isColInPal(Image *img, Color color) // returns index of color
{
    int convColor = ARGB16(1,color.r TO5BITS, color.g TO5BITS, color.b TO5BITS);
    bool hasCol = false;
    int colIndex = 0;
    for (int i = 0; i < img->colors; i++)
    {
        if (img->pal[i] == convColor) return i;
    }

    if (img->colors >= 255)
    {
        TRACELOG(LOG_WARNING,"ISCOLINPAL: PALETTE FULL");
    }

    img->pal[img->colors] = convColor;
    
    return img->colors++;
}
void ImageClearBackground(Image *dst, Color color)
{

    int colIndex = isColInPal(dst,color);
    for (int y = 0; y < dst->size.y; y++)
    {
        for (int x = 0; x < dst->size.x; x++)
        {
            dst->gfx[y * (int)dst->size.x + x] = colIndex;
        }
    }
};                                                // Clear image background with given color
void ImageDrawPixel(Image *dst, int posX, int posY, Color color)
{
    int colIndex = isColInPal(dst,color);
    if (posX < 0 || posX >= (int)dst->size.x || posY < 0 || posY >= (int)dst->size.y )return;
    dst->gfx[posY * (int)dst->size.x + posX] = colIndex;
};
// Draw pixel within an image

void ImageDrawPixelV(Image *dst, Vector2 position, Color color)
{
    if (position.x < 0 || position.x >= (int)dst->size.x || position.y < 0 || position.y >= (int)dst->size.y )return;
    int colIndex = isColInPal(dst,color);
    dst->gfx[(int)position.y * (int)dst->size.x + (int)position.x] = colIndex;
};                                   // Draw pixel within an image (Vector version)
void ImageDrawLine(Image *dst, int x0, int y0, int x1, int y1, Color color)
{
    //https://gist.github.com/bert/1085538
    int colIndex = isColInPal(dst,color);
    int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */

    for (;;){  /* loop */
        //setPixel (x0,y0);
        if (!(y0 < 0 || y0 >= (int)dst->size.y || x0 < 0 || x0 >= (int)dst->size.x)) dst->gfx[y0 * (int)dst->size.x + x0] = colIndex;
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
        if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
    }
} // Draw line within an image
    void ImageDrawLineV(Image *dst, Vector2 start, Vector2 end, Color color)
{
    ImageDrawLine(dst,start.x,start.y,end.x,end.y,color);
};                          // Draw line within an image (Vector version)
    void ImageDrawLineEx(Image *dst, Vector2 start, Vector2 end, int thick, Color color);              // Draw a line defining thickness within an image
    void ImageDrawCircle(Image *dst, int centerX, int centerY, int radius, Color color)
    {
            //DrawCircleSector((Vector2){ (float)centerX, (float)centerY }, radius, 0, 360, 36, color);
            int x = 0;
            int y = (int)(radius);
            int d = 3 - 2 * (int)(radius);

            while (x <= y)
            {

                ImageDrawLine(dst,centerX - x, centerY + y,centerX + x, centerY + y,color);
                ImageDrawLine(dst,centerX - x, centerY - y, centerX + x, centerY - y,color);
                ImageDrawLine(dst,centerX - y, centerY + x, centerX + y, centerY + x,color);
                ImageDrawLine(dst,centerX -y , centerY - x, centerX + y,centerY - x,color);

                x++;
                if(d< 0)
                {
                    d = d + 4 * x + 6;
                }
                else
                {
                    y = y - 1;
                    d = d + 4 * (x - y) + 10;
                }
            }
    };              // Draw a filled circle within an image
    void ImageDrawCircleV(Image *dst, Vector2 center, int radius, Color color)
    {
        ImageDrawCircle(dst,(int)center.x,(int)center.y,radius,color);
    };                        // Draw a filled circle within an image (Vector version)
    void ImageDrawCircleLines(Image *dst, int centerX, int centerY, int radius, Color color)
    {
            int x = 0;
            int y = (int)(radius);
            int d = 3 - 2 * (int)(radius);

            while (x <= y)
            {
                ImageDrawPixel(dst,centerX + x, centerY + y,color);
                ImageDrawPixel(dst,centerX - x, centerY + y,color);
                ImageDrawPixel(dst,centerX + x, centerY - y,color);
                ImageDrawPixel(dst,centerX - x, centerY - y,color);
                ImageDrawPixel(dst,centerX + y, centerY + x,color);
                ImageDrawPixel(dst,centerX - y, centerY + x,color);
                ImageDrawPixel(dst,centerX + y, centerY - x,color);
                ImageDrawPixel(dst,centerX - y, centerY - x,color);

                x++;
                if(d< 0)
                {
                    d = d + 4 * x + 6;
                }
                else
                {
                    y = y - 1;
                    d = d + 4 * (x - y) + 10;
                }
            }
    };          // Draw circle outline within an image
    void ImageDrawCircleLinesV(Image *dst, Vector2 center, int radius, Color color)
    {
        ImageDrawCircleLines(dst,(int)center.x,(int)center.y,radius,color);
    };                   // Draw circle outline within an image (Vector version)
    void ImageDrawRectangle(Image *dst, int posX, int posY, int width, int height, Color color)
    {
        for (int i = 0; i < height; i++)
        {
            ImageDrawLine(dst,posX,posY,posX + width,posY,color);
        }
    };       // Draw rectangle within an image
    void ImageDrawRectangleV(Image *dst, Vector2 position, Vector2 size, Color color)
    {
        ImageDrawRectangle(dst,(int)position.x,(int)position.y,(int)size.x,(int)size.y,color);
    };                 // Draw rectangle within an image (Vector version)
    void ImageDrawRectangleRec(Image *dst, Rectangle rec, Color color)
    {
        ImageDrawRectangle(dst,rec.x,rec.y,rec.x + rec.width,rec.y + rec.height,color);
    };                                // Draw rectangle within an image
    void ImageDrawRectangleLines(Image *dst, Rectangle rec, int thick, Color color)
    {
        ImageDrawLine(dst,rec.x,rec.y,rec.x + rec.width,rec.y,color);
        ImageDrawLine(dst,rec.x,rec.y,rec.x,rec.y + rec.height,color);
        ImageDrawLine(dst,rec.x + rec.width,rec.y,rec.x + rec.width,rec.y + rec.height,color);
        ImageDrawLine(dst,rec.x,rec.y + rec.height,rec.x + rec.width,rec.y + rec.height,color);
    }
    void ImageDrawTriangle(Image *dst, Vector2 v1, Vector2 v2, Vector2 v3, Color color);// Draw triangle within an image
    void ImageDrawTriangleEx(Image *dst, Vector2 v1, Vector2 v2, Vector2 v3, Color c1, Color c2, Color c3);


// Draw triangle with interpolated colors within an image
    void ImageDrawTriangleLines(Image *dst, Vector2 v1, Vector2 v2, Vector2 v3, Color color)
    {
        ImageDrawLine(dst,(int)v1.x,(int)v1.y,(int)v2.x,(int)v2.y,color);
        ImageDrawLine(dst,(int)v2.x,(int)v2.y,(int)v3.x,(int)v3.y,color);
        ImageDrawLine(dst,(int)v3.x,(int)v3.y,(int)v1.x,(int)v1.y,color);
    };          // Draw triangle outline within an image
    void ImageDrawTriangleFan(Image *dst, const Vector2 *points, int pointCount, Color color);         // Draw a triangle fan defined by points within an image (first vertex is the center)
    void ImageDrawTriangleStrip(Image *dst, const Vector2 *points, int pointCount, Color color);       // Draw a triangle strip defined by points within an image


//todo figure out what to do with these? possible but it will be very slow
    void ImageDraw(Image *dst, Image src, Rectangle srcRec, Rectangle dstRec, Color tint);// Draw a source image within a destination image (tint applied to source)
    void ImageDrawText(Image *dst, const char *text, int posX, int posY, int fontSize, Color color);   // Draw text (using default font) within an image (destination)
    void ImageDrawTextEx(Image *dst, Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint);