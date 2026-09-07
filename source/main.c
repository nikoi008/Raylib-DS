#include "raudio_ds.h"
#include "rcore_ds.h"

#define BLACK (Color){0,0,0}
int main()
{
    InitWindow(256,192,"w");
    InitAudioDevice();
    Sound S = LoadSound("piano.wav");
    PlaySound(&S);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        if (IsKeyReleased(KEY_A))
        {
            PauseSound(S);
        }
        if (IsKeyReleased(KEY_B))
        {
            ResumeSound(S);
        }
        static float volume = 0.5f;

        if (IsKeyPressed(KEY_UP))
        {

            volume += 0.1f;
            SetSoundVolume(S,volume);

        }

        if (IsKeyPressed(KEY_DOWN))
        {
            volume -= 0.1f;
            SetSoundVolume(S,volume);

        }
        TRACELOG(LOG_ALL,"channel %d",S.id);
        EndDrawing();


    }

    
}
