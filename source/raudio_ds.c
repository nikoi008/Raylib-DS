#include <math.h>
#include "rcore_ds.h"
#include <nds.h>
#include <stdlib.h>
#include <arm9/as_lib9.h>




typedef struct
{
   SoundInfo s;
    bool alias;
}Wave;
#define MAX_SOUNDS_PLAYING 16
typedef struct
{
    SoundInfo s;
    int channel;
    bool playing;
    u32 pausedOffset;
    bool alias;
    bool pendingHandoff;
    u32 startTick;
    int id;
}SoundState;

typedef struct
{
    SoundState *state;

} Sound;

//Sound *playingSounds[16] = {};
SoundState *playingSounds[16] = {};
uint32_t read32(const u8 *p)
{
    return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}

uint16_t read16(const u8 *p)
{
    return p[0] | (p[1] << 8);
}
Wave LoadWaveFromMemory(const char *fileType, const unsigned char *dat, int dataSize)
{
    Wave w;
    if (dat != NULL)
    {


        //w.s.data = &dat[44];
        if (dat[20] == 1)
        {
            int fmt = read16(&dat[34]);
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
            w.s.rate = read32(&dat[24]) * 2;
        }


        //int ch = AS_SoundPlay(s);
        //printf("%d",ch);
        //printf("channels=%d rate=%ld format=%d size=%lu\n", channels, w.s.rate, w.s.format, w.s.size);
        u32 size = read32(&dat[40]);

        memmove(dat,dat + 44,size);
        void* shrunk = realloc(dat,size);
        dat = shrunk;
        w.s.data = dat;
        w.s.size = size;
        w.s.volume = 127;
        w.s.pan = 127;
        w.s.loop = 1;
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
    int datasize;
    unsigned char* data = LoadFileData(fileName,&datasize);
    return LoadWaveFromMemory(GetFileExtension(fileName),data,datasize);
};
bool isWaveValid(Wave w);


Sound LoadSoundFromWave(Wave wave)
{
    Sound s;
    s.state = malloc(sizeof(SoundInfo));
    s.state->s = wave.s;
    s.state->alias = false;
    s.state->id = -1;
    s.state->playing = false;
};
Sound LoadSound(const char *fileName)
{
    return LoadSoundFromWave(LoadWave(fileName));
};

bool IsSoundValid(Sound sound);

void UnloadWave(Wave wave);
void UnloadSound(Sound sound);




bool ExportWave(Wave wave, const char *fileName);
bool ExportWaveAsCode(Wave wave, const char *fileName);

void InitAudioDevice(void);
void StopSound(Sound sound);
void PlaySound(Sound sound);//how will this be passed by va;ue!?!??!
void PauseSound(Sound sound);
void ResumeSound(Sound sound);
void UpdateSounds(); //todo rename and restructurealso fill in audiodevice stuff

bool IsSoundPlaying(Sound sound);
void SetSoundVolume(Sound sound, float volume);
void SetSoundPitch(Sound sound,float pitch);
void SetSoundPan(Sound sound, float pan); // Set pan for a sound (-1.0 left, 0.0 center, 1.0 right)


typedef struct
{
    int mSize;
    unsigned char* mData;
    char* filename;
    int pitch;
    int pan; //todo free
    int volume;
    bool playing; //todo dont forget to also free
}Music;

Music LoadMusicStream(const char *fileName)
{
    char* mData; int mSize;
    mData = LoadFileData(fileName,&mSize);
    return (Music){mSize,mData,fileName,1,64,false};
} // Load music stream from file
Music LoadMusicStreamFromMemory(const char *fileType, const unsigned char *data, int dataSize)
{
    if (strcmp(fileType,"mp3") == 0)
    {
        (Music){dataSize,data,"",1,64,false};
    }
    TRACELOG(LOG_INFO,"MUSIC STREAM CAN ONLY BE LOADED AS MP3");
    return (Music){0,0,0,0};
}; // Load music stream from data
bool IsMusicValid(Music *music)
{
    if (music->mData == NULL) return false;//todo check more
    return true;

};
void UnloadMusicStream(Music *music)
{
    free(music->mData);
    free(music->filename);

};                            // Unload music stream
void PlayMusicStream(Music *music)
{
    DC_FlushRange(music->mData, music->mSize);
    AS_MP3DirectPlay(music->mData, music->mSize);
    music->playing = true;
};                           // Start music playing
bool IsMusicStreamPlaying(Music *music)
{
    return music->playing;
};                         // Check if music is playing
void UpdateMusicStream(Music music);                            // Updates buffers for music streaming todo see what this does -- aslib hadnles this by itself afaik
void StopMusicStream(Music music)
{
    AS_MP3Stop();
};                              // Stop music playing
void PauseMusicStream(Music *music)
{
    if (music->playing == true)
    {
        AS_MP3Pause();
    }

};                             // Pause music playing
void ResumeMusicStream(Music *music)
{
    if (music->playing == false)
    {
        AS_MP3Unpause();
    }
};                            // Resume playing paused music
void SeekMusicStream(Music music, float position)
{
    //impossible afaik
};              // Seek music to a position (in seconds)
void SetMusicVolume(Music *music, float volume)
{
    music->volume = (int)(volume * 127);
    AS_SetMP3Volume(music->volume);
}                 // Set volume for music (1.0 is max level)
void SetMusicPitch(Music *music, float pitch)
{
    music->pitch = (int)(32000.0f * pitch);
    AS_SetMP3Rate(music->pitch);
};                   // Set pitch for a music (1.0 is base level)
void SetMusicPan(Music *music, float pan)
{
    int pI = ((int)(pan * 64.0f)) + 64;
    music->pan = pI;
    AS_SetMP3Pan(pI);
};                       // Set pan for a music (-1.0 left, 0.0 center, 1.0 right)
float GetMusicTimeLength(Music music){}//todo figure out how to do this};                          // Get music time length (in seconds)
float GetMusicTimePlayed(Music music){}//todo add a timer};

//AUDIOSTREAM IS BROKEN ASLIB READS FROM THE SD CARD DURING AN INTERRUPT BUT IT IS BROKEN