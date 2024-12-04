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
    
    int screenWidth = 1600;
    int screenHeight = 900;

    SetConfigFlags(FLAG_MSAA_4X_HINT);//| FLAG_FULLSCREEN_MODE);
    InitWindow(screenWidth, screenHeight, "Bouncing Balls with Pause Functionality");

    const int ballCount = 10;
    const int ballRadius = 20;
    const float ballSpeedValue = 4.0f;
    const int coinRadius = 10;

    Vector2 ballPositions[ballCount];
    Vector2 ballSpeeds[ballCount];
    Vector2 coinPositions;
    Color coinColors;
    Color ballColors[ballCount];

    srand(time(NULL));

    // Initialize ball positions, speeds, and colors
    coinPositions.x = GetRandomValue(ballRadius, screenWidth - coinRadius);
    coinPositions.y = GetRandomValue(ballRadius, screenHeight - coinRadius);
    coinColors.r = 255;
    coinColors.g = 215;
    coinColors.b = 1;
    coinColors.a = 255;

    for (int i = 0; i < ballCount; i++)
    {
        ballPositions[i].x = GetRandomValue(ballRadius, screenWidth - ballRadius);
        ballPositions[i].y = GetRandomValue(ballRadius, screenHeight - ballRadius);


        float angle = GetRandomValue(0, 360) * DEG2RAD;
        ballSpeeds[i].x = ballSpeedValue * cos(angle);
        ballSpeeds[i].y = ballSpeedValue * sin(angle);

        // Assign a random unique color to each ball
        ballColors[i].r = GetRandomValue(150, 255);
        ballColors[i].g = GetRandomValue(10, 50);
        ballColors[i].b = GetRandomValue(10, 50);
        ballColors[i].a = 255;
    }

    Vector2 squarePos = { screenWidth / 2, screenHeight / 2 };
    Vector2 squareSize = { 30, 30 };

    float dashCooldown = 0.0f;  // Cooldown timer for the dash
    const float dashDistance = 100.0f; // Distance of the dash
    const float dashCooldownTime = 3.0f; // Dash cooldown time in seconds

    int score = 0;
    bool gameOver = false;
    bool pause = false;
    float survivalTime = 0.0f;
    float speedMultiplier = 1.0f;

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
            speedMultiplier += GetFrameTime() * 0.1f; // Increase speed gradually
            if (dashCooldown > 0.0f)
                dashCooldown -= GetFrameTime();

            for (int i = 0; i < ballCount; i++)
            {
                // Move the balls

                ballPositions[i].x += ballSpeeds[i].x * speedMultiplier;
                ballPositions[i].y += ballSpeeds[i].y * speedMultiplier;

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

                if (MyCheckCollisionCircles(coinPositions, coinRadius, { squarePos.x + squareSize.x / 2, squarePos.y + squareSize.y / 2 }, squareSize.x / 2))
                {
                    score += 1;
                    coinPositions.x = GetRandomValue(ballRadius, screenWidth - coinRadius);
                    coinPositions.y = GetRandomValue(ballRadius, screenHeight - coinRadius);
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

            //dash 
            if (IsKeyPressed(KEY_LEFT_SHIFT) && dashCooldown <= 0.0f)
            {
                if (IsKeyDown(KEY_W) && squarePos.y > 0)
                    squarePos.y -= dashDistance;
                if (IsKeyDown(KEY_S) && squarePos.y + squareSize.y < currentScreenHeight)
                    squarePos.y += dashDistance;
                if (IsKeyDown(KEY_A) && squarePos.x > 0)
                    squarePos.x -= dashDistance;
                if (IsKeyDown(KEY_D) && squarePos.x + squareSize.x < currentScreenWidth)
                    squarePos.x += dashDistance;

                // Ensure the square stays within bounds
                if (squarePos.x < 0)
                    squarePos.x = 0;
                if (squarePos.y < 0)
                    squarePos.y = 0;
                if (squarePos.x + squareSize.x > currentScreenWidth)
                    squarePos.x = currentScreenWidth - squareSize.x;
                if (squarePos.y + squareSize.y > currentScreenHeight)
                    squarePos.y = currentScreenHeight - squareSize.y;

                // Start dash cooldown
                dashCooldown = dashCooldownTime;
            }
        }
        else if (gameOver && IsKeyPressed(KEY_ENTER))
        {
            gameOver = false;
            survivalTime = 0.0f;
            speedMultiplier = 1.0f;
            score = 0;
            dashCooldown = 0.0f;

            //reset coins
            coinPositions.x = GetRandomValue(ballRadius, screenWidth - coinRadius);
            coinPositions.y = GetRandomValue(ballRadius, screenHeight - coinRadius);
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
        ClearBackground(BLACK);

        for (int i = 0; i < ballCount; i++)
            DrawCircleV(ballPositions[i], ballRadius, ballColors[i]);
        
        DrawCircleV(coinPositions, coinRadius, coinColors);
        DrawRectangleV(squarePos, squareSize, BLUE);

        if (gameOver)
        {
            DrawText(TextFormat("Game Over! You survived %.2f seconds", survivalTime), 10, 10, 20, RED);
            DrawText(TextFormat("Your score is : %i.", score), 10, 40, 20, RED);
            DrawText("Press ENTER to restart", 10, 70, 20, DARKGRAY);

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
                DrawText(TextFormat("Score: %i", score), 10, 10, 20, DARKGRAY);
                DrawText(TextFormat("Dash Cooldown: %.1f", dashCooldown > 0.0f ? dashCooldown : 0.0f), 10, 40, 20, DARKGRAY);
                DrawText("Press W/S/A/D to move", 10, 70, 20, LIGHTGRAY);
                DrawText("Press SPACE to pause", 10, 100, 20, LIGHTGRAY);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

