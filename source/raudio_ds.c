//
// Created by Nmsou on 13/08/2026.
//


#include "rcore_ds.h"
#include <nds.h>
#include <stdlib.h>
#include <arm9/as_lib9.h>
#include <arm9/PA_General.h>

#include "../../../../../../msys64/opt/wonderful/thirdparty/blocksds/external/palib/include/arm9/as_lib9.h"


typedef  struct
{
    SoundInfo s;
    bool alias;
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
    int channel;
    bool playing;
}Sound;
Sound LoadSound(const char *fileName)
{
    Sound s;
    s.w = LoadWave(fileName);
    //todo add rest of stuff once sound is filled
    s.alias = false;
    return s;
};                          // Load sound from file
Sound LoadSoundFromWave(Wave wave)
{
    return (Sound){wave,false};
};                             // Load sound from wave data
Sound LoadSoundAlias(Sound source)
{
    return (Sound){source.w,true};
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
    if (alias.alias != true) return;


    alias.w.s.data = NULL;
    alias.w.s.size = 0;

};                             // Unload a sound alias (does not deallocate sample data)
bool ExportWave(Wave wave, const char *fileName)
{
    return false; //todo
};               // Export wave data to file, returns true on success
bool ExportWaveAsCode(Wave wave, const char *fileName)
{
    //todo
};


void InitAudioDevice(void)
{
    PA_VBLFunctionInit(AS_SoundVBL);
    AS_Init(AS_MODE_MP3);
    AS_SetMasterVolume(127);
}


void PlaySound(Sound sound)
{
    sound.channel = AS_SoundPlay(sound.w.s);
}

void StopSound(Sound sound)
{
    if (sound.channel == NULL || sound.channel < 0 || sound.channel > 15) return;
    AS_SoundStop(sound.channel);
}

void PauseSound(Sound sound)
{//doesnt fucking exist

    if (sound.playing == true)
    {
        AS_Sound();
        sound.playing = false;
    }AS_SoundStop();
}

void ResumeSound(Sound sound)
{//same with pausing todo figure out what to do later
    if (sound.playing == false)
    {
        AS_MP3Unpause();
        sound.playing = true;
    }
}

bool IsSoundPlaying(Sound sound)
{
    return sound.playing;
}

void SetSoundVolume(Sound sound, float volume)
{
    //todo check if this exists
}

void SetSoundPitch(Sound sound,float pitch) //todo make this work with more than 32k sample rate
{
    sound.pitch = (int)(32000.0f * pitch);
    AS_SetMP3Rate(sound.pitch);
}

void SetSoundPan(Sound sound, float pan) // Set pan for a sound (-1.0 left, 0.0 center, 1.0 right)
{
    int pI = ((int)(pan * 64.0f)) + 64;
    sound.pan = pI;
    AS_SetMP3Pan(pI);

}

//todo REFACTOR THIS TOP BIT TO BE WAV FILES REFER TO https://codeberg.org/SkyLyrac/palib/src/branch/master/source/arm9/as_lib9.c

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
}; // Load music stream from file
Music LoadMusicStreamFromMemory(const char *fileType, const unsigned char *data, int dataSize)
{
    if (strcmp(fileType,"mp3") == 0)
    {
        (Music){dataSize,data,"",1,64,false};
    }
    TRACELOG(LOG_INFO,"MUSIC STREAM CAN ONLY BE LOADED AS MP3");
    return (Music){0,0,0,0};
}; // Load music stream from data
bool IsMusicValid(Music music)
{
    if (music.mData == NULL) return false;//todo check more
    return true;

};
void UnloadMusicStream(Music music)
{
    free(music.mData);
    free(music.filename);

};                            // Unload music stream
void PlayMusicStream(Music music)
{
    AS_MP3DirectPlay(music.mData,music.mSize);
    music.playing = true;
};                              // Start music playing
bool IsMusicStreamPlaying(Music music)
{
    return music.playing;
};                         // Check if music is playing
void UpdateMusicStream(Music music);                            // Updates buffers for music streaming todo see what this does
void StopMusicStream(Music music)
{
    AS_MP3Stop();
};                              // Stop music playing
void PauseMusicStream(Music music)
{
    if (music.playing == true)
    {
        AS_MP3Pause();
    }

};                             // Pause music playing
void ResumeMusicStream(Music music)
{
    if (music.playing == false)
    {
        AS_MP3Unpause();
    }
};                            // Resume playing paused music
void SeekMusicStream(Music music, float position)
{
    //impossible afaik
};              // Seek music to a position (in seconds)
void SetMusicVolume(Music music, float volume)
{
    music.volume = (int)(volume * 127);
    AS_SetMP3Volume(music.volume);
}                 // Set volume for music (1.0 is max level)
void SetMusicPitch(Music music, float pitch)
{
    music.pitch = (int)(32000.0f * pitch);
    AS_SetMP3Rate(music.pitch);
};                   // Set pitch for a music (1.0 is base level)
void SetMusicPan(Music music, float pan)
{
    int pI = ((int)(pan * 64.0f)) + 64;
    music.pan = pI;
    AS_SetMP3Pan(pI);
};                       // Set pan for a music (-1.0 left, 0.0 center, 1.0 right)
float GetMusicTimeLength(Music music){}//todo figure out how to do this};                          // Get music time length (in seconds)
float GetMusicTimePlayed(Music music){}//todo add a timer};

