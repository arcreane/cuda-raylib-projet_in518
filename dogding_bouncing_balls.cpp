#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

// Function to detect collision between two circles
bool MyCheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2)
{
    float distance = sqrt(pow(center2.x - center1.x, 2) + pow(center2.y - center1.y, 2));
    return distance <= (radius1 + radius2);
}

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Bouncing Balls with Pause Functionality");

    const int ballCount = 5;
    const int ballRadius = 20;
    const float ballSpeedValue = 4.0f;

    Vector2 ballPositions[ballCount];
    Vector2 ballSpeeds[ballCount];
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

        // Assign a random unique color to each ball
        ballColors[i].r = GetRandomValue(50, 255);
        ballColors[i].g = GetRandomValue(50, 255);
        ballColors[i].b = GetRandomValue(50, 255);
        ballColors[i].a = 255;
    }

    Vector2 squarePos = { screenWidth / 2.0f, screenHeight / 2.0f };
    Vector2 squareSize = { 30, 30 };

    bool gameOver = false;
    bool pause = false;
    float survivalTime = 0.0f;

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        int currentScreenWidth = GetScreenWidth();
        int currentScreenHeight = GetScreenHeight();

        // Toggle pause on SPACE press, but only if the game is not over
        if (IsKeyPressed(KEY_SPACE) && !gameOver)
        {
            pause = !pause;
        }

        if (!pause && !gameOver)
        {
            survivalTime += GetFrameTime();

            for (int i = 0; i < ballCount; i++)
            {
                // Move the balls
                ballPositions[i].x += ballSpeeds[i].x;
                ballPositions[i].y += ballSpeeds[i].y;

                // Handle wall collisions
                if (ballPositions[i].x > currentScreenWidth - ballRadius || ballPositions[i].x < ballRadius)
                    ballSpeeds[i].x *= -1.0f;

                if (ballPositions[i].y > currentScreenHeight - ballRadius || ballPositions[i].y < ballRadius)
                    ballSpeeds[i].y *= -1.0f;

                // Handle ball-to-ball collisions
                for (int j = i + 1; j < ballCount; j++)
                {
                    if (MyCheckCollisionCircles(ballPositions[i], ballRadius, ballPositions[j], ballRadius))
                    {
                        // Calculate collision normal
                        Vector2 collisionNormal = {
                            ballPositions[j].x - ballPositions[i].x,
                            ballPositions[j].y - ballPositions[i].y
                        };

                        float distance = sqrt(collisionNormal.x * collisionNormal.x + collisionNormal.y * collisionNormal.y);
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

                        ballPositions[i].x -= correction.x;
                        ballPositions[i].y -= correction.y;
                        ballPositions[j].x += correction.x;
                        ballPositions[j].y += correction.y;
                    }
                }

                // Check collision with the square
                if (MyCheckCollisionCircles(ballPositions[i], ballRadius, { squarePos.x + squareSize.x / 2, squarePos.y + squareSize.y / 2 }, squareSize.x / 2))
                {
                    gameOver = true;
                }
            }

            // Move the square
            if (IsKeyDown(KEY_W) && squarePos.y > 0)
                squarePos.y -= 5;
            if (IsKeyDown(KEY_S) && squarePos.y + squareSize.y < currentScreenHeight)
                squarePos.y += 5;
            if (IsKeyDown(KEY_A) && squarePos.x > 0)
                squarePos.x -= 5;
            if (IsKeyDown(KEY_D) && squarePos.x + squareSize.x < currentScreenWidth)
                squarePos.x += 5;
        }
        else if (gameOver && IsKeyPressed(KEY_ENTER))
        {
            gameOver = false;
            survivalTime = 0.0f;

            // Reset balls
            for (int i = 0; i < ballCount; i++)
            {
                ballPositions[i].x = GetRandomValue(ballRadius, currentScreenWidth - ballRadius);
                ballPositions[i].y = GetRandomValue(ballRadius, currentScreenHeight - ballRadius);

                float angle = GetRandomValue(0, 360) * DEG2RAD;
                ballSpeeds[i].x = ballSpeedValue * cos(angle);
                ballSpeeds[i].y = ballSpeedValue * sin(angle);
            }
        }

        // Draw
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
            if (pause)
            {
                DrawText("Paused", currentScreenWidth / 2 - MeasureText("Paused", 40) / 2, currentScreenHeight / 2 - 20, 40, GRAY);
                DrawText("Press SPACE to resume", currentScreenWidth / 2 - MeasureText("Press SPACE to resume", 20) / 2, currentScreenHeight / 2 + 20, 20, DARKGRAY);
            }
            else
            {
                DrawText(TextFormat("Time: %.2f", survivalTime), 10, 10, 20, DARKGRAY);
                DrawText("Press W/S/A/D to move", 10, 40, 20, LIGHTGRAY);
                DrawText("Press SPACE to pause", 10, 70, 20, LIGHTGRAY);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
