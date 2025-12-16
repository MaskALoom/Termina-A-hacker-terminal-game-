#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <fstream>

#include "raylib.h"
#include <cjson/cJSON.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

class TerminalManager{
    private:
	std::unordered_map<std::string, std::string> commandLocate = {
	    {"A7831", "Frozen Dunes"},
	    {"A9285", "Abandoned Worklands"},
	    {"invalid", "could not locate with that command"}
	};
	std::unordered_map<std::string, std::string> commandContact = {
	    {"[Leopard]", "Adminastrator Leopard"},
	    {"{Loom}", "Observer Loom"},
	    {"Lizzie", "Jr. Overseer Lizzie"},
	    {"invalid", "Could not contact with that command"}
	};
	std::unordered_map<std::string, std::string> commandView= {
	    {"menu", "The Menu"},
	    {"locations", "Available Locations"},
	    {"system", "Personal System"},
	    {"invalid", "Could view with that command"}
	};
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> commandMap = {
	    {"locate", commandLocate},
	    {"contact", commandContact},
	    {"view", commandView}
	};
    public:
	std::string terminalCommandString = "";
	std::string terminalString = "";

	std::vector<std::string> commands;

	bool isTerminalCommands = false;
	bool isBackspaceHeld = false;
	float holdTime = 0;
	float holdLimit = 0.5f;
	Font terminalFont = LoadFontEx("font2.ttf", 30, 0, 0);

	std::vector<std::string> terminalLines;

	int currentTerminalLine = 0;
	void RunCommand(){
	    auto it = commandMap.find(commands[0]);
	    if(it != commandMap.end()){
		auto it2 = it->second.find(commands[1]);
		if(it2 != it->second.end()){
		    if(commands[0] == "view"){
			std::cout << "Viewing " << it2->second << std::endl;

			terminalString.clear();
			terminalString = "Viewing" + it2->second;
		    }
		    else if(commands[0] == "locate"){
			std::cout << "Locating area[" << commands[1] << "]..."<< std::endl;
			std::cout << "Location FOUND: " << it2->second << std::endl;

			terminalString.clear();
			terminalString = "Location area[" + commands[1] + "]... " + "Location FOUND: " + it2->second;
		    }
		    else if(commands[0] == "contact"){
			std::cout << "Attempting to contact " << it2->second << "..." << std::endl;
			std::cout << "Contact with " << it2->second << " FAILED. Could not reach, failure of network" << std::endl;
		    }
		}
		else{
		    it2 = it->second.find("invalid");
		    std::cout << it2->second << std::endl;

		    terminalString.clear();
		    terminalString = it2->second;
		}
	    }
	    else{
		std::cout << "Invalid Command" << std::endl;

		    terminalString.clear();
		    terminalString = "Invalid Command";
	    }
	}
	void Terminal(){
	    std::string *pTerminal;
	    if(!isTerminalCommands) pTerminal = &terminalString;
	    else if(isTerminalCommands) pTerminal = &terminalCommandString;

	    int key = GetCharPressed();
	    if(key >= 32 && (key <= 125) && key != '/'){
		pTerminal->push_back((char)key);
	    }

	    if(!isTerminalCommands){
		if(IsKeyPressed(KEY_SLASH)){
		    isTerminalCommands = true;
		    pTerminal = &terminalCommandString;
		    pTerminal->push_back('/');
		}
		else if(IsKeyPressed(KEY_ENTER)){
		    currentTerminalLine ++;
		    pTerminal->push_back('\n');
		}
	    }
	    else{
		if(IsKeyPressed(KEY_Q)){
		    isTerminalCommands = false;
		    pTerminal->clear();
		}
		else if(IsKeyPressed(KEY_ENTER)){
		    commands.clear();
		    std::string currentCommandPart;

		    for(char c : *pTerminal){
			if(c == '/') continue;
			if(c == ' '){
			    if(!currentCommandPart.empty()){
				commands.push_back(currentCommandPart);
				currentCommandPart.clear();
			    }
			}
			else{
			    currentCommandPart.push_back(c);
			}
		    }
		    if(!currentCommandPart.empty()){
			commands.push_back(currentCommandPart);
		    }
		    isTerminalCommands = false;
		    pTerminal->clear();

		    RunCommand();
		}
	    }

	    if(IsKeyDown(KEY_BACKSPACE)){
		if(terminalLines[0].length() > 0){
		    if(pTerminal->at(pTerminal->length() - 1) == '/'){
			if(!isBackspaceHeld){
			    if(terminalLines.at(terminalLines.size()).at(terminalLines.at(terminalLines.size()).size()) != '\n'){
				terminalLines.pop_back();
			    }
			    else{
				terminalLines.at(terminalLines.size()).pop_back();
			    }
			}
		    }
		}
		if(pTerminal->length() > 0){
		    if(pTerminal->at(pTerminal->length() - 1) != '/'){
			if(!isBackspaceHeld){
			    if(pTerminal->at(pTerminal->length() - 1) == '\n'){
				currentTerminalLine --;
			    }
			    pTerminal->pop_back();
			    isBackspaceHeld = true;
			}
			else if(isBackspaceHeld){
			    holdTime += GetFrameTime();
			    if(holdTime >= holdLimit){
				holdTime = holdLimit;
				if(pTerminal->at(pTerminal->length() - 1) == '\n'){
				    currentTerminalLine --;
				}
				pTerminal->pop_back();
			    }
			}
		    }
		}
	    }
	    else{
		isBackspaceHeld = false;
		holdTime = 0;
	    }
	    if(!isTerminalCommands){
		terminalLines.clear();
		std::string newLineString;
		for(char c : *pTerminal){
		    if(c == '\n'){
			if(!newLineString.empty()){
			    terminalLines.push_back(newLineString);
			    newLineString.clear();
			}
		    }
		    else{
			newLineString.push_back(c);
		    }
		}
		if(!newLineString.empty()){
		    terminalLines.push_back(newLineString);
		}
	    }
	}
	void DrawProgramSystem(){
	    Vector2 terminalCommandsLength = MeasureTextEx(terminalFont, terminalCommandString.c_str(), 30, 1);
	    Color hackerGreen = {0, 255, 0, 255};
	    for(int i = 0; i < terminalLines.size(); i++){
		std::cout << "size: " << terminalLines.size() << std::endl;
		Vector2 terminalTextLength = MeasureTextEx(terminalFont, terminalLines[i].c_str(), 30, 1);
		int textPosY = 10;
		textPosY += 23 * i;
		DrawTextEx(terminalFont, terminalLines[i].c_str(), (Vector2){10, (float)textPosY}, 30, 1, hackerGreen);
		if(i == terminalLines.size() - 1 && !isTerminalCommands){
		    int posY = 10;
		    if(i > 0){
			posY += 23 * i;
		    }
		    DrawRectangle(10 + terminalTextLength.x, posY, 15, 25, hackerGreen);
		    posY = 15;
		}
	    }
	    if(isTerminalCommands){
		DrawTextEx(terminalFont, terminalCommandString.c_str(), (Vector2){10, 690}, 30, 1, hackerGreen);
		DrawRectangle(10 + terminalCommandsLength.x, 690, 15, 25, hackerGreen);
	    }
	}
};

class Terminal{
    private:
	Font terminalFont = LoadFontEx("font2.ttf", 25, 0, 0);
	std::vector<std::string> terminalMainString;
	int terminalSize = sizeof(terminalMainString);
	
	std::string commandString = "";
	std::vector<std::string> commandStringVector;

	bool isHeld = false;
	bool commandCall = false;
	int commandCharCount = 0;
	float holdTime = 0.0f;
	float holdLimit = 0.4f;

	float newLineLimit = 0.04f;
	float newLineTime = 0.0f;

	cJSON *root;
    public:
	std::string typedString;

	void TerminalFunc(void){
	    if(typedString.size() == 0){
		typedString.push_back('*');
	    }
	    int key = GetCharPressed();
	    if(key >= 32 && key <= 125 && key != '-'){
		typedString.push_back((char)key);
		key = GetCharPressed();
	    }
	    if(IsKeyPressed(KEY_ENTER)){
		if(!terminalMainString.empty()){
		    typedString.push_back('-');
		}
	    }
	    else if(IsKeyDown(KEY_BACKSPACE)){
		if(typedString.size() > 1){
		    if(!isHeld){
			typedString.pop_back();
			isHeld = true;

		    }
		    else if(isHeld){
			holdTime += GetFrameTime();
			if(holdTime >= holdLimit){
			    holdTime = holdLimit;
			    typedString.pop_back();
			}
		    }
		}
	    }else{
		isHeld = false;
		holdTime = 0;
	    }
	    terminalMainString.clear();
	    std::string tempString;
	    if(!typedString.empty()){
		for(char c : typedString){
		    if(c == '-'){
			terminalMainString.push_back(tempString);
			tempString.clear();
			tempString.push_back('*');
		    }
		    else{
			tempString.push_back(c);
		    }
		}
	    }
	    if(!tempString.empty()){
		terminalMainString.push_back(tempString);
	    }
	}
	void DrawTerminal(void){
	    int posY = 10;
	    Color hackerGreen = {0, 255, 0, 255};
	    hackerGreen = BLUE;
	    Vector2 textLength;
	    int i = 1;
	    for(std::string s : terminalMainString){
		if(s.at(0) == '*') s.erase(0, 1);
		if(s.size() > 0){
		    textLength = MeasureTextEx(terminalFont, s.c_str(), 25, 1);
		}
		else if(s.empty()){
		    textLength.x = 0;
		}
		DrawTextEx(terminalFont, s.c_str(), (Vector2){10.0f, (float)posY}, 25, 1, hackerGreen);
		posY += 25;
		if(i == terminalMainString.size()){
		    DrawRectangle(10 + textLength.x, posY - 25, 10, 25, hackerGreen);
		}
		++i;
	    }
	}
	void CallCommand(std::string command){
	    commandString.clear();
	    commandCall = true;
	    cJSON* commands = cJSON_GetObjectItem(root, "commands");
	    int commandsCount = cJSON_GetArraySize(commands);
	    for(int i = 0; i < commandsCount; ++i){
		cJSON* commandIndex = cJSON_GetArrayItem(commands, i);
		cJSON* commandName = cJSON_GetObjectItem(commandIndex, "command_name");
		if(commandName->valuestring == command){
		    cJSON* commandArray = cJSON_GetObjectItem(commandIndex, "command");
		    int cc = cJSON_GetArraySize(commandArray);
		    for(int j = 0; j < cc; ++j){
			cJSON* newIndex = cJSON_GetArrayItem(commandArray, j);
			cJSON* newText = cJSON_GetObjectItem(newIndex, "tt");

			//commandString.append(newText->valuestring);
			commandStringVector.push_back(newText->valuestring);
		    }
		    break;
		}
	    }
	}
	void TerminalCommand(){
	    if(commandCall == true){
		newLineTime += GetFrameTime();
		if(newLineTime >= newLineLimit){
		    newLineTime = 0;
		    typedString.append(commandStringVector.at(commandCharCount));
		    ++commandCharCount;
		}
	    }
	    if(commandCharCount >= commandStringVector.size()) commandCall = false;
	}
	void LoadRoot(std::string filename){
	    std::ifstream file(filename);
	    if(!file.is_open()){
		std::cout << "That did not go very well... FIX IT!" << std::endl;
	    }
	    else{
		std::stringstream buffer;
		buffer << file.rdbuf();

		std::string jsonContent = buffer.str();
		if(jsonContent.empty()){
		    std::cout << "Man, what the fuck is going on right now? FIX IT NOW!" << std::endl;
		}
		else{
		    root = cJSON_Parse(jsonContent.c_str());
		}
	    }
	}
};

int main(void){
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Map Testing");
    SetTargetFPS(60);
    ToggleFullscreen();
    Terminal term;
    term.LoadRoot("commands.json");
    term.CallCommand("system_error_0");
    while(!WindowShouldClose()){
	term.TerminalFunc();
	term.TerminalCommand();
	BeginDrawing();
	ClearBackground(BLACK);
	term.DrawTerminal();
	EndDrawing();
    }
    CloseWindow();
    return 0;
}
