
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "raylib.h"

// Struct for a buttons shape size/pos and text
struct BUTTON{
	int width;
	int height;
	int Xpos;
	int Ypos;
	char *text;
};

// Ball vars (global)
float ballX = 400.0;
float ballY = 300.0; // Top right is 0,0
float modVelo = 2.5;
float maxVelo = 16.9;
float resistence = 1.9;
float Xmovement = 0;
float Ymovement = 0;

struct Rectangle platform = {
	5,
	580,
	790,
	10
};
// Function for keyboard movement
int moveKeys(){

		if(IsKeyDown(263) == 1){ // Left
			Xmovement = Xmovement - modVelo;
		}
		if(IsKeyDown(265) == 1){ // Up
			Ymovement = Ymovement - modVelo;
		}
		if(IsKeyDown(262) == 1){ // Right
			Xmovement = Xmovement + modVelo;
		}
		if(IsKeyDown(264) == 1){ // Down
			Ymovement = Ymovement + modVelo;
		}

	if(Xmovement > maxVelo){Xmovement=maxVelo;}
	if(Xmovement < -(maxVelo)){Xmovement=-(maxVelo);}
	if(Ymovement > maxVelo){Ymovement=maxVelo;}
	if(Ymovement < -(maxVelo)){Ymovement=-(maxVelo);}

	if(fabsf(Xmovement) > resistence){
		if(Xmovement>resistence){Xmovement = Xmovement - resistence;}
		if(Xmovement<-(resistence)){Xmovement = Xmovement + resistence;}
	}else{
		Xmovement = 0;
	}
	if(fabsf(Ymovement) > resistence){
		if(Ymovement>resistence){Ymovement = Ymovement - resistence;}
		if(Ymovement<-(resistence)){Ymovement = Ymovement + resistence;}
	}else{
		Ymovement = 0;
	}

	ballX = ballX + Xmovement;
	ballY = ballY + Ymovement;

	return 0;
}

// Function for mouse movement
void moveMouse(){
	ballX = GetMouseX();
	ballY = GetMouseY();
}

void movePhysics(){
	if(CheckCollisionPointRec((Vector2){ballX, ballY + 10}, platform) == 0){
		Ymovement = Ymovement + 1.2;
		if(Ymovement > maxVelo){Ymovement=maxVelo;}
		ballY = ballY + Ymovement;
	}
}

int main(int argc, char* argv[]){
	
	// Setup the mode button with values
	struct BUTTON modeButton;
	modeButton.width = 110;
	modeButton.height = 50;
	modeButton.Xpos = 680;
	modeButton.Ypos = 10;
	modeButton.text = "kb mode";

struct Rectangle button = {
	modeButton.Xpos,
	modeButton.Ypos,
	modeButton.width,
	modeButton.height
};


	// initialize variables
	int width = 800;
	int height = 600;
	int inputMode = 1; // Keyboard is 1, mouse is 2

	// Setup window
	InitWindow(width, height, "ball");
	SetTargetFPS(60);

	// Main loop
	while (!WindowShouldClose())
    {

    	// Doing stuff
    	if(IsMouseButtonDown(0) == 1 && CheckCollisionPointRec((Vector2){GetMouseX(), GetMouseY()}, button) == 1){
    		if(inputMode == 1){
    			inputMode = 2;
    			modeButton.text = "mouse mode";
    		}else if(inputMode == 2){
    			inputMode = 3;
    			modeButton.text = "physics mode";
    			ballX = 300;
    			ballY = 400;
    		}else if(inputMode == 3){
    			inputMode = 1;
    			modeButton.text = "kb mode";
    		}

    	SetMousePosition(400, 300);
    	}
    	if(inputMode == 1){
        	moveKeys();
        }
        if(inputMode == 2){
        	moveMouse();
        }
        if(inputMode == 3){
        	movePhysics();
        }

        // Drawing stuff
        BeginDrawing();

        	ClearBackground(RAYWHITE);
        	DrawRectangle(platform.x, platform.y, platform.width, platform.height, BROWN); 
        	DrawRectangle(modeButton.Xpos, modeButton.Ypos, modeButton.width, modeButton.height, MAROON);
        	DrawText(modeButton.text, 685, 25, 20, RAYWHITE);

            DrawCircle(round(ballX), round(ballY), 10, RED);

            DrawFPS(10, 10);

        EndDrawing();
    }

    // End program
    CloseWindow();

return 0;

}
