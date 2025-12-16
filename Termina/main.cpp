#include <iostream>
#include "raylib.h"
#include <vector>
#include <array>
#include <string>
#include <cstring>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

class TypingAnimation{
    private:
	int currentFrame = 1;
	float frameTime = 0;
	float frameSpeed = 0.75;
	float colorOpacity = 1.0f;

	char name[2500] = "\0";
	char terminal[2500] = "\0";
	char command[2500] = "\0";
	int letterCount = 0;
	int letterCountTerminal = 0;

	bool isHoldingBackspace = false;
	bool isTerminalActive = false;
	bool isTyping = false;
	bool firstKey = false;
	float holdingTime = 0;
	float holdingSpeed = 0.5;

	std::array<char[30], 2> commandString;

	Font textFont = LoadFontEx("font.ttf", 35, 0, 0);
	Font smallTextFont = LoadFontEx("font.ttf", 20, 0, 0);
    public:
	void RunCommand(void){
	    std::string command1 = commandString[0];
	    std::string command2 = commandString[1];
	    for(int i = 0; i < 2; i++){
		if(i == 0){
		    if(command1 == "contact"){
			if(command2 == "[Leopard]"){
			    std::cout << "Contacting Adminastrator Leopard..." << std::endl;
			    std::cout << "Contact with Adminastrator Leopard FAILED" << std::endl;
			}
			else if(command2 == "[Loom]"){
			    std::cout << "Contacting Observer Loom..." << std::endl;
			    std::cout << "Contact with Observer Loom Denied" << std::endl;
			}
			else{
			    std::cout << "Unknwon contact by code or name that you are attempting to contact" << std::endl;
			}
		    }
		    else if(command1 == "locate"){
			if(command2 == "4951"){
			    std::cout << "The location that you are attempting to locate is no longer\nexistent in this realm. Try another location" << std::endl;
			}
			else{
			    //std::cout << "Unknown area code or name that you are attempting to locate" << std::endl;
			    std::cout << "Count not locate by code or name '" << command2 << "'" << std::endl;
			}
		    }
		    else{
			std::cout << "Unknwon Command" << std::endl;
		    }
		}
	    }
	    memset(commandString[0], '\0', sizeof(commandString[0]));
	    memset(commandString[1], '\0', sizeof(commandString[1]));
	}
	void GetNameInput(void){
	    int key = GetCharPressed();
	    if(key >= 32 && (key <= 125) && letterCount < 2500 && key != '/'){
		if(!isTerminalActive){
		    name[letterCount] = key;
		    name[letterCount + 1] = '\0';
		    letterCount ++;
		}
		else{
		    terminal[letterCountTerminal] = key;
		    terminal[letterCountTerminal + 1] = '\0';
		    letterCountTerminal ++;
		}
	    }

	    key = GetCharPressed();

	    if(!isTerminalActive){
		if(IsKeyPressed(KEY_SLASH)){
		    isTerminalActive = true;
		    terminal[letterCountTerminal] = '/';
		    letterCountTerminal = 1;
		}
	    }
	    else{
		if(IsKeyPressed(KEY_Q)){
		    isTerminalActive = false;
		    letterCountTerminal = 0;
		    memset(terminal, '\0', sizeof(terminal));
		}
		else if(IsKeyPressed(KEY_ENTER)){
		    int commandLine = 0;
		    int commandCount = 0;
		    int typedCommandCount = 1;

		    while(commandLine < 2){
			if(terminal[typedCommandCount] != ' ' && terminal[typedCommandCount] != '\0'){
			    commandString[commandLine][commandCount] = terminal[typedCommandCount];
			    commandCount ++;
			    typedCommandCount ++;
			}
			else{
			    commandLine ++;
			    commandCount = 0;
			    typedCommandCount ++;

			    if(commandLine > 1){
				break;
			    }
			}
		    }
		    isTerminalActive = false;
		    letterCountTerminal = 0;
		    memset(terminal, '\0', sizeof(terminal));
		    memset(command, '\0', sizeof(command));

		    RunCommand();
		}
	    }
	    if(IsKeyDown(KEY_BACKSPACE)){
		if(!isHoldingBackspace){
		    holdingTime += GetFrameTime();
		    if(holdingTime >= holdingSpeed){
			isHoldingBackspace = true;
			holdingTime = 0;
		    }
		    if(!firstKey){
			if(!isTerminalActive){
			    letterCount --;
			    if(letterCount < 0) letterCount = 0;
			    name[letterCount] = '\0';
			}
			else{
			    if(terminal[letterCountTerminal - 1] != '/'){
				letterCountTerminal --;
				if(letterCountTerminal < 0) letterCountTerminal = 0;
				terminal[letterCountTerminal] = '\0';
			    }
			}
			firstKey = true;
		    }
		}
		else{
		    isTyping = true;
		    currentFrame = 1;
		    colorOpacity = 1.0f;
		    frameTime = 0;
		    if(!isTerminalActive){
			letterCount --;
			if(letterCount < 0) letterCount = 0;
			name[letterCount] = '\0';
		    }
		    else{
			if(terminal[letterCountTerminal - 1] != '/'){
			    letterCountTerminal --;
			    if(letterCountTerminal < 0) letterCountTerminal = 0;
			    terminal[letterCountTerminal] = '\0';
			}
		    }
		}
	    }
	    else{
		holdingTime = 0;
		isHoldingBackspace = false;
		firstKey = false;

		isTyping = false;
	    }
	}
	void ProgressAnimation(void){
	    Vector2 nameLength = MeasureTextEx(textFont, name, 35, 1);
	    Vector2 terminalLength = MeasureTextEx(textFont, terminal, 20, 1);
	    if(isTyping == false){
		frameTime += GetFrameTime();
		if(frameTime >= frameSpeed){
		    frameTime = 0;
		    currentFrame ++;
		    if(currentFrame > 1) currentFrame = 0;
		    if(currentFrame == 1) colorOpacity = 1.0f;
		    else if(currentFrame == 0) colorOpacity = 0.0f;
		}
	    }
	    DrawTextEx(textFont, name, (Vector2){10, 10}, 35, 1, WHITE);
	    if(!isTerminalActive){
		DrawRectangle(10 + nameLength.x, 10, 20, 35, Fade(WHITE, colorOpacity));
	    }
	    else{
		DrawTextEx(smallTextFont, terminal, (Vector2){10, 690}, 20, 1, WHITE);
		DrawRectangle(10 + terminalLength.x, 690, 15, 20, Fade(WHITE, colorOpacity));
	    }
	}
};

int main(void){
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Decode");
    SetTargetFPS(60);
    ToggleFullscreen();
    TypingAnimation ta;
    while(!WindowShouldClose()){
	ta.GetNameInput();
	BeginDrawing();
	ClearBackground(BLACK);
	ta.ProgressAnimation();
	EndDrawing();
    }
    CloseWindow();
    return 0;
}
