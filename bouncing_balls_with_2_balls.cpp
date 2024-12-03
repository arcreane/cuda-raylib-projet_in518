/*******************************************************************************************
*
*   raylib [shapes] example - bouncing ball
*
*   Example originally created with raylib 2.5, last time updated with raylib 2.5
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2013-2024 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <math.h>

// Function to detect collision between two circles
bool MyCheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2)
{
    float distance = sqrt(pow(center2.x - center1.x, 2) + pow(center2.y - center1.y, 2));
    return distance <= (radius1 + radius2);
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
    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - bouncing ball");
    // Ball 1 properties
    Vector2 ball1Position = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    Vector2 ball1Speed = { 5.0f, 4.0f };
    int ball1Radius = 20;

    // Ball 2 properties
    Vector2 ball2Position = { 2 * GetScreenWidth() / 3.0f, GetScreenHeight() / 2.0f };
    Vector2 ball2Speed = { -3.0f, 4.0f };
    int ball2Radius = 20;

    bool pause = 0;
    int framesCounter = 0;

    //Store the initial screen dimensions
    int lastScreenWidth = GetScreenWidth();
    int lastScreenHeight = GetScreenHeight();

    // Calculate initial relative position
    float ball1RelativeX = ball1Position.x / lastScreenWidth;
    float ball1RelativeY = ball1Position.y / lastScreenHeight;
    float ball2RelativeX = ball2Position.x / lastScreenWidth;
    float ball2RelativeY = ball2Position.y / lastScreenHeight;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //----------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        

        // Check if the window size has changed
        int currentScreenWidth = GetScreenWidth();
        int currentScreenHeight = GetScreenHeight();

        if (currentScreenWidth != lastScreenWidth || currentScreenHeight != lastScreenHeight)
        {
            // Update ball position proportionally
            ball1Position.x = ball1RelativeX * currentScreenWidth;
            ball1Position.y = ball1RelativeY * currentScreenHeight;
            ball2Position.x = ball2RelativeX * currentScreenWidth;
            ball2Position.y = ball2RelativeY * currentScreenHeight;

            // Update stored dimensions
            lastScreenWidth = currentScreenWidth;
            lastScreenHeight = currentScreenHeight;
        }
        
        // Update
        //-----------------------------------------------------
        if (IsKeyPressed(KEY_SPACE)) pause = !pause;

        if (!pause)
        {   // Move Ball 1
            ball1Position.x += ball1Speed.x;
            ball1Position.y += ball1Speed.y;

            // Move Ball 2
            ball2Position.x += ball2Speed.x;
            ball2Position.y += ball2Speed.y;

            // Update relative positions
            ball1RelativeX = ball1Position.x / currentScreenWidth;
            ball1RelativeY = ball1Position.y / currentScreenHeight;

            ball2RelativeX = ball2Position.x / currentScreenWidth;
            ball2RelativeY = ball2Position.y / currentScreenHeight;

            //Check collisions with window borders for Ball 1
            
            if ((ball1Position.x >= (currentScreenWidth - ball1Radius)) || (ball1Position.x <= ball1Radius))
                ball1Speed.x *= -1.0f;
            if ((ball1Position.y >= (currentScreenHeight - ball1Radius)) || (ball1Position.y <= ball1Radius))
                ball1Speed.y *= -1.0f;
            // Check collisions with window borders for Ball 2
            if ((ball2Position.x >= (currentScreenWidth - ball2Radius)) || (ball2Position.x <= ball2Radius))
                ball2Speed.x *= -1.0f;
            if ((ball2Position.y >= (currentScreenHeight - ball2Radius)) || (ball2Position.y <= ball2Radius))
                ball2Speed.y *= -1.0f;
            if (MyCheckCollisionCircles(ball1Position, ball1Radius, ball2Position, ball2Radius))
            {
                // Swap velocities (simple collision response)
                Vector2 temp = ball1Speed;
                ball1Speed = ball2Speed;
                ball2Speed = temp;
            }
        }
        else framesCounter++;
        //-----------------------------------------------------

        // Draw
        //-----------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawCircleV(ball1Position, (float)ball1Radius, MAROON);
        DrawCircleV(ball2Position, (float)ball2Radius, BLUE);
        DrawText("PRESS SPACE to PAUSE BALL MOVEMENT", 10, GetScreenHeight() - 25, 20, LIGHTGRAY);

        // On pause, we draw a blinking message
        // if (pause && ((framesCounter / 30) % 2)) DrawText("PAUSED", 350, 200, 30, GRAY);
        if (pause && ((framesCounter / 30) % 2))
            DrawText("PAUSED", currentScreenWidth / 2 - MeasureText("PAUSED", 30) / 2, currentScreenHeight / 2, 30, GRAY);
        DrawFPS(10, 10);

        EndDrawing();
        //-----------------------------------------------------
    }

    // De-Initialization
    //---------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //----------------------------------------------------------

    return 0;
}