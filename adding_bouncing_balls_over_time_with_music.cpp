/*
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

// Function to detect collision between two circles
bool MyCheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2)
{
    float distance = sqrtf(powf(center2.x - center1.x, 2.0f) + powf(center2.y - center1.y, 2.0f));
    return distance <= (radius1 + radius2);
}

int main(void)
{
    // Initial screen dimensions
    int screenWidth = 800;
    int screenHeight = 450;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Dynamic Ball Colors with Proper Initialization");

    InitAudioDevice();              // Initialize audio device

    Music music = LoadMusicStream("retro-game-music-245230.mp3");
    Sound GameOverSound = LoadSound("mixkit-arcade-space-shooter-dead-notification-272.wav");

    PlayMusicStream(music);

    const int maxBallCount = 100; // Maximum number of balls
    const float initialBallRadius = 20.0f;
    const float initialSquareSize = 30.0f;

    Vector2 ballPositions[maxBallCount];
    Vector2 ballSpeeds[maxBallCount];
    Color ballColors[maxBallCount];

    float ballRadius = initialBallRadius * ((float)screenWidth / 800.0f);
    Vector2 squareSize = { initialSquareSize, initialSquareSize };
    squareSize.x *= ((float)screenWidth / 800.0f);
    squareSize.y *= ((float)screenHeight / 450.0f);

    float ballSpeedValueX = (float)screenWidth * 0.01f;
    float ballSpeedValueY = (float)screenHeight * 0.01f;
    float squareSpeed = (float)screenWidth * 0.01f;

    srand((unsigned int)time(NULL));

    // Initialize the first ball
    int currentBallCount = 1; // Start with one ball
    for (int i = 0; i < maxBallCount; i++)
    {
        ballPositions[i] = {
            (float)GetRandomValue((int)ballRadius, screenWidth - (int)ballRadius),
            (float)GetRandomValue((int)ballRadius, screenHeight - (int)ballRadius)
        };

        float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
        ballSpeeds[i] = { ballSpeedValueX * cosf(angle), ballSpeedValueY * sinf(angle) };

        // Assign a random unique color to each ball
        ballColors[i] = {
            (unsigned char)GetRandomValue(50, 255),
            (unsigned char)GetRandomValue(50, 255),
            (unsigned char)GetRandomValue(50, 255),
            255
        };
    }

    Vector2 squarePos = { (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };

    bool gameOver = false;
    bool pause = false;
    bool gameOverSoundPlayed = false; // Track if the game over sound has been played
    float survivalTime = 0.0f;
    float timeSinceLastBall = 0.0f; // Timer for adding new balls

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        if (!pause && !gameOver)
        {
            survivalTime += GetFrameTime();
            timeSinceLastBall += GetFrameTime();

            // Add a new ball every 4 seconds
            if (timeSinceLastBall >= 4.0f && currentBallCount < maxBallCount)
            {
                timeSinceLastBall = 0.0f;
                int newBallIndex = currentBallCount;
                ballPositions[newBallIndex] = {
                    (float)GetRandomValue((int)ballRadius, screenWidth - (int)ballRadius),
                    (float)GetRandomValue((int)ballRadius, screenHeight - (int)ballRadius)
                };

                float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
                ballSpeeds[newBallIndex] = { ballSpeedValueX * cosf(angle), ballSpeedValueY * sinf(angle) };

                ballColors[newBallIndex] = {
                    (unsigned char)GetRandomValue(50, 255),
                    (unsigned char)GetRandomValue(50, 255),
                    (unsigned char)GetRandomValue(50, 255),
                    255
                };

                currentBallCount++;
            }

            for (int i = 0; i < currentBallCount; i++)
            {
                // Move the balls
                ballPositions[i].x += ballSpeeds[i].x;
                ballPositions[i].y += ballSpeeds[i].y;

                // Handle wall collisions
                if (ballPositions[i].x > screenWidth - ballRadius || ballPositions[i].x < ballRadius)
                    ballSpeeds[i].x *= -1.0f;

                if (ballPositions[i].y > screenHeight - ballRadius || ballPositions[i].y < ballRadius)
                    ballSpeeds[i].y *= -1.0f;

                // Check collision with the square
                if (MyCheckCollisionCircles(ballPositions[i], ballRadius,
                    { squarePos.x + squareSize.x / 2.0f, squarePos.y + squareSize.y / 2.0f },
                    squareSize.x / 2.0f))
                {
                    gameOver = true;
                }
            }

            // Move the square
            if (IsKeyDown(KEY_W) && squarePos.y > 0)
                squarePos.y -= squareSpeed;
            if (IsKeyDown(KEY_S) && squarePos.y + squareSize.y < screenHeight)
                squarePos.y += squareSpeed;
            if (IsKeyDown(KEY_A) && squarePos.x > 0)
                squarePos.x -= squareSpeed;
            if (IsKeyDown(KEY_D) && squarePos.x + squareSize.x < screenWidth)
                squarePos.x += squareSpeed;
        }

        if (gameOver)
        {
            if (!gameOverSoundPlayed) // Play sound only once
            {
                StopMusicStream(music);
                PlaySound(GameOverSound);
                gameOverSoundPlayed = true;
            }

            if (IsKeyPressed(KEY_ENTER)) // Restart the game
            {
                gameOver = false;
                pause = false;
                gameOverSoundPlayed = false; // Reset the sound flag
                survivalTime = 0.0f;
                timeSinceLastBall = 0.0f;
                currentBallCount = 1;

                // Reset balls
                for (int i = 0; i < maxBallCount; i++)
                {
                    ballPositions[i] = {
                        (float)GetRandomValue((int)ballRadius, screenWidth - (int)ballRadius),
                        (float)GetRandomValue((int)ballRadius, screenHeight - (int)ballRadius)
                    };

                    float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
                    ballSpeeds[i] = { ballSpeedValueX * cosf(angle), ballSpeedValueY * sinf(angle) };
                }

                PlayMusicStream(music);
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < currentBallCount; i++)
            DrawCircleV(ballPositions[i], ballRadius, ballColors[i]);

        DrawRectangleV(squarePos, squareSize, BLUE);

        if (gameOver)
        {
            DrawText(TextFormat("Game Over! You survived %.2f seconds", survivalTime), 10, 10, 20, RED);
            DrawText("Press ENTER to restart", 10, 40, 20, DARKGRAY);
        }
        else if (pause)
        {
            DrawText("Game Paused", screenWidth / 2 - 80, screenHeight / 2 - 20, 40, DARKGRAY);
        }
        else
        {
            DrawText(TextFormat("Time: %.2f", survivalTime), screenWidth - 150, 10, 20, DARKGRAY);
            DrawText("Press SPACE to pause", 10, screenHeight - 30, 20, LIGHTGRAY);
        }

        EndDrawing();
    }

    UnloadMusicStream(music);
    UnloadSound(GameOverSound);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
*/
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

// Function to detect collision between two circles
bool MyCheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2)
{
    float distance = sqrtf(powf(center2.x - center1.x, 2.0f) + powf(center2.y - center1.y, 2.0f));
    return distance <= (radius1 + radius2);
}

int main(void)
{
    // Initial screen dimensions
    int screenWidth = 800;
    int screenHeight = 450;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Dynamic Ball Colors with Proper Initialization");

    InitAudioDevice();              // Initialize audio device

    Music music = LoadMusicStream("retro-game-music-245230.mp3");
    Sound GameOverSound = LoadSound("mixkit-arcade-space-shooter-dead-notification-272.wav");

    PlayMusicStream(music);

    const int maxBallCount = 100; // Maximum number of balls
    const float initialBallRadius = 20.0f;
    const float initialSquareSize = 30.0f;

    Vector2 ballPositions[maxBallCount];
    Vector2 ballSpeeds[maxBallCount];
    Color ballColors[maxBallCount];

    float ballRadius = initialBallRadius * ((float)screenWidth / 800.0f);
    Vector2 squareSize = { initialSquareSize, initialSquareSize };
    squareSize.x *= ((float)screenWidth / 800.0f);
    squareSize.y *= ((float)screenHeight / 450.0f);

    float ballSpeedValueX = (float)screenWidth * 0.01f;
    float ballSpeedValueY = (float)screenHeight * 0.01f;
    float squareSpeed = (float)screenWidth * 0.01f;

    srand((unsigned int)time(NULL));

    // Initialize the first ball
    int currentBallCount = 1; // Start with one ball
    for (int i = 0; i < maxBallCount; i++)
    {
        ballPositions[i] = {
            (float)GetRandomValue((int)ballRadius, screenWidth - (int)ballRadius),
            (float)GetRandomValue((int)ballRadius, screenHeight - (int)ballRadius)
        };

        float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
        ballSpeeds[i] = { ballSpeedValueX * cosf(angle), ballSpeedValueY * sinf(angle) };

        // Assign a random unique color to each ball
        ballColors[i] = {
            (unsigned char)GetRandomValue(50, 255),
            (unsigned char)GetRandomValue(50, 255),
            (unsigned char)GetRandomValue(50, 255),
            255
        };
    }

    Vector2 squarePos = { (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };

    bool gameOver = false;
    bool pause = false;
    bool gameOverSoundPlayed = false; // Track if the game over sound has been played
    float survivalTime = 0.0f;
    float timeSinceLastBall = 0.0f; // Timer for adding new balls

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        if (IsKeyPressed(KEY_SPACE) && !gameOver)
        {
            pause = !pause;

            // Pause or resume music depending on the pause state
            if (pause)
            {
                PauseMusicStream(music);
            }
            else
            {
                ResumeMusicStream(music);
            }
        }

        if (!pause && !gameOver)
        {
            survivalTime += GetFrameTime();
            timeSinceLastBall += GetFrameTime();

            // Add a new ball every 4 seconds
            if (timeSinceLastBall >= 4.0f && currentBallCount < maxBallCount)
            {
                timeSinceLastBall = 0.0f;
                int newBallIndex = currentBallCount;
                ballPositions[newBallIndex] = {
                    (float)GetRandomValue((int)ballRadius, screenWidth - (int)ballRadius),
                    (float)GetRandomValue((int)ballRadius, screenHeight - (int)ballRadius)
                };

                float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
                ballSpeeds[newBallIndex] = { ballSpeedValueX * cosf(angle), ballSpeedValueY * sinf(angle) };

                ballColors[newBallIndex] = {
                    (unsigned char)GetRandomValue(50, 255),
                    (unsigned char)GetRandomValue(50, 255),
                    (unsigned char)GetRandomValue(50, 255),
                    255
                };

                currentBallCount++;
            }

            for (int i = 0; i < currentBallCount; i++)
            {
                // Move the balls
                ballPositions[i].x += ballSpeeds[i].x;
                ballPositions[i].y += ballSpeeds[i].y;

                // Handle wall collisions
                if (ballPositions[i].x > screenWidth - ballRadius || ballPositions[i].x < ballRadius)
                    ballSpeeds[i].x *= -1.0f;

                if (ballPositions[i].y > screenHeight - ballRadius || ballPositions[i].y < ballRadius)
                    ballSpeeds[i].y *= -1.0f;

                // Check collision with the square
                if (MyCheckCollisionCircles(ballPositions[i], ballRadius,
                    { squarePos.x + squareSize.x / 2.0f, squarePos.y + squareSize.y / 2.0f },
                    squareSize.x / 2.0f))
                {
                    gameOver = true;
                }
            }

            // Move the square
            if (IsKeyDown(KEY_W) && squarePos.y > 0)
                squarePos.y -= squareSpeed;
            if (IsKeyDown(KEY_S) && squarePos.y + squareSize.y < screenHeight)
                squarePos.y += squareSpeed;
            if (IsKeyDown(KEY_A) && squarePos.x > 0)
                squarePos.x -= squareSpeed;
            if (IsKeyDown(KEY_D) && squarePos.x + squareSize.x < screenWidth)
                squarePos.x += squareSpeed;
        }

        if (gameOver)
        {
            if (!gameOverSoundPlayed) // Play sound only once
            {
                StopMusicStream(music);
                PlaySound(GameOverSound);
                gameOverSoundPlayed = true;
            }

            if (IsKeyPressed(KEY_ENTER)) // Restart the game
            {
                gameOver = false;
                pause = false;
                gameOverSoundPlayed = false; // Reset the sound flag
                survivalTime = 0.0f;
                timeSinceLastBall = 0.0f;
                currentBallCount = 1;

                // Reset balls
                for (int i = 0; i < maxBallCount; i++)
                {
                    ballPositions[i] = {
                        (float)GetRandomValue((int)ballRadius, screenWidth - (int)ballRadius),
                        (float)GetRandomValue((int)ballRadius, screenHeight - (int)ballRadius)
                    };

                    float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
                    ballSpeeds[i] = { ballSpeedValueX * cosf(angle), ballSpeedValueY * sinf(angle) };
                }

                PlayMusicStream(music);
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < currentBallCount; i++)
            DrawCircleV(ballPositions[i], ballRadius, ballColors[i]);

        DrawRectangleV(squarePos, squareSize, BLUE);

        if (gameOver)
        {
            DrawText(TextFormat("Game Over! You survived %.2f seconds", survivalTime), 10, 10, 20, RED);
            DrawText("Press ENTER to restart", 10, 40, 20, DARKGRAY);
        }
        else if (pause)
        {
            DrawText("Game Paused", screenWidth / 2 - 80, screenHeight / 2 - 20, 40, DARKGRAY);
        }
        else
        {
            DrawText(TextFormat("Time: %.2f", survivalTime), screenWidth - 150, 10, 20, DARKGRAY);
            DrawText("Press SPACE to pause", 10, screenHeight - 30, 20, LIGHTGRAY);
        }

        EndDrawing();
    }

    UnloadMusicStream(music);
    UnloadSound(GameOverSound);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
