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

// Function to calculate the direction vector from a point towards the mouse position
Vector2 GetDirectionToMouse(Vector2 ballPosition, Vector2 mousePosition)
{
    Vector2 direction = { mousePosition.x - ballPosition.x, mousePosition.y - ballPosition.y };
    float length = sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length != 0) {
        direction.x /= length; // Normalize the direction vector
        direction.y /= length;
    }
    return direction;
}

// Function to move balls away from each other if they overlap
void PreventBallOverlap(Vector2* ballPosition1, Vector2* ballPosition2, float radius)
{
    Vector2 delta = { ballPosition2->x - ballPosition1->x, ballPosition2->y - ballPosition1->y };
    float distance = sqrt(delta.x * delta.x + delta.y * delta.y);
    if (distance < 2 * radius) {
        // Push balls apart by a small amount to prevent overlap
        float overlap = 2 * radius - distance;
        float pushFactor = overlap / distance;
        delta.x *= pushFactor;
        delta.y *= pushFactor;

        ballPosition1->x -= delta.x / 2;
        ballPosition1->y -= delta.y / 2;
        ballPosition2->x += delta.x / 2;
        ballPosition2->y += delta.y / 2;
    }
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    // Enable the resizable window flag
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - multiple bouncing balls with unique colors and attractive mouse");

    const int ballCount = 5; // Number of balls
    const int ballRadius = 20; // Radius of all balls
    const float ballSpeedValue = 4.0f; // Speed magnitude for all balls

    Vector2 ballPositions[ballCount];
    Vector2 ballSpeeds[ballCount];
    float ballRelativeX[ballCount];
    float ballRelativeY[ballCount];
    Color ballColors[ballCount]; // Array to store unique colors for each ball

    srand(time(NULL)); // Seed for random numbers

    // Initialize ball positions, speeds, and colors
    for (int i = 0; i < ballCount; i++)
    {
        ballPositions[i].x = GetRandomValue(ballRadius, screenWidth - ballRadius);
        ballPositions[i].y = GetRandomValue(ballRadius, screenHeight - ballRadius);

        // Assign random directions
        float angle = GetRandomValue(0, 360) * DEG2RAD;
        ballSpeeds[i].x = ballSpeedValue * cos(angle);
        ballSpeeds[i].y = ballSpeedValue * sin(angle);

        // Store relative positions
        ballRelativeX[i] = ballPositions[i].x / screenWidth;
        ballRelativeY[i] = ballPositions[i].y / screenHeight;

        // Assign a random unique color to each ball
        ballColors[i].r = GetRandomValue(50, 255);
        ballColors[i].g = GetRandomValue(50, 255);
        ballColors[i].b = GetRandomValue(50, 255);
        ballColors[i].a = 255;
    }

    bool pause = false;
    int framesCounter = 0;

    // Store the initial screen dimensions
    int lastScreenWidth = screenWidth;
    int lastScreenHeight = screenHeight;

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //----------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Check if the window size has changed
        int currentScreenWidth = GetScreenWidth();
        int currentScreenHeight = GetScreenHeight();

        if (currentScreenWidth != lastScreenWidth || currentScreenHeight != lastScreenHeight)
        {
            // Update ball positions proportionally
            for (int i = 0; i < ballCount; i++)
            {
                ballPositions[i].x = ballRelativeX[i] * currentScreenWidth;
                ballPositions[i].y = ballRelativeY[i] * currentScreenHeight;
            }

            // Update stored dimensions
            lastScreenWidth = currentScreenWidth;
            lastScreenHeight = currentScreenHeight;
        }

        // Update
        //-----------------------------------------------------
        if (IsKeyPressed(KEY_SPACE))
            pause = !pause;

        // Get mouse position
        Vector2 mousePosition = GetMousePosition();

        // Detect mouse click and move balls towards the mouse (while maintaining the click)
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            for (int i = 0; i < ballCount; i++)
            {
                Vector2 direction = GetDirectionToMouse(ballPositions[i], mousePosition);
                ballSpeeds[i].x = direction.x * ballSpeedValue;
                ballSpeeds[i].y = direction.y * ballSpeedValue;
            }
        }

        if (!pause)
        {
            for (int i = 0; i < ballCount; i++)
            {
                // Move ball
                ballPositions[i].x += ballSpeeds[i].x;
                ballPositions[i].y += ballSpeeds[i].y;

                // Update relative positions
                ballRelativeX[i] = ballPositions[i].x / currentScreenWidth;
                ballRelativeY[i] = ballPositions[i].y / currentScreenHeight;

                // Check collisions with window borders
                if (ballPositions[i].x > currentScreenWidth - ballRadius)
                {
                    ballPositions[i].x = currentScreenWidth - ballRadius; // Correct position
                    ballSpeeds[i].x *= -1.0f;
                }
                else if (ballPositions[i].x < ballRadius)
                {
                    ballPositions[i].x = ballRadius; // Correct position
                    ballSpeeds[i].x *= -1.0f;
                }

                if (ballPositions[i].y > currentScreenHeight - ballRadius)
                {
                    ballPositions[i].y = currentScreenHeight - ballRadius; // Correct position
                    ballSpeeds[i].y *= -1.0f;
                }
                else if (ballPositions[i].y < ballRadius)
                {
                    ballPositions[i].y = ballRadius; // Correct position
                    ballSpeeds[i].y *= -1.0f;
                }

                // Check collisions with other balls and prevent overlap
                for (int j = i + 1; j < ballCount; j++)
                {
                    if (MyCheckCollisionCircles(ballPositions[i], ballRadius, ballPositions[j], ballRadius))
                    {
                        // Swap velocities for a basic collision response
                        Vector2 temp = ballSpeeds[i];
                        ballSpeeds[i] = ballSpeeds[j];
                        ballSpeeds[j] = temp;

                        // Prevent balls from overlapping after collision
                        PreventBallOverlap(&ballPositions[i], &ballPositions[j], ballRadius);
                    }
                }
            }
        }
        else
            framesCounter++;
        //-----------------------------------------------------

        // Draw
        //-----------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        for (int i = 0; i < ballCount; i++)
        {
            DrawCircleV(ballPositions[i], (float)ballRadius, ballColors[i]);
        }

        DrawText("PRESS SPACE to PAUSE BALL MOVEMENT", 10, GetScreenHeight() - 25, 20, LIGHTGRAY);

        // On pause, draw a blinking message
        if (pause && ((framesCounter / 30) % 2))
            DrawText("PAUSED", currentScreenWidth / 2 - MeasureText("PAUSED", 30) / 2, currentScreenHeight / 2, 30, GRAY);

        DrawFPS(10, 10);

        EndDrawing();
        //-----------------------------------------------------
    }

    // De-Initialization
    //---------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //----------------------------------------------------------

    return 0;
}
