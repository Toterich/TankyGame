#define _USE_MATH_DEFINES
#include <math.h>

#include <cassert>
#include <random>
#include <limits>

// Needs to be last include for VC++
#include "raylib.hpp"

#define ARRAYSIZE(x) sizeof(x)/sizeof(*(x))

rl::Vector2 rotateVector(rl::Vector2 v, float angle) {
    rl::Vector2 res(v);
    res.x = v.x * std::cos(angle) - v.y * std::sin(angle);
    res.y = v.x * std::sin(angle) + v.y * std::cos(angle);
    return res;
}

enum GameScreen {MAINMENU, PAUSE, GAMEPLAY, FINISH};

struct CarSettings {
    float accel;
    float decel;
    float drag;
    float steeringFw;
    float steeringBw;
};

struct Actor {
    CarSettings car;

//    BoundingRect bBox;
    rl::Rectangle bBox;

    float steerAngle;
    float vel;
};

struct Coin {
    bool collected = false;

    rl::Vector2 center;

    float radius = 10;
};

int screenWidth = 800;
int screenHeight = 450;

CarSettings car{
    .accel=0.5f,
    .decel=0.2f,
    .drag=0.05f,
    .steeringFw=0.1f,
    .steeringBw=0.05f};
Actor player;
float trackTime = 0;

constexpr int MAXCOINS = 5;
Coin coins[MAXCOINS];

void initCoins(Coin* coins, size_t size, size_t numCoins) {
    for (int i = 0; i < numCoins; i++) {
        coins[i].center.x = std::rand() % (screenWidth - 10) + 10;
        coins[i].center.y = std::rand() % (screenHeight - 10) + 10;
        coins[i].collected = false;
    }
}

void resetTrack() {
    initCoins(coins, MAXCOINS, 20);

    player = {
        .car=car,
        .bBox = {
            .x = (float)screenWidth/2,
            .y = (float)screenHeight/2,
            .width = 50,
            .height = 30,
        },
        .steerAngle = 0,
        .vel = 0
    };

    trackTime = 0;
}

void timeToString(char* str, size_t len, float time) {
    assert(len >= 10);

    int minutes = time / 60;
    int seconds = (int)time % 60;
    int millis = (time - (int)time) * 1000;
    snprintf(str, len, "%02d:%02d.%03d", minutes, seconds, millis);
}

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    rl::InitWindow(screenWidth, screenHeight, "Tanky");
    rl::SetTargetFPS(60);

    GameScreen currentScreen = MAINMENU;

    // Main game loop
    while (!rl::WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        switch(currentScreen)
        {
            case MAINMENU:
            {
                if (rl::IsKeyPressed(rl::KEY_ENTER))
                {
                    currentScreen = GAMEPLAY;
                    resetTrack();
                }
            }
            break;

            case PAUSE:
            {
                if (rl::IsKeyPressed(rl::KEY_ENTER))
                {
                    currentScreen = GAMEPLAY;
                }
            } break;

            case FINISH:
            {
                if (rl::IsKeyPressed(rl::KEY_R))
                {
                    currentScreen = GAMEPLAY;
                    resetTrack();
                }
            } break;

            case GAMEPLAY:
            {
                if (rl::IsKeyPressed(rl::KEY_P))
                {
                    currentScreen = PAUSE;
                    break;
                }

                if (rl::IsKeyPressed(rl::KEY_R)) {
                    resetTrack();
                }

                trackTime += rl::GetFrameTime();

                // Steering
                if (rl::IsKeyDown(rl::KEY_LEFT)) {
                    if (player.vel > 0) {
                        player.steerAngle -= player.car.steeringFw;
                    }
                    else if (player.vel < 0) {
                        player.steerAngle -= player.car.steeringBw;
                    }
                    if (player.steerAngle < -2 * M_PI) player.steerAngle += 2 * M_PI;
                }
                else if (rl::IsKeyDown(rl::KEY_RIGHT)) {
                    if (player.vel > 0) {
                        player.steerAngle += player.car.steeringFw;
                    }
                    else if (player.vel < 0) {
                        player.steerAngle += player.car.steeringBw;
                    }
                    if (player.steerAngle > 2 * M_PI) player.steerAngle -= 2 * M_PI;
                }

                // Acceleration/Deceleration
                if (rl::IsKeyDown(rl::KEY_UP)) {
                    player.vel += player.car.accel;
                    if (player.vel > 5) player.vel = 5;
                }
                else if (rl::IsKeyDown(rl::KEY_DOWN)) {
                    player.vel -= player.car.decel;
                    if (player.vel < -2) player.vel = -2;
                }
                else {
                    player.vel -= player.car.drag;
                    if (player.vel < 0) player.vel = 0;
                }

                // Move Player
                player.bBox.x += player.vel * std::cos(player.steerAngle);
                player.bBox.y += player.vel * std::cos(player.steerAngle - M_PI/2);

                // Endless screen
                if (player.bBox.x > screenWidth)  player.bBox.x = 0;
                if (player.bBox.x < 0)            player.bBox.x = screenWidth;
                if (player.bBox.y > screenHeight) player.bBox.y = 0;
                if (player.bBox.y < 0)            player.bBox.y = screenHeight;

                rl::Vector2 playerCenter{.x=player.bBox.x + player.bBox.width/2, .y=player.bBox.y + player.bBox.height/2};

                // Collision Check for coins
                bool allCoinsCollected = true;
                for (int i = 0; i < MAXCOINS; i++) {
                    if (coins[i].collected) {
                        continue;
                    }
                    allCoinsCollected = false;

                    // Transform Coin into Player coordinate system for collision check
                    rl::Vector2 transformed_p = coins[i].center;
                    transformed_p -= playerCenter;
                    transformed_p = rotateVector(transformed_p, -player.steerAngle);
                    transformed_p += playerCenter;
                    if (rl::CheckCollisionCircleRec(transformed_p, coins[i].radius, player.bBox)) {
                        coins[i].collected = true;
                    }
                }

                if (allCoinsCollected) {
                    currentScreen = FINISH;
                }

            } break;
            default: break;
        }
        //----------------------------------------------------------------------------------

        rl::BeginDrawing();

            rl::ClearBackground(rl::RAYWHITE);

            switch(currentScreen)
            {
                case MAINMENU:
                {
                    rl::DrawRectangle(0, 0, screenWidth, screenHeight, rl::GREEN);
                    rl::DrawText("START SCREEN", 20, 20, 40, rl::DARKGREEN);
                    rl::DrawText("PRESS ENTER to JUMP to GAMEPLAY SCREEN", 120, 220, 20, rl::DARKGREEN);

                } break;
                case PAUSE:
                {
                    rl::DrawRectangle(0, 0, screenWidth, screenHeight, rl::GREEN);
                    rl::DrawText("PAUSE SCREEN", 20, 20, 40, rl::DARKGREEN);
                    rl::DrawText("PRESS ENTER to JUMP to GAMEPLAY SCREEN", 120, 220, 20, rl::DARKGREEN);

                } break;
                case GAMEPLAY:
                {
                    // Player
                    /*
                    rl::DrawRectangle(player.bBox.x, player.bBox.y,
                                      player.bBox.width, player.bBox.height,
                                      rl::GREEN);
                    */
                    rl::Rectangle drawPlayerBox = player.bBox;
                    drawPlayerBox.x += player.bBox.width/2;
                    drawPlayerBox.y += player.bBox.height/2;
                    rl::DrawRectanglePro(drawPlayerBox,
                                         {player.bBox.width/2, player.bBox.height/2},
                                         player.steerAngle * 180 / M_PI,
                                         rl::PURPLE);
                    // Coins
                    for (int i = 0; i < MAXCOINS; i++) {
                        if (!coins[i].collected) {
                            rl::DrawCircle(coins[i].center.x, coins[i].center.y, coins[i].radius, rl::GOLD);
                        }
                    }

                    char timer[10];
                    timeToString(timer, sizeof(timer), trackTime);
                    rl::DrawText(timer, screenWidth - 100, 20, 20, rl::BLACK);

                    rl::DrawText("GAMEPLAY SCREEN", 20, 20, 40, rl::MAROON);

                } break;
                case FINISH:
                {
                    char finishText[sizeof("Final Time: xx:xx.xxx")];
                    char timer[10];
                    timeToString(timer, sizeof(timer), trackTime);
                    snprintf(finishText, sizeof(finishText), "Final Time: %s", timer);
                    int finishFontSize = 40;
                    int textWidth = rl::MeasureText(finishText, finishFontSize);
                    rl::DrawText(finishText, screenWidth/2 - textWidth/2, screenHeight/2 - 40, finishFontSize, rl::BLUE);

                } break;
                default: break;
            }

        rl::EndDrawing();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    // TODO: Unload all loaded data (textures, fonts, audio) here!

    rl::CloseWindow();
    return 0;
}
