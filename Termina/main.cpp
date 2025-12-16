#include <iostream>
#include "raylib.h"

#include "terminalControl.hpp"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

int main(void){
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Decoded");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);
    ToggleFullscreen();

    Terminal terminal;
    CommandMapMain commandMap;
    commandMap.pTerminal = &terminal;
    while(!WindowShouldClose()){
	terminal.TerminalMain();
	commandMap.CheckCommand();
	if(terminal.exit == true){
	    return 0;
	}
	BeginDrawing();
	ClearBackground(BLACK);
	terminal.DrawTerminal();
	EndDrawing();
    }
    CloseWindow();
    return 0;
}
