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
    int pitch; int etc; //todo add these when neccessary
    bool alias; //todo make sure memory management is handled properly for aliases
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
    alias->etc = 0;
    alias->alias = false;
    alias->wave.waveData = NULL;
    alias->wave.waveSize = 0;

}
int main()
{
    PA_Init(); //todo add in initwindow
    InitWindow(256, 192, "");



    if (mp3Data != NULL) AS_MP3DirectPlay(mp3Data, mp3Size);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground((Color){15, 15, 25, 255});
        EndDrawing();
    }

    return 0;
}