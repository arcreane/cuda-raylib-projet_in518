/*
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
    // Initial screen dimensions
    int screenWidth = 800;
    int screenHeight = 450;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Dynamic Ball and Object Sizes, Speeds, and Positions");

    const int ballCount = 5;
    const float initialBallRadius = 20.0f;
    const float initialSquareSize = 30.0f;

    Vector2 ballPositions[ballCount];
    Vector2 ballSpeeds[ballCount];
    Color ballColors[ballCount];

    float ballRadius = initialBallRadius * (screenWidth / 800.0f);
    Vector2 squareSize = { initialSquareSize, initialSquareSize };
    squareSize.x *= (screenWidth / 800.0f);
    squareSize.y *= (screenHeight / 450.0f);

    float ballSpeedValueX = screenWidth * 0.01f;
    float ballSpeedValueY = screenHeight * 0.01f;
    float squareSpeed = screenWidth * 0.01f;

    srand(time(NULL));

    // Initialize ball positions, speeds, and colors
    for (int i = 0; i < ballCount; i++)
    {
        ballPositions[i].x = GetRandomValue(ballRadius, screenWidth - ballRadius);
        ballPositions[i].y = GetRandomValue(ballRadius, screenHeight - ballRadius);

        float angle = GetRandomValue(0, 360) * DEG2RAD;
        ballSpeeds[i].x = ballSpeedValueX * cos(angle);
        ballSpeeds[i].y = ballSpeedValueY * sin(angle);

        // Assign a random unique color to each ball
        ballColors[i].r = GetRandomValue(50, 255);
        ballColors[i].g = GetRandomValue(50, 255);
        ballColors[i].b = GetRandomValue(50, 255);
        ballColors[i].a = 255;
    }

    Vector2 squarePos = { screenWidth / 2.0f, screenHeight / 2.0f };

    bool gameOver = false;
    bool pause = false;
    float survivalTime = 0.0f;

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        int currentScreenWidth = GetScreenWidth();
        int currentScreenHeight = GetScreenHeight();

        // Recalculate dimensions, speeds, and positions if the window size changes
        if (currentScreenWidth != screenWidth || currentScreenHeight != screenHeight)
        {
            float widthRatio = (float)currentScreenWidth / screenWidth;
            float heightRatio = (float)currentScreenHeight / screenHeight;

            screenWidth = currentScreenWidth;
            screenHeight = currentScreenHeight;

            ballRadius = initialBallRadius * (screenWidth / 800.0f);
            squareSize.x = initialSquareSize * (screenWidth / 800.0f);
            squareSize.y = initialSquareSize * (screenHeight / 450.0f);

            ballSpeedValueX = screenWidth * 0.01f;
            ballSpeedValueY = screenHeight * 0.01f;
            squareSpeed = screenWidth * 0.01f;

            // Adjust ball positions and speeds based on screen size
            for (int i = 0; i < ballCount; i++)
            {
                ballPositions[i].x *= widthRatio;
                ballPositions[i].y *= heightRatio;

                float angle = atan2(ballSpeeds[i].y, ballSpeeds[i].x);
                ballSpeeds[i].x = ballSpeedValueX * cos(angle);
                ballSpeeds[i].y = ballSpeedValueY * sin(angle);
            }

            // Adjust square position
            squarePos.x *= widthRatio;
            squarePos.y *= heightRatio;
        }

        // Toggle pause on SPACE press
        if (IsKeyPressed(KEY_SPACE))
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
                if (ballPositions[i].x > screenWidth - ballRadius || ballPositions[i].x < ballRadius)
                    ballSpeeds[i].x *= -1.0f;

                if (ballPositions[i].y > screenHeight - ballRadius || ballPositions[i].y < ballRadius)
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
            survivalTime = 0.0f;

            // Reset balls
            for (int i = 0; i < ballCount; i++)
            {
                ballPositions[i].x = GetRandomValue(ballRadius, screenWidth - ballRadius);
                ballPositions[i].y = GetRandomValue(ballRadius, screenHeight - ballRadius);

                float angle = GetRandomValue(0, 360) * DEG2RAD;
                ballSpeeds[i].x = ballSpeedValueX * cos(angle);
                ballSpeeds[i].y = ballSpeedValueY * sin(angle);
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
                DrawText("Paused", screenWidth / 2 - MeasureText("Paused", 40) / 2, screenHeight / 2 - 20, 40, DARKGRAY);
            }
            else
            {
                DrawText(TextFormat("Time: %.2f", survivalTime), screenWidth - 150, 10, 20, DARKGRAY);
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
    float distance = sqrt(pow(center2.x - center1.x, 2) + pow(center2.y - center1.y, 2));
    return distance <= (radius1 + radius2);
}

int main(void)
{
    // Initial screen dimensions
    int screenWidth = 800;
    int screenHeight = 450;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Dynamic Ball and Object Sizes, Speeds, and Positions");

    const int ballCount = 5;
    const float initialBallRadius = 20.0f;
    const float initialSquareSize = 30.0f;

    Vector2 ballPositions[ballCount];
    Vector2 ballSpeeds[ballCount];
    Color ballColors[ballCount];

    float ballRadius = initialBallRadius * (screenWidth / 800.0f);
    Vector2 squareSize = { initialSquareSize, initialSquareSize };
    squareSize.x *= (screenWidth / 800.0f);
    squareSize.y *= (screenHeight / 450.0f);

    float ballSpeedValueX = screenWidth * 0.01f;
    float ballSpeedValueY = screenHeight * 0.01f;
    float squareSpeed = screenWidth * 0.01f;

    srand(time(NULL));

    // Initialize ball positions, speeds, and colors
    for (int i = 0; i < ballCount; i++)
    {
        ballPositions[i].x = GetRandomValue(ballRadius, screenWidth - ballRadius);
        ballPositions[i].y = GetRandomValue(ballRadius, screenHeight - ballRadius);

        float angle = GetRandomValue(0, 360) * DEG2RAD;
        ballSpeeds[i].x = ballSpeedValueX * cos(angle);
        ballSpeeds[i].y = ballSpeedValueY * sin(angle);

        // Assign a random unique color to each ball
        ballColors[i].r = GetRandomValue(50, 255);
        ballColors[i].g = GetRandomValue(50, 255);
        ballColors[i].b = GetRandomValue(50, 255);
        ballColors[i].a = 255;
    }

    Vector2 squarePos = { screenWidth / 2.0f, screenHeight / 2.0f };

    bool gameOver = false;
    bool pause = false;
    float survivalTime = 0.0f;

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        int currentScreenWidth = GetScreenWidth();
        int currentScreenHeight = GetScreenHeight();

        // Recalculate dimensions, speeds, and positions if the window size changes
        if (currentScreenWidth != screenWidth || currentScreenHeight != screenHeight)
        {
            float widthRatio = (float)currentScreenWidth / screenWidth;
            float heightRatio = (float)currentScreenHeight / screenHeight;

            screenWidth = currentScreenWidth;
            screenHeight = currentScreenHeight;

            ballRadius = initialBallRadius * (screenWidth / 800.0f);
            squareSize.x = initialSquareSize * (screenWidth / 800.0f);
            squareSize.y = initialSquareSize * (screenHeight / 450.0f);

            ballSpeedValueX = screenWidth * 0.01f;
            ballSpeedValueY = screenHeight * 0.01f;
            squareSpeed = screenWidth * 0.01f;

            // Adjust ball positions and speeds based on screen size
            for (int i = 0; i < ballCount; i++)
            {
                ballPositions[i].x *= widthRatio;
                ballPositions[i].y *= heightRatio;

                float angle = atan2(ballSpeeds[i].y, ballSpeeds[i].x);
                ballSpeeds[i].x = ballSpeedValueX * cos(angle);
                ballSpeeds[i].y = ballSpeedValueY * sin(angle);
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

            for (int i = 0; i < ballCount; i++)
            {
                // Move the balls
                ballPositions[i].x += ballSpeeds[i].x;
                ballPositions[i].y += ballSpeeds[i].y;

                // Handle wall collisions
                if (ballPositions[i].x > screenWidth - ballRadius || ballPositions[i].x < ballRadius)
                    ballSpeeds[i].x *= -1.0f;

                if (ballPositions[i].y > screenHeight - ballRadius || ballPositions[i].y < ballRadius)
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
                if (MyCheckCollisionCircles(ballPositions[i], ballRadius,  { squarePos.x + squareSize.x / 2, squarePos.y + squareSize.y / 2 }, squareSize.x / 2))
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
            pause = false;  // Ensure the game is not paused after restarting
            survivalTime = 0.0f;

            // Reset balls
            for (int i = 0; i < ballCount; i++)
            {
                ballPositions[i].x = GetRandomValue(ballRadius, screenWidth - ballRadius);
                ballPositions[i].y = GetRandomValue(ballRadius, screenHeight - ballRadius);

                float angle = GetRandomValue(0, 360) * DEG2RAD;
                ballSpeeds[i].x = ballSpeedValueX * cos(angle);
                ballSpeeds[i].y = ballSpeedValueY * sin(angle);
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
                DrawText("Game Paused", screenWidth / 2 - 80, screenHeight / 2 - 20, 40, DARKGRAY);
            }
            else
            {
                DrawText(TextFormat("Time: %.2f", survivalTime), screenWidth - 150, 10, 20, DARKGRAY);
            }
            DrawText("Press SPACE to pause", 10, screenHeight - 30, 20, LIGHTGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
