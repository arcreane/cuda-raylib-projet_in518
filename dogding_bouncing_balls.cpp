#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

// Function to detect collision between two circles
bool MyCheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2)
{
    float distance = sqrt(pow(center2.x - center1.x, 2) + pow(center2.y - center1.y, 2));
    return distance <= (radius1 + radius2);
}

// Function to normalize a vector and scale it to a given magnitude
Vector2 NormalizeVector(Vector2 vector, float magnitude)
{
    float length = sqrt(vector.x * vector.x + vector.y * vector.y);
    if (length == 0) return  { 0, 0 };
    return  { (vector.x / length)* magnitude, (vector.y / length)* magnitude };
}

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Bouncing Balls with Collisions");

    const int ballCount = 5;
    const int ballRadius = 20;
    const float ballSpeedValue = 4.0f;

    Vector2 ballPositions[ballCount];
    Vector2 ballSpeeds[ballCount];
    float ballRelativeX[ballCount];
    float ballRelativeY[ballCount];
    Color ballColors[ballCount];

    srand(time(NULL));

    // Initialize ball positions, speeds, and colors
    for (int i = 0; i < ballCount; i++)
    {
        ballPositions[i].x = GetRandomValue(ballRadius, screenWidth - ballRadius);
        ballPositions[i].y = GetRandomValue(ballRadius, screenHeight - ballRadius);

        float angle = GetRandomValue(0, 360) * DEG2RAD;
        ballSpeeds[i].x = ballSpeedValue * cos(angle);
        ballSpeeds[i].y = ballSpeedValue * sin(angle);

        ballRelativeX[i] = ballPositions[i].x / screenWidth;
        ballRelativeY[i] = ballPositions[i].y / screenHeight;

        ballColors[i].r = GetRandomValue(50, 255);
        ballColors[i].g = GetRandomValue(50, 255);
        ballColors[i].b = GetRandomValue(50, 255);
        ballColors[i].a = 255;
    }

    bool pause = false;
    bool gameOver = false;
    float survivalTime = 0.0f;

    char survivalText[50];
    int framesCounter = 0;

    Vector2 squarePos = { screenWidth / 2.0f, screenHeight / 2.0f };
    Vector2 squareSize = { 30, 30 };

    int lastScreenWidth = screenWidth;
    int lastScreenHeight = screenHeight;

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        int currentScreenWidth = GetScreenWidth();
        int currentScreenHeight = GetScreenHeight();

        if (currentScreenWidth != lastScreenWidth || currentScreenHeight != lastScreenHeight)
        {
            for (int i = 0; i < ballCount; i++)
            {
                ballPositions[i].x = ballRelativeX[i] * currentScreenWidth;
                ballPositions[i].y = ballRelativeY[i] * currentScreenHeight;
            }

            squarePos.x = (squarePos.x / lastScreenWidth) * currentScreenWidth;
            squarePos.y = (squarePos.y / lastScreenHeight) * currentScreenHeight;

            lastScreenWidth = currentScreenWidth;
            lastScreenHeight = currentScreenHeight;
        }

        if (!gameOver && !pause)
        {
            survivalTime += GetFrameTime();

            for (int i = 0; i < ballCount; i++)
            {
                ballPositions[i].x += ballSpeeds[i].x;
                ballPositions[i].y += ballSpeeds[i].y;

                ballRelativeX[i] = ballPositions[i].x / currentScreenWidth;
                ballRelativeY[i] = ballPositions[i].y / currentScreenHeight;

                if (ballPositions[i].x > currentScreenWidth - ballRadius || ballPositions[i].x < ballRadius)
                    ballSpeeds[i].x *= -1.0f;

                if (ballPositions[i].y > currentScreenHeight - ballRadius || ballPositions[i].y < ballRadius)
                    ballSpeeds[i].y *= -1.0f;

                // Ball-to-ball collisions
                for (int j = i + 1; j < ballCount; j++)
                {
                    if (MyCheckCollisionCircles(ballPositions[i], ballRadius, ballPositions[j], ballRadius))
                    {
                        // Swap velocities for a simple collision response
                        Vector2 tempSpeed = ballSpeeds[i];
                        ballSpeeds[i] = ballSpeeds[j];
                        ballSpeeds[j] = tempSpeed;

                        // Normalize to maintain constant speed
                        ballSpeeds[i] = NormalizeVector(ballSpeeds[i], ballSpeedValue);
                        ballSpeeds[j] = NormalizeVector(ballSpeeds[j], ballSpeedValue);

                        // Resolve overlap
                        Vector2 collisionNormal = {
                            ballPositions[j].x - ballPositions[i].x,
                            ballPositions[j].y - ballPositions[i].y
                        };

                        float distance = sqrt(collisionNormal.x * collisionNormal.x + collisionNormal.y * collisionNormal.y);
                        float overlap = ballRadius * 2 - distance;
                        Vector2 correction = {
                            (overlap / 2) * (collisionNormal.x / distance),
                            (overlap / 2) * (collisionNormal.y / distance)
                        };

                        ballPositions[i].x -= correction.x;
                        ballPositions[i].y -= correction.y;
                        ballPositions[j].x += correction.x;
                        ballPositions[j].y += correction.y;
                    }
                }
            }

            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_S) || IsKeyDown(KEY_A) || IsKeyDown(KEY_D))
            {
                if (IsKeyDown(KEY_W) && squarePos.y > 0)
                    squarePos.y -= 5;
                if (IsKeyDown(KEY_S) && squarePos.y + squareSize.y < currentScreenHeight)
                    squarePos.y += 5;
                if (IsKeyDown(KEY_A) && squarePos.x > 0)
                    squarePos.x -= 5;
                if (IsKeyDown(KEY_D) && squarePos.x + squareSize.x < currentScreenWidth)
                    squarePos.x += 5;
            }

            for (int i = 0; i < ballCount; i++)
            {
                if (MyCheckCollisionCircles(ballPositions[i], ballRadius,  { squarePos.x + squareSize.x / 2, squarePos.y + squareSize.y / 2 }, squareSize.x / 2))
                {
                    gameOver = true;
                }
            }
        }
        else if (gameOver && IsKeyPressed(KEY_ENTER))
        {
            gameOver = false;
            survivalTime = 0.0f;

            for (int i = 0; i < ballCount; i++)
            {
                ballPositions[i].x = GetRandomValue(ballRadius, currentScreenWidth - ballRadius);
                ballPositions[i].y = GetRandomValue(ballRadius, currentScreenHeight - ballRadius);

                float angle = GetRandomValue(0, 360) * DEG2RAD;
                ballSpeeds[i].x = ballSpeedValue * cos(angle);
                ballSpeeds[i].y = ballSpeedValue * sin(angle);
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < ballCount; i++)
            DrawCircleV(ballPositions[i], ballRadius, ballColors[i]);

        DrawRectangleV(squarePos, squareSize, BLUE);

        if (gameOver)
        {
            DrawText(TextFormat("Game Over! You survived %.2f seconds", survivalTime), 10, 10, 20, RED);
            DrawText("Press ENTER to restart", 10, 40, 20, DARKGRAY);
        }
        else
        {
            DrawText("Press SPACE to pause", 10, currentScreenHeight - 30, 20, LIGHTGRAY);
            DrawText(TextFormat("Time: %.2f", survivalTime), currentScreenWidth - 150, 10, 20, DARKGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
