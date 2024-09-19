#define _USE_MATH_DEFINES
#include <math.h>

#include <cassert>
#include <random>
#include <limits>

// Needs to be last include for VC++
#include "raylib.hpp"

#include "types.hpp"

#define ARRAYSIZE(x) sizeof(x)/sizeof(*(x))

rl::Vector2 rotateVector(rl::Vector2 v, float angle) {
    rl::Vector2 res(v);
    res.x = v.x * std::cos(angle) - v.y * std::sin(angle);
    res.y = v.x * std::sin(angle) + v.y * std::cos(angle);
    return res;
}

enum GameScreen {MAINMENU, PAUSE, GAMEPLAY, FINISH};

int screenWidth = 800;
int screenHeight = 450;

CarSettings car{
    .accel=0.5f,
    .decel=0.2f,
    .drag=0.05f,
    .steeringFw=0.1f,
    .steeringBw=0.05f};

SimContext simCtx;

void initCoins(Coin* coins, size_t numCoins) {
    for (int i = 0; i < numCoins; i++) {
        coins[i].center.x = std::rand() % (screenWidth - 10) + 10;
        coins[i].center.y = std::rand() % (screenHeight - 10) + 10;
        coins[i].collected = false;
    }
}

void resetTrack() {
    initCoins(simCtx.coins, MAXCOINS);

    simCtx.player = {
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

    simCtx.trackTime = 0;
}

void timeToString(char* str, size_t len, float time) {
    assert(len >= 10);

    int minutes = time / 60;
    int seconds = (int)time % 60;
    int millis = (time - (int)time) * 1000;
    snprintf(str, len, "%02d:%02d.%03d", minutes, seconds, millis);
}

bool AdvanceSimulation(SimContext& sim, ControlState const &ctrl, float frameTime) {
    sim.trackTime += rl::GetFrameTime();

    // Steering
    if (ctrl.left) {
        if (sim.player.vel > 0) {
            sim.player.steerAngle -= sim.player.car.steeringFw;
        }
        else if (sim.player.vel < 0) {
            sim.player.steerAngle -= sim.player.car.steeringBw;
        }
        if (sim.player.steerAngle < -2 * M_PI) sim.player.steerAngle += 2 * M_PI;
    }
    else if (ctrl.right) {
        if (sim.player.vel > 0) {
            sim.player.steerAngle += sim.player.car.steeringFw;
        }
        else if (sim.player.vel < 0) {
            sim.player.steerAngle += sim.player.car.steeringBw;
        }
        if (sim.player.steerAngle > 2 * M_PI) sim.player.steerAngle -= 2 * M_PI;
    }

    // Acceleration/Deceleration
    if (ctrl.fw) {
        sim.player.vel += sim.player.car.accel;
        if (sim.player.vel > 5) sim.player.vel = 5;
    }
    else if (ctrl.bw) {
        sim.player.vel -= sim.player.car.decel;
        if (sim.player.vel < -2) sim.player.vel = -2;
    }
    else {
        sim.player.vel -= sim.player.car.drag;
        if (sim.player.vel < 0) sim.player.vel = 0;
    }

    // Move Player
    sim.player.bBox.x += sim.player.vel * std::cos(sim.player.steerAngle);
    sim.player.bBox.y += sim.player.vel * std::cos(sim.player.steerAngle - M_PI/2);

    // Endless screen
    if (sim.player.bBox.x > screenWidth)  sim.player.bBox.x = 0;
    if (sim.player.bBox.x < 0)            sim.player.bBox.x = screenWidth;
    if (sim.player.bBox.y > screenHeight) sim.player.bBox.y = 0;
    if (sim.player.bBox.y < 0)            sim.player.bBox.y = screenHeight;

    rl::Vector2 playerCenter{.x=sim.player.bBox.x + sim.player.bBox.width/2, .y=sim.player.bBox.y + sim.player.bBox.height/2};

    // Collision Check for coins
    bool allCoinsCollected = true;
    for (int i = 0; i < MAXCOINS; i++) {
        if (sim.coins[i].collected) {
            continue;
        }
        allCoinsCollected = false;

        // Transform Coin into Player coordinate system for collision check
        rl::Vector2 transformed_p = sim.coins[i].center;
        transformed_p -= playerCenter;
        transformed_p = rotateVector(transformed_p, -sim.player.steerAngle);
        transformed_p += playerCenter;
        if (rl::CheckCollisionCircleRec(transformed_p, sim.coins[i].radius, sim.player.bBox)) {
            sim.coins[i].collected = true;
        }
    }

    return allCoinsCollected;
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

                ControlState ctrl = {};
                ctrl.left = rl::IsKeyDown(rl::KEY_LEFT);
                ctrl.right = rl::IsKeyDown(rl::KEY_RIGHT);
                ctrl.fw = rl::IsKeyDown(rl::KEY_UP);
                ctrl.bw = rl::IsKeyDown(rl::KEY_DOWN);

                if (AdvanceSimulation(simCtx, ctrl, rl::GetFrameTime())) {
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
                    rl::Rectangle drawPlayerBox = simCtx.player.bBox;
                    drawPlayerBox.x += simCtx.player.bBox.width/2;
                    drawPlayerBox.y += simCtx.player.bBox.height/2;
                    rl::DrawRectanglePro(drawPlayerBox,
                                         {simCtx.player.bBox.width/2, simCtx.player.bBox.height/2},
                                         simCtx.player.steerAngle * 180 / M_PI,
                                         rl::PURPLE);
                    // Coins
                    for (int i = 0; i < MAXCOINS; i++) {
                        if (!simCtx.coins[i].collected) {
                            rl::DrawCircle(simCtx.coins[i].center.x, simCtx.coins[i].center.y, simCtx.coins[i].radius, rl::GOLD);
                        }
                    }

                    char timer[10];
                    timeToString(timer, sizeof(timer), simCtx.trackTime);
                    rl::DrawText(timer, screenWidth - 100, 20, 20, rl::BLACK);

                    rl::DrawText("GAMEPLAY SCREEN", 20, 20, 40, rl::MAROON);

                } break;
                case FINISH:
                {
                    char finishText[sizeof("Final Time: xx:xx.xxx")];
                    char timer[10];
                    timeToString(timer, sizeof(timer), simCtx.trackTime);
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
