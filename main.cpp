#include "raylib.hpp"

enum GameScreen {PAUSE, GAMEPLAY};

struct Actor {
    int posX;
    int posY;

    float dirX;
    float dirY;

    float vel;

    int height;
    int width;
};

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    rl::InitWindow(screenWidth, screenHeight, "Tanky");

    GameScreen currentScreen = PAUSE;

    rl::SetTargetFPS(60);

    Actor player{.posX=screenWidth/2, .posY=screenHeight/2, .dirX=1, .dirY=0, .vel=0, .height=30, .width=50};

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

                }


                // Acceleration/Deceleration
                if (rl::IsKeyDown(rl::KEY_UP)) {
                    player.vel += .5f;
                    if (player.vel > 5) player.vel = 5;
                }
                else if (rl::IsKeyDown(rl::KEY_DOWN)) {
                    player.vel -= .5f;
                    if (player.vel < -2) player.vel = -2;
                }
                else {
                    player.vel -= .1f;
                    if (player.vel < 0) player.vel = 0;
                }
                player.posX += player.vel * player.dirX;
                player.posY += player.vel * player.dirY;

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
                    rl::DrawRectangle(player.posX, player.posY, player.width, player.height, rl::PURPLE);
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
