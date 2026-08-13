#include <nds.h>
#include "rcore_ds.h"
#include <arm9/PA_General.h>
#include <arm9/as_lib9.h>

uint32_t read32(const u8 *p)
{
    return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}

uint16_t read16(const u8 *p)
{
    return p[0] | (p[1] << 8);
}

void parseWav()
{
    int dataSize = 0;
    unsigned char* dat = LoadFileData("piano.wav", &dataSize);

    if (dat != NULL)
    {

        SoundInfo s;
        s.data = &dat[44];
        s.size = read32(&dat[40]);
        if (dat[20] == 1)
        {
            int fmt = read16(&dat[16]);
            if (fmt == 8)
            {
                s.format = AS_PCM_8BIT;
            }
            else if (fmt == 16)
            {
                s.format = AS_PCM_16BIT;
            }
        }
        int channels = read16(&dat[22]);
        if (channels == 1)
        {
            s.rate = read32(&dat[24]);
        }
        else if (channels == 2)
        {
            s.rate = read32(&dat[24]) * 2; //todo make it so that 2 channel audio is possible -- this trick suffices for now
        }

        s.volume = 127;
        s.pan = 127;
        s.loop = 1;
        int ch = AS_SoundPlay(s);
        printf("%d",ch);

    }
    else
    {
        printf("Failed to load");
    }
}
int main(void)
{
    PA_Init();
    InitWindow(256, 192, "");

    parseWav();
    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground((Color){15, 15, 25, 255});

        EndDrawing();
    }
    return 0;
}