#include "raylib.hpp"

#include <cmath>

enum GameScreen {PAUSE, GAMEPLAY};

struct CarSettings {
    float accel;
    float decel;
    float drag;
    float steeringFw;
    float steeringBw;
};

struct Actor {
    CarSettings car;

    float posX;
    float posY;

    float steerAngle;

    float vel;

    float height;
    float width;
};

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    rl::InitWindow(screenWidth, screenHeight, "Tanky");

    GameScreen currentScreen = GAMEPLAY;

    rl::SetTargetFPS(60);

    CarSettings car{
        .accel=0.5f,
        .decel=0.2f,
        .drag=0.05f,
        .steeringFw=0.1f,
        .steeringBw=0.05f};

    Actor player{
        .car=car,
        .posX=screenWidth/2,
        .posY=screenHeight/2,
        .steerAngle=0,
        .vel=0,
        .height=30,
        .width=50};

    // Main game loop
    while (!rl::WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        switch(currentScreen)
        {
            case PAUSE:
            {
                if (rl::IsKeyPressed(rl::KEY_ENTER))
                {
                    currentScreen = GAMEPLAY;
                }
            } break;
            case GAMEPLAY:
            {
                if (rl::IsKeyPressed(rl::KEY_P))
                {
                    currentScreen = PAUSE;
                    break;
                }

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
                player.posX += player.vel * std::cos(player.steerAngle);
                player.posY += player.vel * std::cos(player.steerAngle - M_PI/2);

                // Endless screen
                if (player.posX > screenWidth)  player.posX = 0;
                if (player.posX < 0)            player.posX = screenWidth;
                if (player.posY > screenHeight) player.posY = 0;
                if (player.posY < 0)            player.posY = screenHeight;

            } break;
            default: break;
        }
        //----------------------------------------------------------------------------------

        rl::BeginDrawing();

            rl::ClearBackground(rl::RAYWHITE);

            switch(currentScreen)
            {
                case PAUSE:
                {
                    rl::DrawRectangle(0, 0, screenWidth, screenHeight, rl::GREEN);
                    rl::DrawText("PAUSE SCREEN", 20, 20, 40, rl::DARKGREEN);
                    rl::DrawText("PRESS ENTER to JUMP to GAMEPLAY SCREEN", 120, 220, 20, rl::DARKGREEN);

                } break;
                case GAMEPLAY:
                {
                    rl::DrawRectanglePro(rl::Rectangle{.x=player.posX, .y=player.posY,
                                                       .width=player.width, .height=player.height},
                                         {player.width/2, player.height/2},
                                         player.steerAngle * 180 / M_PI,
                                         rl::PURPLE);
                    rl::DrawText("GAMEPLAY SCREEN", 20, 20, 40, rl::MAROON);

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
