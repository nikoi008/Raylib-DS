//
// Created by Nmsou on 05/08/2026.
//#
/*
#include
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
Image createTexture(char* loc)
{
    glImage image[1];
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
       // glLoadSpriteSet(image,1,texcoords,GL_RGB256,size.x, size.y,TEXGEN_TEXCOORD,256,pal,gfx); pushes to vram
       // free(pal);
       // free(gfx);
        return (Image){1,pal,gfx,size,image[0]};
    }
    else
    {
        TRACELOG(LOG_ALL, "failed to load");
        //return (Image){NULL};
    }



}*/