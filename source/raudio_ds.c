#include <math.h>
#include "rcore_ds.h"
#include <nds.h>
#include <stdlib.h>
#include <arm9/as_lib9.h>




typedef  struct
{
    SoundInfo s;
    bool alias;
}Wave;

typedef struct
{
    //SoundInfo s;
    int channel;
    bool playing;
    u32 pausedOffset;
    bool alias;
    bool pendingHandoff;
    int startTick;
    int id;
}SoundState;

typedef struct
{
    SoundState *state;
    Wave w;
} Sound;

Sound *playingSounds[16] = {};

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


        w.s.data = &dat[44];
        w.s.size = read32(&dat[40]);
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

        w.s.volume = 127;
        w.s.pan = 127;
        w.s.loop = 1;
        //int ch = AS_SoundPlay(s);
        //printf("%d",ch);
        //printf("channels=%d rate=%ld format=%d size=%lu\n", channels, w.s.rate, w.s.format, w.s.size);
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


Sound LoadSound(const char *fileName)
{
    Sound s;
    s.w = LoadWave(fileName);
    s.state->alias = false;
    s.state->channel = -1;
    s.state->playing = false;

    s.state->id = -99;

    return s;
}
Sound LoadSoundFromWave(Wave wave)
{

    Sound s = {};
    s.w = wave;
    s.state->id = -99;

    return (Sound){s.state,wave};
};

bool IsSoundValid(Sound sound)
{
    if (sound.w.s.data != NULL) return true; //todo make more robust
    return false;
};            // Checks if a sound is valid (data loaded and buffers initialized)
//void UpdateSound(Sound sound, const void *data, int sampleCount); // Update sound buffer with new data (default data format: 16 bit integer, stereo)



void UnloadWave(Wave wave)
{
    free(wave.s.data);
    wave.s.size = 0;
    wave.s.format = -1;
    wave.s.volume = 0;
};
void UnloadSound(Sound sound)
{
    UnloadWave(sound.w);
    free(sound.state);
};




bool ExportWave(Wave wave, const char *fileName)
{
    return false; //todo
};
bool ExportWaveAsCode(Wave wave, const char *fileName)
{
    //todo
};


void InitAudioDevice(void)
{
    irqSet(IRQ_VBLANK, AS_SoundVBL);
    irqEnable(IRQ_VBLANK);
    AS_Init(AS_MODE_MP3 | AS_MODE_SURROUND | AS_MODE_16CH);
    AS_SetDefaultSettings(AS_PCM_8BIT, 11025, AS_SURROUND);
    DS.audioOn = true;

}


#define MAX_SOUNDS_PLAYING 16
void StopSound(Sound sound)
{
    if (sound.state->channel < 0) return;
    soundKill(sound.state->channel);
    sound.state->playing = false;
    sound.state->id = -99;
}
void PlaySound(Sound sound)//how will this be passed by va;ue!?!??!
{
    sound.state->pausedOffset = 0;
    sound.state->channel = AS_SoundPlay(sound.w.s);
    sound.state->playing = true;
    sound.state->pendingHandoff = false;
    sound.state->startTick = cpuGetTiming();
    if (sound.state->id < 0)
    {
        for (int i = 0; i < MAX_SOUNDS_PLAYING; i++)
        {
            if (playingSounds[i] != NULL) continue;
            else
            {
                playingSounds[i] = &sound;
                sound.state->id = i;
                break;
            }

        }
        if (sound.state->id < 0) TRACELOG(LOG_INFO,"SOUND: ALL CHANNELS ALREADY PLAYING MUSIC");
    }
    else
    {
        TRACELOG(LOG_INFO,"SOUND: SOUND ALREADY PLAYING");
    }

}
void PauseSound(Sound sound)
{

    if (!playingSounds[sound.state->id]->state->playing || playingSounds[sound.state->id]->state->channel < 0) return;

    u32 elapsedTicks = cpuGetTiming() - playingSounds[sound.state->id]->state->startTick;
    float elapsedSeconds = (float)elapsedTicks / BUS_CLOCK;

    int bytesPerSample = (playingSounds[sound.state->id]->w.s.format == AS_PCM_16BIT) ? 2 : 1;
    u32 elapsedBytes = (u32)(elapsedSeconds * playingSounds[sound.state->id]->w.s.rate) * bytesPerSample;

    playingSounds[sound.state->id]->state->pausedOffset += elapsedBytes;
    playingSounds[sound.state->id]->state->pausedOffset %= playingSounds[sound.state->id]->w.s.size;

    AS_SoundStop(playingSounds[sound.state->id]->state->channel);
    playingSounds[sound.state->id]->state->playing = false;
    playingSounds[sound.state->id]->state->pendingHandoff = false;
}
void ResumeSound(Sound sound)
{
    if (playingSounds[sound.state->id]->state->playing ||
        playingSounds[sound.state->id]->state->pausedOffset >= playingSounds[sound.state->id]->w.s.size) return;

    SoundInfo tail = playingSounds[sound.state->id]->w.s;
    tail.data = playingSounds[sound.state->id]->w.s.data + playingSounds[sound.state->id]->state->pausedOffset;
    tail.size = playingSounds[sound.state->id]->w.s.size - playingSounds[sound.state->id]->state->pausedOffset;
    tail.loop = 0;

    playingSounds[sound.state->id]->state->channel = AS_SoundPlay(tail);
    playingSounds[sound.state->id]->state->playing = true;
    playingSounds[sound.state->id]->state->pendingHandoff = true;
    playingSounds[sound.state->id]->state->startTick = cpuGetTiming();
}

void UpdateSounds() //todo rename and restructure
{
    for (int i = 0; i < MAX_SOUNDS_PLAYING; i++)
    {
        if (playingSounds[i] == NULL){ continue;}
        if (!playingSounds[i]->state->playing || !playingSounds[i]->state->pendingHandoff) return;
        if (playingSounds[i]->state->channel < 0) return;
        if (!IPC_Sound->chan[playingSounds[i]->state->channel].busy)
        {
            TRACELOG(LOG_INFO,"[UPTSOUNDS] SOUND %d PLAYING",i);
            SoundInfo full = playingSounds[i]->w.s;
            full.loop = 1;

            playingSounds[i]->state->channel = AS_SoundPlay(full);
            playingSounds[i]->state->pausedOffset = 0;
            playingSounds[i]->state->pendingHandoff = false;
            playingSounds[i]->state->startTick = cpuGetTiming();
        }
    }
}
//todo also fill in audiodevice stuff

bool IsSoundPlaying(Sound sound)
{
    //return sound.playing;
    if (sound.state->id >= 0) return true;
    return false;
}
void SetSoundVolume(Sound sound, float volume)
{

    float v = volume * 127;
    AS_SetSoundVolume(playingSounds[sound.state->id]->state->channel,(int)v);
}
void SetSoundPitch(Sound sound,float pitch)
{
    sound.w.s.rate = (int)(sound.w.s.rate * pitch);
    AS_SetSoundRate(playingSounds[sound.state->id]->state->channel,sound.w.s.rate);
}
void SetSoundPan(Sound sound, float pan) // Set pan for a sound (-1.0 left, 0.0 center, 1.0 right)
{
    int pI = ((int)(pan * 64.0f)) + 64;
    sound.w.s.pan = pI;
    AS_SetSoundPan(playingSounds[sound.state->id]->state->channel,pI);

}


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