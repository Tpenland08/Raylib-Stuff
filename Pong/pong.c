
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"

#define SPECIALCOLOR (Color){ 255, 0, 255, 155 }

// Set true to close game
bool exitWindow = false;

// Convert degrees to radians
float toRadians(float degrees){
    return(degrees * (M_PI / 180.0));
}

// Get side of screen/nearest paddle
int nearestPaddle(Vector2 ballPos, int screenWidth){
    // Case paddle1
    if(ballPos.x < (float)screenWidth / 2){
        return(1);
    //Case paddle2
    }else{
        return(2);
    }
}

// Reflect an angle
float reflect(float angleOfIncidence) {
    float angleOfReflection = 360 - angleOfIncidence;
    return angleOfReflection;
}

// Bounce off paddle
float bounce(Vector2 ballPos, float p1Y, float p2Y, int screenWidth){
    float outValue = 0.0;
    float distance = 0.0;
    // Case paddle1
    if(nearestPaddle(ballPos, (float)screenWidth) == 1){
        outValue = 0;
        distance = Vector2Distance(ballPos, (Vector2){31, p1Y});
        if(ballPos.y - p1Y <= 0){
            outValue = outValue + (distance / 1.2);
        }else{
            outValue = outValue - (distance / 1.2);
        }
    // Case paddle2
    }else{
        outValue = 180;
        distance = Vector2Distance(ballPos, (Vector2){(float)screenWidth - 31, p2Y});
        if(ballPos.y - p2Y <= 0){
            outValue = outValue - (distance / 1.2);
        }else{
            outValue = outValue + (distance / 1.2);
        }
    }
    return(outValue);
}


int main(void)
{

    InitWindow(0, 0, "Pong");
    ToggleFullscreen();
    const int screenWidth = GetMonitorWidth(GetCurrentMonitor());
    const int screenHeight = GetMonitorHeight(GetCurrentMonitor());
    InitAudioDevice();
    SetMasterVolume(1.0);
    SetTargetFPS(60);

    ChangeDirectory(GetApplicationDirectory());
    Sound gameOverSound = LoadSound("./assets/gameOver.wav");
    Sound hitSound = LoadSound("./assets/hit.wav");
    Sound buttonHoverSound = LoadSound("./assets/buttonHover.wav");
    Image checkmarkImage = LoadImage("./assets/checkmark.png");
    ImageResize(&checkmarkImage, 80, 80); 
    Texture2D checkmark = LoadTextureFromImage(checkmarkImage);

    Font silver = LoadFont("./assets/Silver.ttf");
    FILE *configPtr;
    if(!FileExists("./assets/.pongConf")){
        configPtr = fopen("./assets/.pongConf", "w");
        fprintf(configPtr, "showFPS=1;\n");
        fprintf(configPtr, "audio=1;\n");
        fclose(configPtr); 
   	}

   int scene = 0; // 0 = menu, 1 = settings, 2 = gameplay
   int opponent = 0; // 0 = human, 1 = computer
   int score = 0;
   float p1Y = (float)screenHeight/2;
   float p2Y = (float)screenHeight/2;
   Vector2 ballPos = {(float)screenWidth / 2, (float)screenHeight / 2};
   Vector2 ballMove = {0, 0};
   float directionDegrees = 180.0; // 0 is right, down is 90, ETC...
   float directionRadians;
   float directionX;
   float directionY;
   float speed = 15.0;
   double lastGameOver = -2;
   char *settings = LoadFileText("./assets/.pongConf");
   char *scoreStr = (char*)malloc(4 * sizeof(char));
   char *scoreStr2 = (char*)malloc(4 * sizeof(char));
   bool isPressed = false;

    Rectangle button1 = {screenWidth/2, screenHeight/2 - 80, 350, 65};
    Rectangle button2 = {screenWidth/2, screenHeight/2, 460, 65};
    Rectangle button3 = {screenWidth/2, screenHeight/2 + 80, 285, 65};
    Rectangle button4 = {screenWidth/2, screenHeight/2 + 160, 135, 65};
    Rectangle button5 = {40, 40, 210, 65};
    Rectangle button6 = {screenWidth - 220, screenHeight - 105, 180, 65};
    Rectangle textbox1 = {screenWidth/11, screenHeight/3, 700, 65};
    Rectangle textbox2 = {screenWidth/11*2, screenHeight/3*2, 480, 65};
    
    Rectangle paddle1 = {10, p1Y - 50, 10, 100};
    Rectangle paddle2 = {screenWidth - 20, p2Y - 50, 10, 100};


    // Main game loop
    while (!exitWindow)
    {
    	if (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose()) exitWindow = true;

    	// Main Menu
    	if(scene == 0){
    		// Play Human Button pressed
            if(IsMouseButtonDown(0) == 1 && CheckCollisionPointRec(GetMousePosition(), button1) == 1){
                if(settings[(TextFindIndex(settings, "audio=") + 6)] == '1'){PlaySound(buttonHoverSound);}
                scene = 2;
                opponent = 0;
            }
            // Play Computer Button pressed
            if(IsMouseButtonDown(0) == 1 && CheckCollisionPointRec(GetMousePosition(), button2) == 1){
                if(settings[(TextFindIndex(settings, "audio=") + 6)] == '1'){PlaySound(buttonHoverSound);}
                scene = 2;
                opponent = 1;
            }
            // Settings button pressed
            if(IsMouseButtonDown(0) == 1 && CheckCollisionPointRec(GetMousePosition(), button3) == 1){
                settings = LoadFileText("./assets/.pongConf");
                if(settings[(TextFindIndex(settings, "audio=") + 6)] == '1'){PlaySound(buttonHoverSound);}
                scene = 1;
            }
            // Quit button pressed
            if(IsMouseButtonDown(0) == 1 && CheckCollisionPointRec(GetMousePosition(), button4) == 1){
                exitWindow = true;
            }
    	}

    	// Settings
        if(scene == 1){
            // Back Button
            if(IsMouseButtonDown(0) == 1 && CheckCollisionPointRec(GetMousePosition(), button5) == 1){
                if(settings[(TextFindIndex(settings, "audio=") + 6)] == '1'){PlaySound(buttonHoverSound);}
                settings = LoadFileText("./assets/.pongConf");
                scene = 0;
            }
            // Apply Button
            if(IsMouseButtonDown(0) == 1 && CheckCollisionPointRec(GetMousePosition(), button6) == 1){
                if(settings[(TextFindIndex(settings, "audio=") + 6)] == '1'){PlaySound(buttonHoverSound);}
                    remove("./assets/.pongConf");
                    configPtr = fopen("./assets/.pongConf", "w");
                    fprintf(configPtr, "showFPS=");
                    fprintf(configPtr, "%c;\n", settings[(TextFindIndex(settings, "showFPS=") + 8)]);
                    fprintf(configPtr, "audio=");
                    fprintf(configPtr, "%c;\n", settings[(TextFindIndex(settings, "audio=") + 6)]);
                    fclose(configPtr); 
                scene = 0;
            }
            // FPS check
            if(IsMouseButtonDown(0) == 1 && CheckCollisionPointRec(GetMousePosition(), (Rectangle){screenWidth/2, screenHeight/3, 80, 80}) == 1 && isPressed == false){
                if(settings[(TextFindIndex(settings, "audio=") + 6)] == '1'){PlaySound(buttonHoverSound);}
                if(settings[(TextFindIndex(settings, "showFPS=") + 8)] == '1'){
                    settings[(TextFindIndex(settings, "showFPS=") + 8)] = '0';
                }else{
                    settings[(TextFindIndex(settings, "showFPS=") + 8)] = '1';
                }
                isPressed = true;
            }
            // Audio check
            if(IsMouseButtonDown(0) == 1 && CheckCollisionPointRec(GetMousePosition(), (Rectangle){screenWidth/2, screenHeight/3*2, 80, 80}) == 1 && isPressed == false){
                if(settings[(TextFindIndex(settings, "audio=") + 6)] == '1'){PlaySound(buttonHoverSound);}
                if(settings[(TextFindIndex(settings, "audio=") + 6)] == '1'){
                    settings[(TextFindIndex(settings, "audio=") + 6)] = '0';
                }else{
                    settings[(TextFindIndex(settings, "audio=") + 6)] = '1';
                }
                isPressed = true;
            }
            if(IsMouseButtonDown(0) == 0){
                isPressed = false;
            }
        }

        // Gameplay
        if(scene == 2){
        	if(ballPos.x <= 30 && ballPos.y >= p1Y - 60 && ballPos.y <= p1Y + 60){
                ballPos.x = 31;
                if(settings[(TextFindIndex(settings, "audio=") + 6)] == '1'){PlaySound(hitSound);}
                directionDegrees = bounce(ballPos, p1Y, p2Y, screenWidth);
                score += 1;
            }else if(ballPos.x >= screenWidth - 30 && ballPos.y >= p2Y - 60 && ballPos.y <= p2Y + 60){
                ballPos.x = (float)screenWidth - 31;
                if(settings[(TextFindIndex(settings, "audio=") + 6)] == '1'){PlaySound(hitSound);}
                directionDegrees = bounce(ballPos, p1Y, p2Y, screenWidth);
                score += 1;
            }else{
                // Bounce ball off roof/floor
                if(ballPos.y <= 10 | ballPos.y >= screenHeight - 10){
                    if(settings[(TextFindIndex(settings, "audio=") + 6)] == '1'){PlaySound(hitSound);}
                    directionDegrees = reflect(directionDegrees);
                    if(ballPos.y > screenHeight/2){
                        ballPos.y = screenHeight - 11;
                    }else{
                        ballPos.y = 11;
                    }
                }
                // Movement math
                directionRadians = toRadians(directionDegrees);
                directionX = cos(directionRadians);
                directionY = sin(directionRadians);
                ballMove = (Vector2){(directionX * speed), (directionY * speed)};
                ballPos = Vector2Add(ballPos, ballMove);
            }
            // Input
            if(opponent == 0){
                if(IsKeyDown(KEY_UP) == 1 && p2Y > 50){
                    p2Y -= 10;
                }else if(IsKeyDown(KEY_DOWN) == 1 && p2Y < screenHeight - 50){
                    p2Y += 10;
                }
                if(IsKeyDown(KEY_W) == 1 && p1Y > 50){
                    p1Y -= 10;
                }else if(IsKeyDown(KEY_S) == 1 && p1Y < screenHeight - 50){
                    p1Y += 10;
                }
            }else{
                if(IsKeyDown(KEY_UP) == 1 && p1Y > 50){
                    p1Y -= 10;
                }else if(IsKeyDown(KEY_DOWN) == 1 && p1Y < screenHeight - 50){
                    p1Y += 10;
                }
                if(IsKeyDown(KEY_W) == 1 && p1Y > 50){
                    p1Y -= 10;
                }else if(IsKeyDown(KEY_S) == 1 && p1Y < screenHeight - 50){
                    p1Y += 10;
                }
                // Computer control
                if(ballPos.y > p2Y && p2Y < screenHeight - 50){
                    p2Y += 7;
                }
                if(ballPos.y < p2Y && p2Y > 50){
                    p2Y -= 7;
                }
            }

            // Check for gameover
            if(ballPos.x >= screenWidth - 10 | ballPos.x <= 10){
                if(settings[(TextFindIndex(settings, "audio=") + 6)] == '1'){PlaySound(gameOverSound);}
                scene = 0;
                ballPos = (Vector2){screenWidth/2, screenHeight/2};
                directionDegrees = 180;
                p1Y = screenHeight/2;
                p2Y = screenHeight/2;
                score = 0;
                lastGameOver = GetTime();
            }
            sprintf(scoreStr, "%d", score);
        }

        BeginDrawing();

        	// Menu
        	if(scene == 0){
                ClearBackground((Color){0, 20, 41, 255}); // Deep blue
                DrawRectangleRec(button1, SPECIALCOLOR);
                DrawRectangleRec(button2, SPECIALCOLOR);
                DrawRectangleRec(button3, SPECIALCOLOR);
                DrawRectangleRec(button4, SPECIALCOLOR);
                DrawTextEx(silver, "Play Human", (Vector2){screenWidth/2, screenHeight/2 - 80}, 120, 4, RAYWHITE);
                DrawTextEx(silver, "Play Computer", (Vector2){screenWidth/2, screenHeight/2}, 120, 4, RAYWHITE);
                DrawTextEx(silver, "Settings", (Vector2){screenWidth/2, screenHeight/2 + 80}, 120, 4, RAYWHITE);
                DrawTextEx(silver, "Quit", (Vector2){screenWidth/2, screenHeight/2 + 160}, 120, 4, RAYWHITE);

                if(GetTime() - lastGameOver < 2){
                    DrawTextEx(silver, "Game Over", (Vector2){((float)screenWidth / 2) - ((float)screenWidth * 0.0625), 20}, 120, 4, RED);
                }
                if(settings[(TextFindIndex(settings, "showFPS=") + 8)] == '1'){DrawFPS(10, 10);}

            }

            // Settings menu
            if(scene == 1){
                ClearBackground((Color){0, 20, 41, 255}); // Deep blue
                // Back button
                DrawRectangleRec(button5, SPECIALCOLOR);
                DrawTextEx(silver, "< Back", (Vector2){40, 40}, 120, 4, RAYWHITE);
                // Apply button
                DrawRectangleRec(button6, SPECIALCOLOR);
                DrawTextEx(silver, "Apply", (Vector2){screenWidth - 220, screenHeight - 105}, 120, 4, RAYWHITE);
                // FPS Option
                DrawRectangleRec(textbox1, SPECIALCOLOR);
                DrawTextEx(silver, "Draw FPS Counter  -", (Vector2){screenWidth/11, screenHeight/3}, 120, 4, RAYWHITE);
                DrawRectangle(screenWidth/2, screenHeight/3, 80, 80, DARKPURPLE);
                if(settings[(TextFindIndex(settings, "showFPS=") + 8)] == '1'){
                    DrawTexture(checkmark, screenWidth/2, screenHeight/3, GREEN);
                }
                // Audio option
                DrawRectangleRec(textbox2, SPECIALCOLOR);
                DrawTextEx(silver, "Play Audio  -", (Vector2){(screenWidth/11)*2, (screenHeight/3)*2}, 120, 4, RAYWHITE);
                DrawRectangle(screenWidth/2, screenHeight/3*2, 80, 80, DARKPURPLE);
                if(settings[(TextFindIndex(settings, "audio=") + 6)] == '1'){
                    DrawTexture(checkmark, screenWidth/2, screenHeight/3*2, GREEN);
                }
                if(settings[(TextFindIndex(settings, "showFPS=") + 8)] == '1'){DrawFPS(10, 10);}
            }

            // Gameplay
            if(scene == 2){
            	ClearBackground((Color){0, 20, 41, 255}); // Deep blue
                DrawTextEx(silver, scoreStr, (Vector2){(screenWidth / 2), 10}, 120, 4, RAYWHITE);
                DrawCircleV(ballPos, 10, RAYWHITE);  
                DrawRectangle(10, p1Y - 50, 10, 100, RAYWHITE);
                DrawRectangle(screenWidth - 20, p2Y - 50, 10, 100, RAYWHITE);
                DrawLine(screenWidth/2, 100, screenWidth/2, screenHeight, RAYWHITE);
                if(settings[(TextFindIndex(settings, "showFPS=") + 8)] == '1'){DrawFPS(10, 10);}
            }
        EndDrawing();
	}

    free(settings);
    free(scoreStr);
    free(scoreStr2);
	CloseAudioDevice();
    UnloadFont(silver);
    UnloadSound(gameOverSound);
    UnloadSound(hitSound);
    UnloadSound(buttonHoverSound);
    CloseWindow();

    return 0;
}