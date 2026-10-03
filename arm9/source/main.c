
#include <nds.h>
#include <gl2d.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rcore_ds.h"
#include "raudio_ds.h"

#define PADDLE_SPEED 3

#define BASE_SPEED 600
#define INCREMENT 4
#define MAX_SPEED 700


#define BRICK_H 16
#define BRICK_COLS 8
#define BRICK_ROWS 5
#define BRICK_TOP 24

#define FP_SHIFT 8
#define FP(n) ((n) << FP_SHIFT)
#define UNFP(n) ((n) >> FP_SHIFT)

typedef enum { ST_READY, ST_PLAY, ST_PAUSED, ST_LOST, ST_WON } GameState;

GameState state;
bool alive[BRICK_ROWS][BRICK_COLS];
int bricksLeft;
int paddleX;
int ballX,ballY;
int ballVX,ballVY;
int lives;

Texture2D paddleTex,pumpkinTex,ballTex;
Sound hitSnd;

int iabs(int v) { return v < 0 ? -v : v; }

int currentSpeed(void)
{
    int broken = BRICK_ROWS * BRICK_COLS - bricksLeft;
    int s = BASE_SPEED + broken * INCREMENT;
    return s > MAX_SPEED ? MAX_SPEED : s;
}

void resetBallOnPaddle(void)
{
    ballX = FP(paddleX + (32 - 8) / 2);
    ballY = FP(168 - 8);
    ballVX = 0;
    ballVY = 0;
    state = ST_READY;
}

void resetGame(void)
{
    for (int r = 0; r < BRICK_ROWS; r++)
        for (int c = 0; c < BRICK_COLS; c++)
            alive[r][c] = true;
    bricksLeft = BRICK_ROWS * BRICK_COLS;
    lives = 3;
    paddleX = (256 - 32) / 2;
    resetBallOnPaddle();
}

void launchBall(void)
{
    int s = currentSpeed();
    ballVX = (GetRandomValue(0,1) ? 1 : -1) * (s / 3);
    ballVY = -(s - iabs(ballVX) / 3);
    state = ST_PLAY;
}

bool hitBrick(int px, int py)
{
    int c0 = px / 32,c1 = (px + 8 - 1) / 32;
    int r0 = (py - BRICK_TOP) / BRICK_H,r1 = (py + 8 - 1 - BRICK_TOP) / BRICK_H;
    if (py - BRICK_TOP < 0) r0 = (py - BRICK_TOP - BRICK_H + 1) / BRICK_H;

    for (int r = r0; r <= r1; r++)
    {
        for (int c = c0; c <= c1; c++)
        {
            if (r < 0 || r >= BRICK_ROWS || c < 0 || c >= BRICK_COLS) continue;
            if (!alive[r][c]) continue;
            alive[r][c] = false;
            bricksLeft--;
            PlaySound(hitSnd);
            return true;
        }
    }
    return false;
}

void updatePaddle(void)
{
    if (IsKeyDown(KEY_LEFT)) paddleX -= PADDLE_SPEED;
    if (IsKeyDown(KEY_RIGHT)) paddleX += PADDLE_SPEED;
    if (paddleX < 0) paddleX = 0;
    if (paddleX > 256 - 32) paddleX = 256 - 32;
}

void updateBall(void)
{
    int nx = ballX + ballVX;
    int px = UNFP(nx), py = UNFP(ballY);

    if (px < 0) { nx = 0; ballVX = iabs(ballVX); }
    else if (px > 256 - 8){ nx = FP(256 - 8); ballVX = -iabs(ballVX); }
    else if (hitBrick(px, py)) { ballVX = -ballVX; nx = ballX; }
    ballX = nx;

    int ny = ballY + ballVY;
    px = UNFP(ballX); py = UNFP(ny);

    if (py < 0)
    {
        ny = 0;
        ballVY = iabs(ballVY);
    }
    else if (hitBrick(px, py))
    {
        ballVY = -ballVY;
        ny = ballY;
    }
    else if (ballVY > 0 && py + 8 >= 168 && py + 8 <= 168 + 16 &&px + 8 > paddleX && px < paddleX + 32)
    {
        int off = (px + 8 / 2) - (paddleX + 32 / 2);
        int s = currentSpeed();
        ballVX = off * s / 24;
        ballVY = - (s - iabs(ballVX) / 3);
        if (ballVY > -200) ballVY = -200;
        ny = FP(168 - 8);
    }
    ballY = ny;

    if (UNFP(ballY) > 192)
    {
        lives--;
        if (lives <= 0) { lives = 0; state = ST_LOST; }
        else resetBallOnPaddle();
    }

    if (bricksLeft == 0) state = ST_WON;
}

void update(void)
{
    bool a = IsKeyPressed(KEY_A);

    switch (state)
    {
        case ST_READY:
            updatePaddle();
            ballX = FP(paddleX + (32 - 8) / 2);
            if (a) launchBall();
            break;

        case ST_PLAY:
            if (a) { state = ST_PAUSED; break; }
            updatePaddle();
            updateBall();
            break;

        case ST_PAUSED:
            if (a) state = ST_PLAY;
            break;

        case ST_LOST:
        case ST_WON:
            if (a) resetGame();
            break;
    }
}

void draw(void)
{

    for (int r = 0; r < BRICK_ROWS; r++)
        for (int c = 0; c < BRICK_COLS; c++)
            if (alive[r][c]) DrawTexture(pumpkinTex,c * 32,BRICK_TOP + r * BRICK_H,WHITE);

    DrawTexture(paddleTex,paddleX,168,WHITE);
    DrawTexture(ballTex,UNFP(ballX),UNFP(ballY),WHITE);

    for (int i = 0; i < lives; i++)
        DrawTexture(ballTex,4 + i * 12,6,WHITE);

    if (state == ST_READY) DrawText("PRESS A TO START",80,120,0.7f,WHITE);
    if (state == ST_PAUSED) DrawText("PAUSED",108,120,0.7f,WHITE);
    if (state == ST_LOST) DrawText("GAME OVER. PRESS A TO RETRY",20,120,0.7f,RED);
    if (state == ST_WON) DrawText("YOU WIN. PRESS A TO PLAY AGAIN",48,120,0.7f,YELLOW);
}

int main(void)
{
    InitWindow(256, 192, "spooks");
    InitAudioDevice();
    SetMasterVolume(1.0f);
    SetRandomSeed((unsigned int)time(NULL));

    paddleTex  = LoadTexture("nitro:/paddle.png");
    pumpkinTex = LoadTexture("nitro:/pumpkin.png");
    ballTex = LoadTexture("nitro:/ball.png");
    hitSnd = LoadSound("nitro:/hit.wav");

    printf("\x1b[2J");
    printf("\n  LEFT/RIGHT: move paddle\n  A: start/pause/retry\n");
    printf("\x1b[31;1m\n DONT FORGET TO SET KEYBINDS IN CONFIG->INPUT AND HOTKEYS OTHERWISE THIS GAME WILL NOT WORK\x1b[39;0m");
    resetGame();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        update();
        draw();
        EndDrawing();
    }

    return 0;
}