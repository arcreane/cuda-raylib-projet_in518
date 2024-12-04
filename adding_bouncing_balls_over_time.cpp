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
    float survivalTime = 0.0f;
    float timeSinceLastBall = 0.0f; // Timer for adding new balls

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        int currentScreenWidth = GetScreenWidth();
        int currentScreenHeight = GetScreenHeight();

        // Recalculate dimensions, speeds, and positions if the window size changes
        if (currentScreenWidth != screenWidth || currentScreenHeight != screenHeight)
        {
            float widthRatio = (float)currentScreenWidth / (float)screenWidth;
            float heightRatio = (float)currentScreenHeight / (float)screenHeight;

            screenWidth = currentScreenWidth;
            screenHeight = currentScreenHeight;

            ballRadius = initialBallRadius * ((float)screenWidth / 800.0f);
            squareSize.x = initialSquareSize * ((float)screenWidth / 800.0f);
            squareSize.y = initialSquareSize * ((float)screenHeight / 450.0f);

            ballSpeedValueX = (float)screenWidth * 0.01f;
            ballSpeedValueY = (float)screenHeight * 0.01f;
            squareSpeed = (float)screenWidth * 0.01f;

            // Adjust ball positions and speeds based on screen size
            for (int i = 0; i < currentBallCount; i++)
            {
                ballPositions[i].x *= widthRatio;
                ballPositions[i].y *= heightRatio;

                float angle = atan2f(ballSpeeds[i].y, ballSpeeds[i].x);
                ballSpeeds[i].x = ballSpeedValueX * cosf(angle);
                ballSpeeds[i].y = ballSpeedValueY * sinf(angle);
            }

            // Adjust square position
            squarePos.x *= widthRatio;
            squarePos.y *= heightRatio;
        }

        // Toggle pause on SPACE press, but only if not in the "game over" state
        if (IsKeyPressed(KEY_SPACE) && !gameOver)
        {
            pause = !pause;
        }

        if (!pause && !gameOver)
        {
            survivalTime += GetFrameTime();
            timeSinceLastBall += GetFrameTime();

            // Add a new ball every 5 seconds
            if (timeSinceLastBall >= 5.0f && currentBallCount < maxBallCount)
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
        else if (gameOver && IsKeyPressed(KEY_ENTER))
        {
            gameOver = false;
            pause = false;
            survivalTime = 0.0f;
            timeSinceLastBall = 0.0f;
            currentBallCount = 1; // Reset to one ball

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
        else
        {
            if (pause)
            {
                DrawText("Game Paused", screenWidth / 2 - 80, screenHeight / 2 - 20, 40, DARKGRAY);
            }
            else
            {
                DrawText(TextFormat("Time: %.2f", survivalTime), screenWidth - 150, 10, 20, DARKGRAY);
                DrawText("Press SPACE to pause", 10, screenHeight - 30, 20, LIGHTGRAY);
            }
        }

        EndDrawing();
    }

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
    float survivalTime = 0.0f;
    float timeSinceLastBall = 0.0f; // Timer for adding new balls

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        int currentScreenWidth = GetScreenWidth();
        int currentScreenHeight = GetScreenHeight();

        // Recalculate dimensions, speeds, and positions if the window size changes
        if (currentScreenWidth != screenWidth || currentScreenHeight != screenHeight)
        {
            float widthRatio = (float)currentScreenWidth / (float)screenWidth;
            float heightRatio = (float)currentScreenHeight / (float)screenHeight;

            screenWidth = currentScreenWidth;
            screenHeight = currentScreenHeight;

            ballRadius = initialBallRadius * ((float)screenWidth / 800.0f);
            squareSize.x = initialSquareSize * ((float)screenWidth / 800.0f);
            squareSize.y = initialSquareSize * ((float)screenHeight / 450.0f);

            ballSpeedValueX = (float)screenWidth * 0.01f;
            ballSpeedValueY = (float)screenHeight * 0.01f;
            squareSpeed = (float)screenWidth * 0.01f;

            // Adjust ball positions and speeds based on screen size
            for (int i = 0; i < currentBallCount; i++)
            {
                ballPositions[i].x *= widthRatio;
                ballPositions[i].y *= heightRatio;

                float angle = atan2f(ballSpeeds[i].y, ballSpeeds[i].x);
                ballSpeeds[i].x = ballSpeedValueX * cosf(angle);
                ballSpeeds[i].y = ballSpeedValueY * sinf(angle);
            }

            // Adjust square position
            squarePos.x *= widthRatio;
            squarePos.y *= heightRatio;
        }

        // Toggle pause on SPACE press, but only if not in the "game over" state
        if (IsKeyPressed(KEY_SPACE) && !gameOver)
        {
            pause = !pause;
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

                // Handle ball-to-ball collisions
                for (int j = i + 1; j < currentBallCount; j++)
                {
                    if (MyCheckCollisionCircles(ballPositions[i], ballRadius, ballPositions[j], ballRadius))
                    {
                        // Calculate collision normal
                        Vector2 collisionNormal = {
                            ballPositions[j].x - ballPositions[i].x,
                            ballPositions[j].y - ballPositions[i].y
                        };

                        float distance = sqrtf(collisionNormal.x * collisionNormal.x + collisionNormal.y * collisionNormal.y);
                        if (distance == 0) continue;

                        // Normalize the collision normal
                        collisionNormal.x /= distance;
                        collisionNormal.y /= distance;

                        // Reflect ball i's velocity
                        float dotProduct1 = ballSpeeds[i].x * collisionNormal.x + ballSpeeds[i].y * collisionNormal.y;
                        ballSpeeds[i].x -= 2 * dotProduct1 * collisionNormal.x;
                        ballSpeeds[i].y -= 2 * dotProduct1 * collisionNormal.y;

                        // Reflect ball j's velocity
                        float dotProduct2 = ballSpeeds[j].x * collisionNormal.x + ballSpeeds[j].y * collisionNormal.y;
                        ballSpeeds[j].x -= 2 * dotProduct2 * collisionNormal.x;
                        ballSpeeds[j].y -= 2 * dotProduct2 * collisionNormal.y;

                        // Correct positions to resolve overlap
                        float overlap = ballRadius * 2 - distance;
                        Vector2 correction = {
                            overlap * collisionNormal.x / 2,
                            overlap * collisionNormal.y / 2
                        };

                        // Apply the position correction
                        ballPositions[i].x -= correction.x;
                        ballPositions[i].y -= correction.y;
                        ballPositions[j].x += correction.x;
                        ballPositions[j].y += correction.y;
                    }
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
        else if (gameOver && IsKeyPressed(KEY_ENTER))
        {
            gameOver = false;
            pause = false;
            survivalTime = 0.0f;
            timeSinceLastBall = 0.0f;
            currentBallCount = 1; // Reset to one ball

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
        else
        {
            if (pause)
            {
                DrawText("Game Paused", screenWidth / 2 - 80, screenHeight / 2 - 20, 40, DARKGRAY);
            }
            else
            {
                DrawText(TextFormat("Time: %.2f", survivalTime), screenWidth - 150, 10, 20, DARKGRAY);
                DrawText("Press SPACE to pause", 10, screenHeight - 30, 20, LIGHTGRAY);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
