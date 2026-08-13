#include <nds.h>
#include "rcore_ds.h"
#include <arm9/PA_General.h>
#include <arm9/as_lib9.h>
typedef  struct
{
    SoundInfo s;
}Wave;
uint32_t read32(const u8 *p);
uint16_t read16( const u8 *p);
Wave LoadWaveFromMemory(const char *fileType, const unsigned char *dat, int dataSize)
{
    Wave w;
    if (dat != NULL)
    {


        w.s.data = &dat[44];
        w.s.size = read32(&dat[40]);
        if (dat[20] == 1)
        {
            int fmt = read16(&dat[16]);
            if (fmt == 8)
            {
                w.s.format = AS_PCM_8BIT;
            }
            else if (fmt == 16)
            {
                w.s.format = AS_PCM_16BIT;
            }
        }
        int channels = read16(&dat[22]);
        if (channels == 1)
        {
            w.s.rate = read32(&dat[24]);
        }
        else if (channels == 2)
        {
            w.s.rate = read32(&dat[24]) * 2; //todo make it so that 2 channel audio is possible -- this trick suffices for now
        }

        w.s.volume = 127;
        w.s.pan = 127;
        w.s.loop = 1;
        //int ch = AS_SoundPlay(s);
        //printf("%d",ch);
        return w;

    }
    else
    {
        printf("Failed to load");
        return (Wave){0};
    }
}
Wave LoadWave(const char* fileName)
{
    int dataSize = 0;
    unsigned char* dat = LoadFileData(fileName,&dataSize);
    if (dat != NULL)
    {
        Wave w = LoadWaveFromMemory(GetFileExtension(fileName),dat,dataSize);
        return w;
    }
    else
    {
        printf("Failed to load %s",fileName);
    }
}

bool isWaveValid(Wave w)
{
    return true; // todo deal with this later
}

uint32_t read32(const u8 *p)
{
    return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}

uint16_t read16(const u8 *p)
{
    return p[0] | (p[1] << 8);
}
typedef struct
{
    Wave w;
    bool alias;
}Sound;
Sound LoadSound(const char *fileName)
{
    Sound s;
    s.w = LoadWave(fileName);
    //todo add rest of stuff once sound is filled
};                          // Load sound from file
Sound LoadSoundFromWave(Wave wave)
{
    return (Sound){wave};
};                             // Load sound from wave data
Sound LoadSoundAlias(Sound source)
{

};                             // Create a new sound that shares the same sample data as the source sound, does not own the sound data
bool IsSoundValid(Sound sound)
{
    if (sound.w.s.data != NULL) return true; //todo make more robust
    return false;
};                                 // Checks if a sound is valid (data loaded and buffers initialized)
void UpdateSound(Sound sound, const void *data, int sampleCount); // Update sound buffer with new data (default data format: 16 bit integer, stereo)
void UnloadWave(Wave wave)
{
    free(wave.s.data);
    wave.s.size = 0;
    wave.s.format = -1;
    wave.s.volume = 0;
};                                     // Unload wave data
void UnloadSound(Sound sound)
{
    UnloadWave(sound.w);
};                                  // Unload sound
void UnloadSoundAlias(Sound alias)
{
    //todo
};                             // Unload a sound alias (does not deallocate sample data)
bool ExportWave(Wave wave, const char *fileName)
{
    return false; //todo
};               // Export wave data to file, returns true on success
bool ExportWaveAsCode(Wave wave, const char *fileName)
{
    //todo
};
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