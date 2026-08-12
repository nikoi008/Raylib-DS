#include "rcore_ds.h"
#include <nds.h>
#include <stdlib.h>
#include <arm9/as_lib9.h>
#include <arm9/PA_General.h>

void InitAudioDevice(void)
{
    PA_VBLFunctionInit(AS_SoundVBL);
    AS_Init(AS_MODE_MP3);
    AS_SetMasterVolume(127);
}

typedef struct Wave {
    /*unsigned int frameCount;    // Total number of frames (considering channels)
    unsigned int sampleRate;    // Frequency (samples per second)
    unsigned int sampleSize;    // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
    unsigned int channels;      // Number of channels (1-mono, 2-stereo, ...)
    void *data;                 // Buffer data pointer*/
    int waveSize;
    unsigned char* waveData;
    char* filename;
} Wave;


Wave LoadWaveFromMemory(const char *fileType, const unsigned char *fileData, int dataSize) // only supports .mp3 for now todo add suppot for more files
{
    //nt mp3Size = 0;

    return (Wave){dataSize,fileData};
}
Wave LoadWave(const char *fileName)
{
    Wave wave = { 0 };

    // Loading file to memory
    int dataSize = 0;
    unsigned char *fileData = LoadFileData(fileName, &dataSize);

    // Loading wave from memory data
    if (fileData != NULL) wave = LoadWaveFromMemory(GetFileExtension(fileName), fileData, dataSize);
    wave.filename = fileName;

    return wave;
}
bool isWaveValid(Wave wave)
{

    if (wave.waveData != NULL) return true;
    if (wave.waveSize != 0) return true;
    return false;
}

typedef struct Sound
{
    Wave wave;
    int pitch;
    int pan; //todo free
    bool alias; //todo make sure memory management is handled properly for aliases
    bool playing; //todo dont forget to also free
}Sound;


Sound LoadSound(const char *fileName)
{
    Sound s = (Sound){LoadWave(fileName)};
    s.alias = false;
    return s;
};

Sound LoadSoundFromWave(Wave wave)
{
    Sound s = {0};
    s.wave = wave;
    s.alias = false;
    return s;
}

Sound LoadSoundAlias(Sound source) {
    Sound alias = {0};
    alias.wave = source.wave;
    alias.alias = true;
    return alias;
}

bool IsSoundValid(Sound sound)
{
    if (!isWaveValid(sound.wave)) return false;
    //todo check for rest later
    return true;
}

void UpdateSound(Sound sound, const void *data, int sampleCount); //todo implement

void UnloadWave(Wave* wave)
{
    if (wave == NULL) return;

    if (wave->waveData != NULL)
    {
        free(wave->waveData);
        wave->waveData = NULL;
    }

    wave->waveSize = 0;
    wave->filename = NULL;
}

void UnloadSound(Sound* sound)
{
    if (sound == NULL) return;

    if (!sound->alias)
    {
        UnloadWave(&sound->wave);
    }

    sound->pitch = 0;
    //todo add the rest
    sound->alias = false;
}

void UnloadSoundAlias(Sound* alias)
{
    if (alias->alias != true) return;
    if (alias == NULL) return;

    alias->pitch = 0;
    alias->pan = 0;
    alias->alias = false;
    alias->wave.waveData = NULL;
    alias->wave.waveSize = 0;

}

void PlaySound(Sound sound)
{
    AS_MP3DirectPlay(sound.wave.waveData,sound.wave.waveSize);
    sound.playing = true;
}

void StopSound(Sound sound)
{

}

void PauseSound(Sound sound)
{
    if (sound.playing == true)
    {
        AS_MP3Pause();
        sound.playing = false;
    }
}

void ResumeSound(Sound sound)
{
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
float GetMusicTimeLength(Music music){//todo figure out how to do this};                          // Get music time length (in seconds)
float GetMusicTimePlayed(Music music){//todo add a timer};

int main()
{
    PA_Init(); //todo add in initwindow
    InitWindow(256, 192, "");




    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground((Color){15, 15, 25, 255});
        EndDrawing();
    }

    return 0;
}