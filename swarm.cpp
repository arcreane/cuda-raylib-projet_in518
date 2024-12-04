#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PARTICLES 2000 // Maximum number of particles in the system
#define PARTICLE_LIFETIME 1.0f // Lifetime in seconds

typedef struct Particle {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float lifetime; // Time remaining
    bool active; // Is the particle active
} Particle;

Particle particles[MAX_PARTICLES];

// Initialize particles
void InitParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].active = false;
    }
}


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

    //ball setup
    const int ballCount = 10;
    const int ballRadius = 20;
    const float ballSpeedValue = 2.5f;

    bool ballActive[ballCount];
    for (int i = 0; i < ballCount; i++) {
        ballActive[i] = true;
    }

    Vector2 ballPositions[ballCount];
    Vector2 ballSpeeds[ballCount];
    Color ballColors[ballCount];

    //pew setup
    const int pewCount = 10;
    const int pewRadius = 5;
    const float pewSpeedValue = 10.0f;

    bool pewActive[pewCount];
    for (int i = 0; i < pewCount; i++) {
        pewActive[i] = false;
    }

    Vector2 pewPositions[pewCount];
    Vector2 pewSpeeds[pewCount];
    Color pewColors[pewCount];

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
        ballColors[i].r = GetRandomValue(10, 50);
        ballColors[i].g = GetRandomValue(150, 255);
        ballColors[i].b = GetRandomValue(10, 50);
        ballColors[i].a = 255;

        // Assign a random unique color to each ball
        pewColors[i].r = GetRandomValue(150, 255);
        pewColors[i].g = GetRandomValue(10, 50);
        pewColors[i].b = GetRandomValue(10, 50);
        pewColors[i].a = 255;
    }

    Vector2 squarePos = { screenWidth / 2, screenHeight / 2 };
    Vector2 squareSize = { 30, 30 };

    float dashCooldown = 0.0f;  // Cooldown timer for the dash
    const float dashDistance = 100.0f; // Distance of the dash
    const float dashCooldownTime = 3.0f; // Dash cooldown time in seconds

    float pewCooldown = 0.0f;  // Cooldown timer for the projectile
    const float pewCooldownTime = 1.0f;

    int score = 0;
    bool gameOver = false;
    bool win = false;
    bool pause = false;
    float survivalTime = 0.0f;
    float speedMultiplier = 1.0f;

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        int currentScreenWidth = GetScreenWidth();
        int currentScreenHeight = GetScreenHeight();


        bool allBallsInactive = true;
        for (int i = 0; i < ballCount; i++) {
            if (ballActive[i]) { // If any ball is still active
                allBallsInactive = false;
                break; // No need to check further
            }
        }

        if (allBallsInactive) {
            // Player wins
            DrawText("You Win!", screenWidth / 2 - MeasureText("You Win!", 40) / 2, screenHeight / 2 - 20, 40, GREEN);

            // Optionally, pause the game or end the session
            gameOver = true;
            win = true;
        }

        // Toggle pause on SPACE press, but only if the game is not over
        if (IsKeyPressed(KEY_SPACE) && !gameOver)
        {
            pause = !pause;
        }

        if (!pause && !gameOver)
        {
            survivalTime += GetFrameTime();
            speedMultiplier += GetFrameTime() * 0.01f; // Increase speed gradually
            if (dashCooldown > 0.0f)
                dashCooldown -= GetFrameTime();

            if (pewCooldown > 0.0f)
                pewCooldown -= GetFrameTime();

            for (int i = 0; i < pewCount; i++)
            {
                if (pewActive[i] == true)
                {
                    pewPositions[i].x += pewSpeeds[i].x;
                    pewPositions[i].y += pewSpeeds[i].y;
                }

                for (int y = 0; y < ballCount; y++)
                {
                    if (MyCheckCollisionCircles(ballPositions[y], ballRadius, pewPositions[i], pewRadius) && ballActive[y] && pewActive[i]) {
                        ballActive[y] = false;
                        pewActive[i] = false;

                        // Spawn particles at the collision point
                        for (int p = 0; p < 200; p++) {
                            for (int j = 0; j < MAX_PARTICLES; j++) {
                                if (!particles[j].active) {
                                    particles[j].active = true;
                                    particles[j].position = ballPositions[y];
                                    particles[j].velocity.x = GetRandomValue(-100, 100) / 100.0f; // Random velocity
                                    particles[j].velocity.y = GetRandomValue(-100, 100) / 100.0f;
                                    particles[j].color.r = 255; // Red
                                    particles[j].color.g = 0;   // Green
                                    particles[j].color.b = 0;   // Blue
                                    particles[j].color.a = 255; // Full opacity

                                    particles[j].lifetime = PARTICLE_LIFETIME;
                                    break; // Move to the next particle slot
                                }
                            }
                        }
                    }

                }
            }
            for (int i = 0; i < MAX_PARTICLES; i++) {
                if (particles[i].active) {
                    // Update position
                    particles[i].position.x += particles[i].velocity.x;
                    particles[i].position.y += particles[i].velocity.y;

                    // Update lifetime
                    particles[i].lifetime -= GetFrameTime();
                    if (particles[i].lifetime <= 0) {
                        particles[i].active = false; // Deactivate the particle
                    }

                    // Fade out
                    particles[i].color.a = (unsigned char)(255 * (particles[i].lifetime / PARTICLE_LIFETIME));
                }
            }


            for (int i = 0; i < ballCount; i++)
            {
                // Calculate direction vector toward the player
                Vector2 direction = { squarePos.x + squareSize.x / 2 - ballPositions[i].x,
                                      squarePos.y + squareSize.y / 2 - ballPositions[i].y };

                // Normalize the direction vector
                float length = sqrt(direction.x * direction.x + direction.y * direction.y);
                if (length != 0)
                {
                    direction.x /= length;
                    direction.y /= length;
                }

                // Set ball speeds to move toward the player
                ballSpeeds[i].x = direction.x * ballSpeedValue * speedMultiplier;
                ballSpeeds[i].y = direction.y * ballSpeedValue * speedMultiplier;

                // Move the balls
                ballPositions[i].x += ballSpeeds[i].x;
                ballPositions[i].y += ballSpeeds[i].y;

                // Handle collision with walls (optional for bouncing effect)
                if (ballPositions[i].x > currentScreenWidth - ballRadius || ballPositions[i].x < ballRadius)
                    ballSpeeds[i].x *= -1.0f;

                if (ballPositions[i].y > currentScreenHeight - ballRadius || ballPositions[i].y < ballRadius)
                    ballSpeeds[i].y *= -1.0f;

                // Check collision with the square
                if (MyCheckCollisionCircles(ballPositions[i], ballRadius,{squarePos.x + squareSize.x / 2, squarePos.y + squareSize.y / 2}, squareSize.x / 2) && ballActive[i] == true)
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

            //dash 
            if (IsKeyPressed(KEY_LEFT_SHIFT) && dashCooldown <= 0.0f)
            {
                if (IsKeyDown(KEY_W) && squarePos.y > 0)
                    squarePos.y = dashDistance;
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

            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && pewCooldown <= 0.0f)
            {
                for (int i = 0; i < pewCount; i++) {
                    if (!pewActive[i]) // Find an inactive projectile
                    {
                        pewActive[i] = true;

                        // Set initial position of the projectile (center of the player)
                        pewPositions[i].x = squarePos.x + squareSize.x / 2;
                        pewPositions[i].y = squarePos.y + squareSize.y / 2;

                        // Get the mouse position
                        Vector2 mousePos = GetMousePosition();

                        // Calculate direction vector
                        Vector2 direction = {
                            mousePos.x - pewPositions[i].x,
                            mousePos.y - pewPositions[i].y
                        };

                        // Normalize the direction vector
                        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
                        if (length != 0) {
                            direction.x /= length;
                            direction.y /= length;
                        }

                        // Assign speed to the projectile
                        pewSpeeds[i].x = direction.x * pewSpeedValue;
                        pewSpeeds[i].y = direction.y * pewSpeedValue;

                        // Start pew cooldown
                        pewCooldown = pewCooldownTime;
                        break; // Fire one projectile at a time
                    }
                }
            }


        }
        else if (gameOver && IsKeyPressed(KEY_ENTER))
        {
            gameOver = false;
            win = false;
            survivalTime = 0.0f;
            speedMultiplier = 1.0f;
            score = 0;
            dashCooldown = 0.0f;
            pewCooldown = 0.0f;

            // Reset balls
            for (int i = 0; i < ballCount; i++)
            {
                ballPositions[i].x = (float)GetRandomValue(ballRadius, currentScreenWidth - ballRadius);
                ballPositions[i].y = (float)GetRandomValue(ballRadius, currentScreenHeight - ballRadius);

                float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
                ballSpeeds[i].x = ballSpeedValue * cos(angle);
                ballSpeeds[i].y = ballSpeedValue * sin(angle);
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);

        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (particles[i].active) {
                DrawCircleV(particles[i].position, 3.0f, particles[i].color);
            }
        }


        for (int i = 0; i < ballCount; i++)
            if (ballActive[i] == true)
            {
                DrawCircleV(ballPositions[i], ballRadius, ballColors[i]);
            }

        for (int i = 0; i < pewCount; i++)
            if (pewActive[i] == true)
            {
                DrawCircleV(pewPositions[i], pewRadius, pewColors[i]);
            }

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
                DrawText(TextFormat("Pew Cooldown: %.1f", pewCooldown > 0.0f ? dashCooldown : 0.0f), 10, 40, 20, DARKGRAY);
                DrawText("Press W/S/A/D to move", 10, 70, 20, LIGHTGRAY);
                DrawText("Press SPACE to pause", 10, 100, 20, LIGHTGRAY);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

