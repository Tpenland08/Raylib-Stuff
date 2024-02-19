
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h"
#include "structs.h"

/*
- Keybinds -
Move : WAD/Arrows
Reset : R
Shoot : E/Space
Warp : Q
*/


// MUSIC FROM Astroghix on ITCH & Gregor Quendel

#define MAX_BULLETS 10
#define HEALTH_WIDTH 400
#define PLANET_COUNT 5

#define TRANSPARENTGRAY (Color){ 60, 60, 65, 205 }
#define TRANSPARENTBLUE (Color){ 69, 69, 169, 155 }

// Set true to close game
bool exitWindow = false;
bool gamePaused = false;

float toRadians(float degrees){
    return(degrees * (M_PI / 180.0));
}

int menuMouseSelect(Vector2 mousePos, Rectangle button0, Rectangle button1, Rectangle button2, Rectangle button3, Rectangle button4, int previous, Sound buttonSound){
    if(CheckCollisionPointRec(mousePos,button0) == 1){
        if(previous != 0){PlaySound(buttonSound);}
        return 0;
    }else if(CheckCollisionPointRec(mousePos, button1) == 1){
        if(previous != 1){PlaySound(buttonSound);}
        return 1;
    }else if(CheckCollisionPointRec(mousePos, button2) == 1){
        if(previous != 2){PlaySound(buttonSound);}
        return 2;
    }else if(CheckCollisionPointRec(mousePos, button3) == 1){
        if(previous != 3){PlaySound(buttonSound);}
        return 3;
    }else if(CheckCollisionPointRec(mousePos, button4) == 1){
        if(previous != 4){PlaySound(buttonSound);}
        return 4;
    }else{
        return previous;
    }
}

float roundNumber(float x, int y) { // round x to the nearest y
    return round(x / y) * y;
}

planet getNearestPlanet(planet planetList[PLANET_COUNT], Vector2 playerPos){
    planet planets[PLANET_COUNT];
    for(int i = 0; i < PLANET_COUNT; i++){
        planets[i] = planetList[i];
    }
    planet nearestPlanet;
    float shortestDistance = 0;
    for(int i = 0; i < sizeof(planets) / sizeof(planets[0]); i++){
        if(Vector2Distance(planets[i].worldPos, playerPos) < shortestDistance){
            nearestPlanet = planets[i];
            shortestDistance = Vector2Distance(planets[i].worldPos, playerPos);
        }
        if(shortestDistance == 0){
            nearestPlanet = planets[i];
            shortestDistance = Vector2Distance(planets[i].worldPos, playerPos);
        }
    }
    return(nearestPlanet);
}


int main(void)
{

    InitWindow(0, 0, "spacegame");
    ToggleFullscreen();
    SetExitKey(KEY_F4);
    const int screenWidth = GetMonitorWidth(GetCurrentMonitor());
    const int screenHeight = GetMonitorHeight(GetCurrentMonitor());
    InitAudioDevice();
    SetMasterVolume(0.7);
    SetTargetFPS(60);

    printf("screen res: %dx%d\n", screenWidth, screenHeight);

    ChangeDirectory(GetApplicationDirectory());
    ChangeDirectory("./assets");

    // Sounds
    Sound explosionSound = LoadSound("./explosion.wav");
    Sound buttonSound = LoadSound("./button.wav");
    Sound shootSound = LoadSound("./shoot.wav");
    Sound warpSound = LoadSound("./warp.wav");

    // Music
    int songNum = GetRandomValue(0, 3);
    Music songs[4];
    songs[0] = LoadMusicStream("./music1.mp3");
    songs[1] = LoadMusicStream("./music2.mp3");
    songs[2] = LoadMusicStream("./music3.mp3");
    songs[3] = LoadMusicStream("./music4.wav");
    PlayMusicStream(songs[songNum]);

    // Character
    Image characterOffImage = LoadImage("./character.png");
    Image characterOnImage =  ImageCopy(characterOffImage);
    ImageCrop(&characterOffImage, (Rectangle){1, 0, 13, 19});
    ImageCrop(&characterOnImage, (Rectangle){16, 0, 13, 19});
    Texture2D characterOff = LoadTextureFromImage(characterOffImage);
    Texture2D characterOn = LoadTextureFromImage(characterOnImage);

    // Background
    Texture2D background = LoadTexture("./bg.png");

    // PLANETS
    Image planetOneImage = LoadImage("./planet1.png");
    Image planetTwoImage = LoadImage("./planet2.png");
    Image planetThreeImage = LoadImage("./planet3.png");
    Image blackHoleImage = LoadImage("./blackHole.png");
    Image moonImage = LoadImage("./moon.png");
    ImageResizeNN(&planetOneImage, 480, 480);
    ImageResizeNN(&planetTwoImage, 480, 480);
    ImageResizeNN(&planetThreeImage, 480, 480);
    ImageResizeNN(&blackHoleImage, 960, 960);
    ImageResizeNN(&moonImage, 360, 360);
    Texture2D planetOneTexture = LoadTextureFromImage(planetOneImage);
    Texture2D planetTwoTexture = LoadTextureFromImage(planetTwoImage);
    Texture2D planetThreeTexture = LoadTextureFromImage(planetThreeImage);
    Texture2D blackHoleTexture = LoadTextureFromImage(blackHoleImage);
    Texture2D moonTexture = LoadTextureFromImage(moonImage);
    UnloadImage(planetOneImage);
    UnloadImage(planetTwoImage);
    UnloadImage(planetThreeImage);
    UnloadImage(blackHoleImage);
    UnloadImage(moonImage);

    // Explosion animation
    Image explosionImage = LoadImage("./explosion.png");
    Rectangle explosionFrame1 = {0, 0, 32, 32};
    Rectangle explosionFrame2 = {31, 0, 32, 32};
    Rectangle explosionFrame3 = {63, 0, 32, 32};
    Rectangle explosionFrame4 = {95, 0, 32, 32};
    Rectangle explosionFrame5 = {127, 0, 32, 32};
    Rectangle explosionFrame6 = {159, 0, 32, 32};
    Rectangle explosionFrame7 = {191, 0, 32, 32};
    Texture2D explosion = LoadTextureFromImage(explosionImage);
    UnloadImage(explosionImage);

    // GUNS
    Image defaultGunImage = LoadImage("./defaultGun.png");
    ImageCrop(&defaultGunImage, (Rectangle){1, 0, 13, 19});
    Texture2D defaultGunTexture = LoadTextureFromImage(defaultGunImage);

    // Font
    Font silver = LoadFontEx("./silver.ttf", 101, NULL, 0);

    // Main Camera
    Camera2D mainCamera = {
        {screenWidth/2, screenHeight/2}, // Offset
        {0, 0}, // Target
        0, // Rotation
        1 // Zoom
    };

    // Func variables
    float warpStart;
    float explosionStartTime = 0;
    bool isPressed = false;
    bool exploding = false;
    int itemSelected = 0; // 0=unpause, 1=mute, 2=reset, 3=quit
    int lastKey = 0;
    int explosionFrame = 0;
    float sens = 3;
    char sensStr[9];
    sprintf(sensStr, "%d", (int)sens);
    char nearestSideStr[9];
    bullet bullets[MAX_BULLETS];
    int gameSeed = GetRandomValue(0, 696969);
    planet nearestPlanet;
    Vector2 minimap = {screenWidth - 210, 210};
    float frameTime;
    int scene = 0; // 0 : Menu, 1 : gameplay
    int menuTick = 0;

    char xPosStr[16];
    char yPosStr[16];

    // PLAYER
    entity player = {
        {0, 0, 0}, // World position (x, y, rotation)
        0, // Rotation radians
        {0, 0}, // Velo
        0.8, // speed
        3, // health
        3, // max health
        0, // type (Player)
        true // visible
    };

    // Weapons
    weapon defaultGun = {
        true, // Unlocked
        1, // damage
        0.5, // Cooldown
        0, // when last fired
        23, // bullet speed
        10 // ammo
    };

    planet planets[PLANET_COUNT] = {
        {
            {GetRandomValue(-5000, 5000), GetRandomValue(-5000, 5000)}, // World Position
            480, // Size (Pixels)
            20, // Gravity
            planetOneTexture // Texture
        },
        {
            {GetRandomValue(-5000, 5000), GetRandomValue(-5000, 5000)}, // World Position
            480, // Size (Pixels)
            22, // Gravity
            planetTwoTexture // Texture
        },
        {
            {GetRandomValue(-5000, 5000), GetRandomValue(-5000, 5000)}, // World Position
            480, // Size (Pixels)
            23, // Gravity
            planetThreeTexture // Texture
        },
        {
            {GetRandomValue(-5000, 5000), GetRandomValue(-5000, 5000)}, // World Position
            960, // Size (Pixels)
            28, // Gravity
            blackHoleTexture // Texture
        },
        {
            {GetRandomValue(-5000, 5000), GetRandomValue(-5000, 5000)}, // World Position
            360, // Size (Pixels)
            17, // Gravity
            moonTexture // Texture
        }
    };

    // Button Boxes
    Rectangle menu = {50, 50, 525, 657};
    Rectangle unpauseButton = {85, 85, 437.5, 75};
    Rectangle pausedMuteButton = {85, 205, 287.5, 75};
    Rectangle pausedResetButton = {85, 325, 340, 75};
    Rectangle pausedQuitButton = {85, 565, 287.5, 75};
    Rectangle sensSlider = {85, 445, 380, 75};

    Rectangle menuPlayButton = {(screenWidth / 2) - (MeasureTextEx(silver, "< Start Game >", 150, 10).x / 2), screenHeight / 2, MeasureTextEx(silver, "< Start Game >", 150, 10).x, MeasureTextEx(silver, "< Start Game >", 150, 10).y / 2};

    // Game only runs when focused
    if(IsWindowFocused() == 0){
        gamePaused = true;
    }else{
        gamePaused = false;
    }

    // Main game loop
    while (!exitWindow)
    {   // Game Closing
        UpdateMusicStream(songs[songNum]);
        if(!IsMusicStreamPlaying(songs[songNum])){
            songNum++;
        }
        if(songNum > 3){
            songNum = 0;
        }
    	if (WindowShouldClose()){
            exitWindow = true;
        }

        frameTime = GetFrameTime() * 59;

        // Open/close pause menu
        if(scene == 1){
            if(IsKeyPressed(KEY_ESCAPE) == 1){
                if(gamePaused == 0){
                    gamePaused = 1;
                    ShowCursor();
                }else{
                    gamePaused = 0;
                    HideCursor();
                }
            }
        }

                                    // -- Gameplay logic --
        // While game active
        if(gamePaused == false && scene == 1){
            // While the ship is alive
            if(exploding == false){

                // Speed damping w/ minimum of 1
                if(fabs(player.velo.x) > 1){
                    player.velo.x = player.velo.x * frameTime * 0.94;
                }
                if(fabs(player.velo.y) > 1){
                    player.velo.y = player.velo.y * frameTime * 0.94;
                }

                // Keyboard Input
                if(IsKeyDown(KEY_UP) | IsKeyDown(KEY_W)){
                    player.rotationRadians = toRadians(player.worldPos.z - 90);
                    player.velo.x += cos(player.rotationRadians) * player.speed;
                    player.velo.y += sin(player.rotationRadians) * player.speed;
                }
                if(IsKeyDown(KEY_LEFT) | IsKeyDown(KEY_A)){
                    player.worldPos.z -= sens * 1.5 * frameTime;
                }
                if(IsKeyDown(KEY_RIGHT) | IsKeyDown(KEY_D)){
                    player.worldPos.z += sens * 1.5 * frameTime;
                }
                if(IsKeyDown(KEY_R)){
                    player.health = 0;
                }
                if(IsKeyDown(KEY_Q) && GetTime() - warpStart >= 5){
                    PlaySound(warpSound);
                    player.rotationRadians = toRadians(player.worldPos.z - 90);
                    player.velo.x += cos(player.rotationRadians) * player.speed * 49;
                    player.velo.y += sin(player.rotationRadians) * player.speed * 49;
                    warpStart = GetTime();
                }
                // Spawn Bullet & init
                if(IsKeyDown(KEY_SPACE) | IsKeyDown(KEY_E)){
                    if(defaultGun.ammo > 0 && GetTime() - defaultGun.fired > defaultGun.cooldown){
                        for (int i = 0; i < MAX_BULLETS; i++) {
                            if (bullets[i].lifetime <= 0) {
                                PlaySound(shootSound);
                                defaultGun.ammo--;
                                bullets[i].worldPos.x = player.worldPos.x;
                                bullets[i].worldPos.y = player.worldPos.y;
                                bullets[i].damage = defaultGun.damage;
                                bullets[i].speed = defaultGun.bulletSpeed;
                                player.rotationRadians = toRadians(player.worldPos.z - 90);
                                bullets[i].velo.x = cos(player.rotationRadians) * bullets[i].speed;
                                bullets[i].velo.y = sin(player.rotationRadians) * bullets[i].speed;
                                bullets[i].lifetime = 180; // Lifetime frames
                                defaultGun.fired = GetTime();
                                break;
                            }
                        }
                    }
                }
            }

            // Planet Collision
            for(int i = 0; i < sizeof(planets) / sizeof(planets[0]); i++){
                if(CheckCollisionCircles((Vector2){planets[i].worldPos.x + planets[i].size / 2, planets[i].worldPos.y + planets[i].size / 2}, planets[i].size / 2, (Vector2){player.worldPos.x, player.worldPos.y}, 21) && !exploding){
                    player.health = 0;
                }
            }

            // Planet gravity + Moving Math + Cam follow
            if(!exploding){
                nearestPlanet = getNearestPlanet(planets, (Vector2){player.worldPos.x, player.worldPos.y});
                if(Vector2Distance((Vector2){player.worldPos.x, player.worldPos.y}, (Vector2){nearestPlanet.worldPos.x + nearestPlanet.size / 2, nearestPlanet.worldPos.y + nearestPlanet.size / 2}) < nearestPlanet.gravity * 65){
                    player.velo = Vector2Add(
                        Vector2Scale(
                            Vector2Normalize(
                                Vector2Subtract(
                                    (Vector2){nearestPlanet.worldPos.x + nearestPlanet.size / 2, nearestPlanet.worldPos.y + nearestPlanet.size / 2}, (Vector2){player.worldPos.x, player.worldPos.y}
                                )
                            ), GetFrameTime() * nearestPlanet.gravity * Normalize(-Vector2Distance((Vector2){player.worldPos.x, player.worldPos.y}, (Vector2){nearestPlanet.worldPos.x + nearestPlanet.size / 2, nearestPlanet.worldPos.y + nearestPlanet.size / 2}) + nearestPlanet.gravity * 105, 100, nearestPlanet.gravity * 75)
                        ), player.velo
                    );
                }

                player.worldPos.x += player.velo.x * frameTime;
                player.worldPos.y += player.velo.y * frameTime;
                
                mainCamera.target = (Vector2){player.worldPos.x, player.worldPos.y};
            }
            
            // Explosion Animation
            if(explosionStartTime == 0 && exploding){
                explosionStartTime = GetTime();
            }
            if(explosionStartTime - GetTime() <= GetFrameTime() && exploding){
                explosionFrame++;
                if(explosionFrame > 70){
                    explosionFrame = 0;
                    explosionStartTime = 0;
                    exploding = false;
                    player.worldPos.x = 0;
                    player.worldPos.y = 0;
                    player.worldPos.z = 0;
                    player.velo.x = 0;
                    player.velo.y = 0;
                    player.health = player.maxHealth;
                    defaultGun.ammo = 10;
                    explosionFrame = 0;
                    explosionStartTime = 0;
                    ShowCursor();
                    scene = 0;
                    gamePaused = true;
                }
            }

            if(player.health <= 0){
                    PlaySound(explosionSound);
                    exploding = true;
                    explosionFrame = 1;
                    player.health = player.maxHealth;
            }
            if(player.health > player.maxHealth){
                player.health = player.maxHealth;
            }

            // Bullet moving & Stuff
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bullets[i].lifetime > 0) {
                    bullets[i].worldPos.x += bullets[i].velo.x * frameTime;
                    bullets[i].worldPos.y += bullets[i].velo.y * frameTime;
                    bullets[i].lifetime -= 1;
                    for(int j = 0; j < sizeof(planets) / sizeof(planets[0]); j++){
                        if(CheckCollisionPointCircle((Vector2){bullets[i].worldPos.x, bullets[i].worldPos.y}, (Vector2){planets[j].worldPos.x + planets[j].size / 2, planets[j].worldPos.y + planets[j].size / 2}, planets[j].size / 2)){
                            bullets[i].lifetime = 0;
                        }
                    }
                }
            }

            // UI Pos
            sprintf(xPosStr, "%d", (int)player.worldPos.x);
            sprintf(yPosStr, "%d", (int)player.worldPos.y);

        }
        if(gamePaused && scene == 1){
            // Game is paused

            // Menu KB input
            lastKey = GetKeyPressed();
            switch(lastKey){
                case KEY_UP:
                    itemSelected--;
                    PlaySound(buttonSound);
                    break;
                case KEY_DOWN:
                    itemSelected++;
                    PlaySound(buttonSound);
                    break;
                case KEY_LEFT:
                    if(itemSelected == 3){
                        if(sens > 1){
                            sens--;
                            sprintf(sensStr, "%d", (int)sens);
                        }
                    }
                    break;
                case KEY_RIGHT:
                    if(itemSelected == 3){
                        if(sens < 7){
                            sens++;
                            sprintf(sensStr, "%d", (int)sens);
                        }
                    }
                    break;
            }

            // Menu Mouse Input
            itemSelected = menuMouseSelect(GetMousePosition(), unpauseButton, pausedMuteButton, pausedResetButton, sensSlider, pausedQuitButton, itemSelected, buttonSound);

            if(itemSelected > 4){
                itemSelected = 0;
            }else if(itemSelected < 0){
                itemSelected = 4;
            }

            // Selecting input
            if(IsMouseButtonPressed(0) == 1 || IsKeyPressed(KEY_ENTER) == 1){
                switch(itemSelected){
                    case 0:
                        gamePaused = 0;
                        HideCursor();
                        break;
                    case 1:
                        if(GetMasterVolume() == 0){
                            SetMasterVolume(0.7);
                            pausedMuteButton.width = 287.5;
                        }else{
                            SetMasterVolume(0);
                            pausedMuteButton.width = 385;
                        }
                        break;
                    case 2:
                        if(exploding == false){
                            PlaySound(explosionSound);
                            exploding = true;
                            explosionFrame = 1;
                        }
                        gamePaused = 0;
                        HideCursor();
                        break;
                    case 3:
                        if(sens < 7){
                            sens++;
                        }else{
                            sens = 1;
                        }
                        sprintf(sensStr, "%d", (int)sens);
                        break;
                    case 4:
                        exitWindow = true;
                        break;
                }
            }
        }
        if(scene == 0){
            if(IsKeyDown(KEY_P)){
                scene = 1;
                gamePaused = false;
                HideCursor();
            }
        }

        BeginDrawing();

        if(scene == 1){
            BeginMode2D(mainCamera);

                // Background
                ClearBackground(RAYWHITE);
                DrawTexturePro(background, (Rectangle){0, 0, 482, 321}, (Rectangle){roundNumber(player.worldPos.x, screenWidth), roundNumber(player.worldPos.y, screenHeight), screenWidth, screenHeight}, (Vector2){0, 0}, 0, WHITE);
                DrawTexturePro(background, (Rectangle){0, 0, 482, 321}, (Rectangle){roundNumber(player.worldPos.x, screenWidth) - screenWidth, roundNumber(player.worldPos.y, screenHeight), screenWidth, screenHeight}, (Vector2){0, 0}, 0, WHITE);
                DrawTexturePro(background, (Rectangle){0, 0, 482, 321}, (Rectangle){roundNumber(player.worldPos.x, screenWidth) - screenWidth, roundNumber(player.worldPos.y, screenHeight) - screenHeight, screenWidth, screenHeight}, (Vector2){0, 0}, 0, WHITE);
                DrawTexturePro(background, (Rectangle){0, 0, 482, 321}, (Rectangle){roundNumber(player.worldPos.x, screenWidth), roundNumber(player.worldPos.y, screenHeight) - screenHeight, screenWidth, screenHeight}, (Vector2){0, 0}, 0, WHITE);

                for(int i = 0; i < sizeof(planets) / sizeof(planets[0]); i++){
                    DrawTexture(planets[i].texture, planets[i].worldPos.x, planets[i].worldPos.y, WHITE);
                }

                EndMode2D();


                // If Game active
                if(gamePaused == 0){
                BeginMode2D(mainCamera);
                    // Character
                    if(exploding == 0){
                        if(IsKeyDown(KEY_UP) | IsKeyDown(KEY_W)){   // set origin to half of texture size; set destination rect to center x/y
                            DrawTexturePro(characterOn, (Rectangle){0, 0, 13, 19}, (Rectangle){player.worldPos.x, player.worldPos.y, 39, 57}, (Vector2){19.5, 28.5}, player.worldPos.z, WHITE);
                            DrawTexturePro(defaultGunTexture, (Rectangle){0, 0, 13, 19}, (Rectangle){player.worldPos.x + 3, player.worldPos.y, 39, 57}, (Vector2){19.5, 28.5}, player.worldPos.z, WHITE);
                        }else{
                            DrawTexturePro(characterOff, (Rectangle){0, 0, 13, 19}, (Rectangle){player.worldPos.x, player.worldPos.y, 39, 57}, (Vector2){19.5, 28.5}, player.worldPos.z, WHITE);
                            DrawTexturePro(defaultGunTexture, (Rectangle){0, 0, 13, 19}, (Rectangle){player.worldPos.x + 3, player.worldPos.y, 39, 57}, (Vector2){19.5, 28.5}, player.worldPos.z, WHITE);
                        }
                    }else{
                        switch(explosionFrame){
                            case 1 ... 10:
                                DrawTexturePro(explosion, explosionFrame1, (Rectangle){player.worldPos.x, player.worldPos.y, 64, 64}, (Vector2){32, 32}, player.worldPos.z, WHITE);
                                break;
                            case 11 ... 20:
                                DrawTexturePro(explosion, explosionFrame2, (Rectangle){player.worldPos.x, player.worldPos.y, 64, 64}, (Vector2){32, 32}, player.worldPos.z, WHITE);
                                break;
                            case 21 ... 30:
                                DrawTexturePro(explosion, explosionFrame3, (Rectangle){player.worldPos.x, player.worldPos.y, 64, 64}, (Vector2){32, 32}, player.worldPos.z, WHITE);
                                break;
                            case 31 ... 40:
                                DrawTexturePro(explosion, explosionFrame4, (Rectangle){player.worldPos.x, player.worldPos.y, 64, 64}, (Vector2){32, 32}, player.worldPos.z, WHITE);
                                break;
                            case 41 ... 50:
                                DrawTexturePro(explosion, explosionFrame5, (Rectangle){player.worldPos.x, player.worldPos.y, 64, 64}, (Vector2){32, 32}, player.worldPos.z, WHITE);
                                break;
                            case 51 ... 60:
                                DrawTexturePro(explosion, explosionFrame6, (Rectangle){player.worldPos.x, player.worldPos.y, 64, 64}, (Vector2){32, 32}, player.worldPos.z, WHITE);
                                break;
                            case 61 ... 70:
                                DrawTexturePro(explosion, explosionFrame7, (Rectangle){player.worldPos.x, player.worldPos.y, 64, 64}, (Vector2){32, 32}, player.worldPos.z, WHITE);
                                break;
                        }
                    }
                    // Bullets
                    for (int i = 0; i < MAX_BULLETS; i++) {
                        if (bullets[i].lifetime > 0) {
                            DrawCircleGradient(bullets[i].worldPos.x + 2, bullets[i].worldPos.y + 2, 10, WHITE, BLANK);
                            DrawRectangle(bullets[i].worldPos.x, bullets[i].worldPos.y, 4, 4, YELLOW);
                        }
                    }

                EndMode2D();

                // In Game HUD
                // Background panel
                DrawRectangleRounded((Rectangle){0, screenHeight - 100, screenWidth, 200}, 0.3, 7, TRANSPARENTGRAY);

                // Minimap
                DrawCircle(minimap.x, minimap.y, 180, TRANSPARENTGRAY);
                for(int i = 0; i < sizeof(planets) / sizeof(planets[0]); i++){
                    DrawCircle((minimap.x + (planets[i].worldPos.x + planets[i].size / 2) / 40), (minimap.y + (planets[i].worldPos.y + planets[i].size / 2) / 40), 4, LIME);
                }
                DrawCircle(minimap.x + (player.worldPos.x) / 40, minimap.y + (player.worldPos.y) / 40, 3, RAYWHITE);


                // Ammo Count
                DrawRectangleRounded((Rectangle){20, screenHeight - 80, 310, 60}, 0.4, 7, GRAY);
                for(int i = 30; i <= defaultGun.ammo * 30; i += 30){
                    if(defaultGun.ammo > i / 30 - 1){
                        DrawRectangleRounded((Rectangle){i, screenHeight - 70, 20, 40}, 0.35, 5, YELLOW);
                    }
                }

                // Health
                // Max units : player.maxHealth
                // Drawn Units : player.health
                // width of unit : HEALTH_WIDTH/player.maxHealth
                // width of bar : HEALTH_WIDTH
                DrawRectangleRounded((Rectangle){screenWidth - ((HEALTH_WIDTH / player.maxHealth) * 3) - 40, screenHeight - 80, HEALTH_WIDTH + 20, 60}, 0.4, 7, GRAY);
                DrawRectangleRounded((Rectangle){screenWidth - ((HEALTH_WIDTH / player.maxHealth) * player.health) - 30, screenHeight - 70, HEALTH_WIDTH / player.maxHealth * player.health, 40}, 0.4, 7, RED);

                // Warp Cooldown
                DrawRectangleRounded((Rectangle){screenWidth / 2 - 260, screenHeight - 80, 520, 60}, 0.4, 7, GRAY);
                if(GetTime() - warpStart < 5){
                    DrawRectangleRounded((Rectangle){screenWidth / 2 - 250, screenHeight - 70, (GetTime() - warpStart) * 100, 40}, 0.4, 7, GOLD);
                }else{
                    DrawRectangleRounded((Rectangle){screenWidth / 2 - 250, screenHeight - 70, 500, 40}, 0.4, 7, DARKGREEN);
                }

                // Coords
                DrawTextEx(silver, "X:", (Vector2){30, 40}, 60, 4, GREEN);
                DrawTextEx(silver, xPosStr, (Vector2){70, 40}, 60, 4, GREEN);
                DrawTextEx(silver, "Y:", (Vector2){30, 80}, 60, 4, GREEN);
                DrawTextEx(silver, yPosStr, (Vector2){70, 80}, 60, 4, GREEN);

                }else{ // Draw Menu
                    DrawRectangleRounded(menu, 0.1, 7, TRANSPARENTGRAY);
                    if(itemSelected == 0){
                        DrawRectangleRec(unpauseButton, TRANSPARENTBLUE);
                    }
                    DrawTextEx(silver, "[Unpause]", (Vector2){unpauseButton.x, unpauseButton.y - 15}, 150, 10, RAYWHITE);
                    if(itemSelected == 1){
                        DrawRectangleRec(pausedMuteButton, TRANSPARENTBLUE);
                    }
                    if(GetMasterVolume() == 0){
                        DrawTextEx(silver, "[Unmute]", (Vector2){pausedMuteButton.x, pausedMuteButton.y - 15}, 150, 10, RAYWHITE);
                    }else{
                        DrawTextEx(silver, "[Mute]", (Vector2){pausedMuteButton.x, pausedMuteButton.y - 15}, 150, 10, RAYWHITE);
                    }
                    if(itemSelected == 2){
                        DrawRectangleRec(pausedResetButton, TRANSPARENTBLUE);
                    }
                    DrawTextEx(silver, "[Reset]", (Vector2){pausedResetButton.x, pausedResetButton.y - 15}, 150, 10, RAYWHITE);
                    if(itemSelected == 3){
                        DrawRectangleRec(sensSlider, TRANSPARENTBLUE);
                    }
                    DrawTextEx(silver, "[Sens: ]", (Vector2){sensSlider.x, sensSlider.y - 15}, 150, 10, RAYWHITE);
                    DrawTextEx(silver, sensStr, (Vector2){sensSlider.x + 300, sensSlider.y - 15}, 150, 10, RAYWHITE);
                    if(itemSelected == 4){
                        DrawRectangleRec(pausedQuitButton, TRANSPARENTBLUE);
                    }
                    DrawTextEx(silver, "[Quit]", (Vector2){pausedQuitButton.x, pausedQuitButton.y - 15}, 150, 10, RAYWHITE);
                }

            	// DEBUG
                DrawFPS(10, 10);
        }else{
            DrawTexturePro(background, (Rectangle){0, 0, 482, 321}, (Rectangle){menuTick, 0, screenWidth, screenHeight}, (Vector2){0, 0}, 0, WHITE);
            DrawTexturePro(background, (Rectangle){0, 0, 482, 321}, (Rectangle){menuTick + screenWidth, 0, screenWidth, screenHeight}, (Vector2){0, 0}, 0, WHITE);
            
            DrawTextEx(silver, "< Start Game >", (Vector2){menuPlayButton.x - 2, menuPlayButton.y - 13}, 150, 10, BLUE);
            DrawTextEx(silver, "< Start Game >", (Vector2){menuPlayButton.x, menuPlayButton.y - 15}, 150, 10, RAYWHITE);

            menuTick--;
            if(menuTick <= -screenWidth){
                menuTick = 0;
            }
        }


        EndDrawing();
	}

    // CLEAN UP
    UnloadImage(characterOnImage);
    UnloadImage(characterOffImage);
    UnloadImage(defaultGunImage);
    UnloadMusicStream(songs[0]);
    UnloadMusicStream(songs[1]);
    UnloadMusicStream(songs[2]);
    UnloadMusicStream(songs[3]);
    UnloadTexture(characterOn);
    UnloadTexture(characterOff);
    UnloadTexture(defaultGunTexture);
    UnloadTexture(background);
    UnloadTexture(explosion);
    UnloadSound(explosionSound);
    UnloadSound(buttonSound);
	CloseAudioDevice();
    UnloadFont(silver);
    CloseWindow();

    return 0;
}