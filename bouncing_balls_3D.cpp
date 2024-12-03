#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

// Function to detect collision between two spheres
bool MyCheckCollisionSpheres(Vector3 center1, float radius1, Vector3 center2, float radius2)
{
    float distance = sqrt(pow(center2.x - center1.x, 2) + pow(center2.y - center1.y, 2) + pow(center2.z - center1.z, 2));
    return distance <= (radius1 + radius2);
}

// Function to invert the direction of the ball's velocity (for collision)
void InvertDirection(Vector3* speed, char axis)
{
    switch (axis)
    {
    case 'x':
        speed->x *= -1.0f;
        break;
    case 'y':
        speed->y *= -1.0f;
        break;
    case 'z':
        speed->z *= -1.0f;
        break;
    }
}

// Function to separate the two balls after a collision
void SeparateBalls(Vector3* pos1, Vector3* pos2, float radius)
{
    Vector3 direction = { pos2->x - pos1->x, pos2->y - pos1->y, pos2->z - pos1->z };
    float distance = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

    // Move balls apart by an amount so they are no longer colliding
    float overlap = (radius * 2.0f) - distance;  // Amount they overlap
    if (overlap > 0)
    {
        // Normalize direction and move balls away from each other
        direction.x /= distance;
        direction.y /= distance;
        direction.z /= distance;

        pos1->x -= direction.x * overlap * 0.5f;
        pos1->y -= direction.y * overlap * 0.5f;
        pos1->z -= direction.z * overlap * 0.5f;

        pos2->x += direction.x * overlap * 0.5f;
        pos2->y += direction.y * overlap * 0.5f;
        pos2->z += direction.z * overlap * 0.5f;
    }
}

void DrawWireframeBox(Vector3 min, Vector3 max, Color color)
{
    // Draw edges of the box
    DrawLine3D({ min.x, min.y, min.z }, { max.x, min.y, min.z }, color);
    DrawLine3D({ max.x, min.y, min.z }, { max.x, max.y, min.z }, color);
    DrawLine3D({ max.x, max.y, min.z }, { min.x, max.y, min.z }, color);
    DrawLine3D({ min.x, max.y, min.z }, { min.x, min.y, min.z }, color);

    DrawLine3D({ min.x, min.y, max.z }, { max.x, min.y, max.z }, color);
    DrawLine3D({ max.x, min.y, max.z }, { max.x, max.y, max.z }, color);
    DrawLine3D({ max.x, max.y, max.z }, { min.x, max.y, max.z }, color);
    DrawLine3D({ min.x, max.y, max.z }, { min.x, min.y, max.z }, color);

    DrawLine3D({ min.x, min.y, min.z }, { min.x, min.y, max.z }, color);
    DrawLine3D({ max.x, min.y, min.z }, { max.x, min.y, max.z }, color);
    DrawLine3D({ max.x, max.y, min.z }, { max.x, max.y, max.z }, color);
    DrawLine3D({ min.x, max.y, min.z }, { min.x, max.y, max.z }, color);
}

int main(void)
{
    // Initialization
    //---------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    // Enable the resizable window flag
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - bouncing spheres in 3D");

    const int ballCount = 10; // Number of balls
    const float ballRadius = 1.0f; // Radius of all balls
    const float ballSpeedValue = 4.0f; // Speed magnitude for all balls

    Vector3 ballPositions[ballCount];
    Vector3 ballSpeeds[ballCount];
    Color ballColors[ballCount];

    srand(time(NULL)); // Seed for random numbers

    // Initialize ball positions, speeds, and colors
    for (int i = 0; i < ballCount; i++)
    {
        ballPositions[i].x = GetRandomValue(-10, 10);
        ballPositions[i].y = GetRandomValue(-5, 5);
        ballPositions[i].z = GetRandomValue(-10, 10);

        // Assign random directions
        float angleXY = GetRandomValue(0, 360) * DEG2RAD;
        float angleZ = GetRandomValue(-45, 45) * DEG2RAD;
        ballSpeeds[i].x = ballSpeedValue * cos(angleXY) * cos(angleZ);
        ballSpeeds[i].y = ballSpeedValue * sin(angleZ);
        ballSpeeds[i].z = ballSpeedValue * sin(angleXY) * cos(angleZ);

        // Assign a random unique color to each ball
        ballColors[i].r = GetRandomValue(50, 255);
        ballColors[i].g = GetRandomValue(50, 255);
        ballColors[i].b = GetRandomValue(50, 255);
        ballColors[i].a = 255;
    }

    // Camera setup
    Camera3D camera = { {0.0f, 10.0f, 20.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE };

    bool pause = false;

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //----------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //-----------------------------------------------------
        if (IsKeyPressed(KEY_SPACE))
            pause = !pause;

        if (!pause)
        {
            for (int i = 0; i < ballCount; i++)
            {
                /*
                // Move ball
                ballPositions[i].x += ballSpeeds[i].x * GetFrameTime();
                ballPositions[i].y += ballSpeeds[i].y * GetFrameTime();
                ballPositions[i].z += ballSpeeds[i].z * GetFrameTime();

                // Check collisions with world bounds and invert direction
                if (ballPositions[i].x >= 10.0f || ballPositions[i].x <= -10.0f)
                {
                    InvertDirection(&ballSpeeds[i], 'x');
                    // Ensure the ball is not stuck in the border
                    ballPositions[i].x = (ballPositions[i].x > 10.0f) ? 10.0f : -10.0f;
                }
                if (ballPositions[i].y >= 5.0f || ballPositions[i].y <= -5.0f)
                {
                    InvertDirection(&ballSpeeds[i], 'y');
                    // Ensure the ball is not stuck in the border
                    ballPositions[i].y = (ballPositions[i].y > 5.0f) ? 5.0f : -5.0f;
                }
                if (ballPositions[i].z >= 10.0f || ballPositions[i].z <= -10.0f)
                {
                    InvertDirection(&ballSpeeds[i], 'z');
                    // Ensure the ball is not stuck in the border
                    ballPositions[i].z = (ballPositions[i].z > 10.0f) ? 10.0f : -10.0f;
                }
                */

                // Move ball
                ballPositions[i].x += ballSpeeds[i].x * GetFrameTime();
                ballPositions[i].y += ballSpeeds[i].y * GetFrameTime();
                ballPositions[i].z += ballSpeeds[i].z * GetFrameTime();

                // Check collisions with world bounds and invert direction
                if (ballPositions[i].x >= 10.0f || ballPositions[i].x <= -10.0f)
                {
                    InvertDirection(&ballSpeeds[i], 'x');
                    // Adjust the ball position to ensure it's inside the bounds
                    if (ballPositions[i].x > 10.0f)
                        ballPositions[i].x = 10.0f - 0.01f;  // Slightly inside the boundary
                    if (ballPositions[i].x < -10.0f)
                        ballPositions[i].x = -10.0f + 0.01f; // Slightly inside the boundary
                }
                if (ballPositions[i].y >= 5.0f || ballPositions[i].y <= -5.0f)
                {
                    InvertDirection(&ballSpeeds[i], 'y');
                    // Adjust the ball position to ensure it's inside the bounds
                    if (ballPositions[i].y > 5.0f)
                        ballPositions[i].y = 5.0f - 0.01f;  // Slightly inside the boundary
                    if (ballPositions[i].y < -5.0f)
                        ballPositions[i].y = -5.0f + 0.01f; // Slightly inside the boundary
                }
                if (ballPositions[i].z >= 10.0f || ballPositions[i].z <= -10.0f)
                {
                    InvertDirection(&ballSpeeds[i], 'z');
                    // Adjust the ball position to ensure it's inside the bounds
                    if (ballPositions[i].z > 10.0f)
                        ballPositions[i].z = 10.0f - 0.01f;  // Slightly inside the boundary
                    if (ballPositions[i].z < -10.0f)
                        ballPositions[i].z = -10.0f + 0.01f; // Slightly inside the boundary
                }

                // Check collisions with other balls and invert direction
                for (int j = i + 1; j < ballCount; j++)
                {
                    if (MyCheckCollisionSpheres(ballPositions[i], ballRadius, ballPositions[j], ballRadius))
                    {
                        // Separate the balls to avoid them sticking together
                        SeparateBalls(&ballPositions[i], &ballPositions[j], ballRadius);

                        // Invert velocities after collision
                        InvertDirection(&ballSpeeds[i], 'x');
                        InvertDirection(&ballSpeeds[i], 'y');
                        InvertDirection(&ballSpeeds[i], 'z');
                        InvertDirection(&ballSpeeds[j], 'x');
                        InvertDirection(&ballSpeeds[j], 'y');
                        InvertDirection(&ballSpeeds[j], 'z');
                    }
                }
            }
        }
        //-----------------------------------------------------

        // Draw
        //-----------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        // Draw wireframe box representing the boundaries
        Vector3 boxMin = { -10.0f, -5.0f, -10.0f };
        Vector3 boxMax = { 10.0f, 5.0f, 10.0f };
        DrawWireframeBox(boxMin, boxMax, DARKGRAY);

        // Draw balls
        for (int i = 0; i < ballCount; i++)
        {
            DrawSphere(ballPositions[i], ballRadius, ballColors[i]);
        }

        EndMode3D();

        DrawText("PRESS SPACE to PAUSE", 10, 10, 20, DARKGRAY);
        DrawFPS(10, 30);

        EndDrawing();
        //-----------------------------------------------------
    }

    // De-Initialization
    //---------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //----------------------------------------------------------

    return 0;
}
