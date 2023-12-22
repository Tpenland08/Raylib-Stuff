
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"


// Set true to close game
bool exitWindow = false;

// Convert degrees to radians
float toRadians(float degrees){
    return(degrees * (M_PI / 180.0));
}

// Get side of screen/nearest paddle
int nearestPaddle(Vector2 ballPos, float screenWidthF){
    // Case paddle1
    if(ballPos.x < screenWidthF / 2){
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
float bounce(Vector2 ballPos, float p1Y, float p2Y, float screenWidthF){
    float outValue = 0.0;
    float distance = 0.0;
    // Case paddle1
    if(nearestPaddle(ballPos, screenWidthF) == 1){
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
        distance = Vector2Distance(ballPos, (Vector2){screenWidthF - 31, p2Y});
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

    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();
    const float screenWidthF = (float)screenWidth;
    const float screenHeightF = (float)screenWidth;
    InitWindow(screenWidth, screenHeight, "Pong");
    InitAudioDevice();
    SetMasterVolume(1.0);
    ToggleFullscreen();
    SetTargetFPS(60);

        ChangeDirectory(GetApplicationDirectory());
    Sound gameOverSound = LoadSound("./assets/gameOver.wav");
    Sound hitSound = LoadSound("./assets/hit.wav");
    Sound buttonHoverSound = LoadSound("./assets/buttonHover.wav");

    Font silver = LoadFont("./assets/Silver.ttf");
    FILE *configPtr;
    if(!FileExists("./assets/.pongConf")){
        configPtr = fopen("./.pongConf", "w");
        fprintf(configPtr, "showFPS=1;\n");
        fprintf(configPtr, "audio=1;\n");
        fclose(configPtr); 
   	}

   int scene = 0; // 0 = menu, 1 = settings, 2 = gameplay
   int opponent = 0; // 0 = human, 1 = computer
   int score = 0;
   float p1Y = 600.0;
   float p2Y = 600.0;
   Vector2 ballPos = {screenWidthF / 2, screenHeightF / 2};
   Vector2 ballMove = {0, 0};
   float directionDegrees = 180.0; // 0 is right, down is 90, ETC...
   float directionRadians;
   float directionX;
   float directionY;
   float speed = 15.0;
   double lastGameOver = 1;
   char *settings = LoadFileText("./assets/.pongConf");
   char *scoreStr = (char*)malloc(4 * sizeof(char));
   char *scoreStr2 = (char*)malloc(4 * sizeof(char));

    // Main game loop
    while (!exitWindow)
    {
    	if (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose()) exitWindow = true;

    	// Main Menu
    	if(scene == 0){
    		;;
    	}

    	// Settings
        if(scene == 1){
            ;;
        }

        // Gameplay
        if(scene == 2){
        	;;
        }

        BeginDrawing();

        	// Menu
        	if(scene == 0){
                ClearBackground((Color){0, 20, 41, 255}); // Deep blue
                DrawTextEx(silver, "Play Human", (Vector2){940, 565}, 120, 4, RAYWHITE);
                DrawTextEx(silver, "Play Computer", (Vector2){940, 645}, 120, 4, RAYWHITE);
                DrawTextEx(silver, "Settings", (Vector2){940, 725}, 120, 4, RAYWHITE);
                DrawTextEx(silver, "Quit", (Vector2){940, 805}, 120, 4, RAYWHITE);
                if(GetTime() - lastGameOver < 2){
                    DrawTextEx(silver, "Game Over", (Vector2){(screenWidthF / 2) + (screenWidthF * 0.0625), 20}, 120, 4, RED);
                    printf("%f", (screenWidthF / 2) + (screenWidthF * 0.0625));
                }
                if(settings[(TextFindIndex(settings, "showFPS=") + 8)] == '1'){DrawFPS(10, 10);}
            }

            // Settings menu
            if(scene == 1){
            	;;
            }

            // Gameplay
            if(scene == 2){
            	;;
            }
        EndDrawing();
	}

	CloseAudioDevice();
    UnloadFont(silver);
    UnloadSound(gameOverSound);
    UnloadSound(hitSound);
    UnloadSound(buttonHoverSound);
    CloseWindow();

    return 0;
}