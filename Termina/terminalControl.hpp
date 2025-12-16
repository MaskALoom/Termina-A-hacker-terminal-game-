#ifndef TERMINAL_CONTROL_HPP
#define TERMINAL_CONTROL_HPP

#include <functional>
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include "raylib.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

typedef enum{
    C_CLEAR = 0,
    C_MKDIR,
    C_TOUCH,
    C_LS,
    C_CD,
    C_CDBACK,
    C_RM,
    C_OPEN,
    C_EXIT,
    ERROR
}CommandID;

typedef enum{
    DIRECTORY = 0,
    TERM_FILE
}FileType;

class DirectoryFile{
    public:
	std::string name;
	FileType fileType;
	std::vector<DirectoryFile> directoryFile;
	std::vector<std::string> savedFileStrings;
	std::vector<std::string> unsavedFileStrings;
};

class TerminalPlayer{
    public:
	std::string playerID = "amos";
	std::string systemID = "A87491";
};

class TimeFlow{
    public:
	bool isActive = false;
	float time = 0.0f;
	float limit = 0.0f;
};

class Terminal{
    public:
	DirectoryFile root = {
	    .name = "root",
	    .fileType = DIRECTORY
	};
	TerminalPlayer tp;
	int terminalLine = 0;
	Font terminalFont = LoadFontEx("font.ttf", 25, 0, 0);
	Font terminalFontBig = LoadFontEx("font.ttf", 35, 0, 0);
	std::string workingDirectoryString;
	std::string terminalStart = tp.playerID + "@" + tp.systemID + "$" + workingDirectoryString + ":" + "`_";
	std::string inputedCommand;

	std::vector<std::string> terminalStrings;
	std::vector<std::string> directoryTree;

	DirectoryFile *workingDirectory = &root;
	DirectoryFile *workingTermFile;

	bool isFile = false;
	bool isFileInfo = false;
	bool exit = false;
	bool isCommandActive = false;

	TimeFlow backkey = {false, 0.0f, 0.5f};
	TimeFlow fileLoading = {false, 0.0f, 0.035f};
	TimeFlow scrolling = {false, 0.0f, 0.5f};
	TimeFlow holdingEnter = {false, 0.0f, 0.5f};

	int fileLoadingLines = 0;
	int currentScroll = 0;
	int currentTerminalScrolll = 0;
	int fileLineLimit = 25;
	std::vector<std::string> commandStrings;

	void TerminalMain(void){
	    terminalLine = terminalStrings.size() - 1;

	    Typing();
	    FileControls();
	    TerminalControls();

	    if(fileLoading.isActive){
		fileLoading.time += GetFrameTime();
		while(workingTermFile->unsavedFileStrings.at(fileLoadingLines).empty()){
		    ++fileLoadingLines;
		    if(fileLoadingLines >= workingTermFile->unsavedFileStrings.size()){
			break;
		    }
		}
		if(fileLoading.time >= fileLoading.limit){
		    fileLoading.time = 0.0f;
		    ++fileLoadingLines;
		    if(fileLoadingLines >= workingTermFile->unsavedFileStrings.size()){
			fileLoading.isActive = false;
			fileLoadingLines = 0;
		    }
		}
	    }
	}
	void Typing(){
	    int tempDFLine;
	    std::vector<std::string> *tempDF;
	    if(isFile) tempDF = &workingTermFile->unsavedFileStrings;
	    else if(!isFile) tempDF = &terminalStrings;
	    if(!isFile){
		DirectoryChange();
		if(terminalStrings.empty()){
		    terminalStrings.push_back(terminalStart);
		    terminalLine = 0;
		}
	    }
	    if(!isFileInfo){
		int termChar = GetCharPressed();
		if(termChar >= 32 && termChar <= 125 && termChar != '`'){
		    tempDFLine = tempDF->size() - 1;

		    tempDF->at(tempDFLine).pop_back();
		    tempDF->at(tempDFLine).push_back(termChar);
		    tempDF->at(tempDFLine).push_back('_');

		    if(!isFile) inputedCommand.push_back(termChar);
		    else if(isFile){
			Vector2 fileTextLineLength = MeasureTextEx(terminalFont, tempDF->at(tempDFLine).c_str(), 25, 1);
			if(fileTextLineLength.x > 680){
			    tempDF->at(tempDFLine).pop_back();
			    tempDF->at(tempDFLine).pop_back();
			    tempDF->push_back("-");
			    tempDF->at(tempDFLine + 1).push_back(termChar);
			    tempDF->at(tempDFLine + 1).push_back('_');
			    tempDF->at(tempDFLine + 1).erase(0, 1);
			}
			if(workingTermFile->unsavedFileStrings.size() - 1 - currentScroll >= fileLineLimit){
			    ++currentScroll;
			}
		    }
		    termChar = GetCharPressed();
		}

		if(IsKeyDown(KEY_BACKSPACE)){
		    tempDFLine = tempDF->size() - 1;
		    if(tempDF->at(tempDFLine).size() > terminalStart.size() || isFile){
			if(!backkey.isActive){
			    if(tempDF->at(tempDFLine).size() > 1){
				tempDF->at(tempDFLine).pop_back();
				tempDF->at(tempDFLine).pop_back();
				if(!isFile) inputedCommand.pop_back();
				tempDF->at(tempDFLine).push_back('_');
			    }
			    else{
				if(isFile){
				    if(tempDF->size() > 1){
					tempDF->pop_back();
					tempDF->at(tempDFLine - 1).push_back('_');
				    }
				}
			    }
			    backkey.isActive = true;
			}
			else if(backkey.isActive){
			    backkey.time += GetFrameTime();
			    if(backkey.time >= backkey.limit){
				backkey.time = backkey.limit;
				if(tempDF->at(tempDFLine).size() > 1){
				    tempDF->at(tempDFLine).pop_back();
				    tempDF->at(tempDFLine).pop_back();
				    if(!isFile){
					inputedCommand.pop_back();
				    }
				    tempDF->at(tempDFLine).push_back('_');
				}
				else{
				    if(isFile){
					if(tempDF->size() > 1){
					    tempDF->pop_back();
					    tempDF->at(tempDFLine - 1).push_back('_');
					}

				    }
				}
			    }
			}
			if(isFile){
			    if(workingTermFile->unsavedFileStrings.size() > 1){
				if(workingTermFile->unsavedFileStrings.size() - 1 - currentScroll <= 0){
				    --currentScroll;
				}
			    }
			}
		    }
		}
		else{
		    backkey.isActive = false;
		    backkey.time = 0.0f;
		}
	    }
	}
	void TerminalControls(){
	    if(IsKeyPressed(KEY_ENTER)){
		if(!isFile){
		    std::string tempString;
		    terminalStrings.at(terminalLine).pop_back();
		    if(!inputedCommand.empty()){
			for(char c : inputedCommand){
			    if(c == ' '){
				if(!tempString.empty()){
				    commandStrings.push_back(tempString);
				    tempString.clear();
				}
			    }
			    else{
				tempString.push_back(c);
			    }
			}
			if(!tempString.empty()){
			    commandStrings.push_back(tempString);
			}
		    }
		    isCommandActive = true;
		}
	    }
	}
	void FileControls(){
	    if(isFile){
		if(!isFileInfo){
		    if(IsKeyDown(KEY_ENTER)){
			if(!holdingEnter.isActive){
			    if(IsKeyPressed(KEY_ENTER)){
				workingTermFile->unsavedFileStrings.at(workingTermFile->unsavedFileStrings.size() - 1).pop_back();
				workingTermFile->unsavedFileStrings.push_back("_");

				holdingEnter.isActive = true;
			    }
			}
			else if(holdingEnter.isActive){
			    holdingEnter.time += GetFrameTime();
			    if(holdingEnter.time >= holdingEnter.limit){
				holdingEnter.time = holdingEnter.limit;

				workingTermFile->unsavedFileStrings.at(workingTermFile->unsavedFileStrings.size() - 1).pop_back();
				workingTermFile->unsavedFileStrings.push_back("_");
			    }
			}
			if(!fileLoading.isActive){
			    if(workingTermFile->unsavedFileStrings.size() - 1 - currentScroll >= fileLineLimit){
				++currentScroll;
			    }
			}
		    }
		    else if(IsKeyPressed(KEY_Q)){
			isFile = false;
			workingTermFile->unsavedFileStrings.at(workingTermFile->unsavedFileStrings.size() - 1).pop_back();
			workingTermFile->unsavedFileStrings.at(workingTermFile->unsavedFileStrings.size() - 1).pop_back();
			workingTermFile->savedFileStrings = workingTermFile->unsavedFileStrings;
			workingTermFile = nullptr;

			currentScroll = 0;
		    }
		    else if(IsKeyDown(KEY_UP)){
			if(!scrolling.isActive){
			    --currentScroll;
			    if(currentScroll < 0) currentScroll = 0;
			    scrolling.isActive = true;
			}
			else if(scrolling.isActive){
			    scrolling.time += GetFrameTime();
			    if(scrolling.time >= scrolling.limit){
				--currentScroll;
				if(currentScroll < 0) currentScroll = 0;
			    }
			}
		    }
		    else if(IsKeyDown(KEY_DOWN)){
			if(!scrolling.isActive){
			    ++currentScroll;
			    scrolling.isActive = true;
			}
			else if(scrolling.isActive){
			    scrolling.time += GetFrameTime();
			    if(scrolling.time >= scrolling.limit){
				scrolling.time = scrolling.limit;
				++currentScroll;
			    }
			}
		    }
		    else{
			scrolling.isActive = false;
			scrolling.time = 0.0f;

			holdingEnter.isActive = false;
			holdingEnter.time = 0.0f;
		    }
		}

		if(IsKeyPressed(KEY_TAB)){
		    if(!isFileInfo){
			isFileInfo = true;
		    }
		    else{
			isFileInfo = false;
		    }
		}
	    }
	}
	void DirectoryChange(void){
	    workingDirectoryString.clear();
	    if(workingDirectoryString.empty()){
		workingDirectoryString.append("~/");
		if(!directoryTree.empty()){
		    for(int i = 0; i < directoryTree.size(); i++){
			workingDirectoryString.append(directoryTree[i] + "/");
		    }
		}
	    }
	    terminalStart.clear();
	    terminalStart = tp.playerID + "@" + tp.systemID + "$" + workingDirectoryString + ": " + "_";
	}
	void DrawUserTerminal(Color color){
	    int posY = 10;
	    int count = 0;
	    if(!isFile){
		for(int i = currentTerminalScrolll; i < terminalStrings.size(); ++i){
		    DrawTextEx(terminalFont, terminalStrings.at(i).c_str(), (Vector2){10, (float)posY}, 25, 1, color);
		    posY += 26;
		    ++count;
		}
	    }
	}
	void DrawFile(Color color){
	    int posY = 10;
	    int scrollCount = 0;
	    if(isFile){
		DrawRectangleLinesEx((Rectangle){290, 5, 700, 680}, 2, color);
		for(int i = 0; i < fileLineLimit; i++){
		    std::string lineCount = std::to_string(currentScroll + i);
		    Vector2 lineCountLength = MeasureTextEx(terminalFont, lineCount.c_str(), 25, 1);
		    if(workingTermFile->unsavedFileStrings.size() - 1 == i + currentScroll){
			DrawTextEx(terminalFontBig, lineCount.c_str(), (Vector2){280 - lineCountLength.x - 4, (float)posY + 1}, 35, 1, color);
		    }
		    else{
			DrawTextEx(terminalFont, lineCount.c_str(), (Vector2){280 - lineCountLength.x, (float)posY + 5}, 25, 1, color);
		    }
		    posY += 26;
		}
		posY = 10;
		for(std::string s : workingTermFile->unsavedFileStrings){
		    if(scrollCount >= currentScroll && scrollCount - currentScroll < fileLineLimit){
			if(fileLoading.isActive){
			    if(scrollCount < fileLoadingLines - 1){
				DrawTextEx(terminalFont, s.c_str(), (Vector2){300, (float)posY + 5}, 25, 1, color);
			    }
			}
			else DrawTextEx(terminalFont, s.c_str(), (Vector2){300, (float)posY + 5}, 25, 1, color);
			posY += 26;
		    }
		    ++scrollCount;
		}
		if(isFileInfo){
		    DrawRectangle(430, 255, 420, 180, BLACK);
		    DrawRectangleLinesEx((Rectangle){430, 255, 420, 180}, 3, color);

		    std::string fileDirectory;
		    int sizeBytes = 0;

		    if(directoryTree.empty()) fileDirectory = "root/" + workingTermFile->name;
		    else{
			fileDirectory.append("root/");
			for(int i = 0; i < directoryTree.size(); i++){
			    fileDirectory.append(directoryTree[i] + "/");
			}
			fileDirectory.append(workingTermFile->name);
		    }

		    for(std::string s : workingTermFile->unsavedFileStrings){
			sizeBytes += s.size();
		    }
		    sizeBytes -= 1;

		    std::string infoName = "File Name: " + workingTermFile->name;
		    std::string infoDirectory = "File Directory: " + fileDirectory;
		    std::string infoSize = "File Size: " + std::to_string(sizeBytes) + " Bytes";

		    DrawTextEx(terminalFont, infoName.c_str(), (Vector2){438, (float)263}, 25, 1, color);
		    DrawTextEx(terminalFont, infoDirectory.c_str(), (Vector2){438, (float)289}, 25, 1, color);
		    DrawTextEx(terminalFont, infoSize.c_str(), (Vector2){438, (float)315}, 25, 1, color);
		}
	    }
	}
	void DrawTerminal(void){
	    Color terminalColor = RED;
	    DrawUserTerminal(terminalColor);
	    DrawFile(terminalColor);
	}
};

void RunCommand(Terminal *pTerminal, CommandID commandID);

class CommandMapMain{
    private:
	std::unordered_map<std::string, std::function<void(Terminal &pTerminal)>> commandTree = {
	    {"clear", [](Terminal &pTerminal){RunCommand(&pTerminal, C_CLEAR);}},
	    {"mkdir", [](Terminal &pTerminal){RunCommand(&pTerminal, C_MKDIR);}},
	    {"cd", [](Terminal &pTerminal){RunCommand(&pTerminal, C_CD);}},
	    {"rm", [](Terminal &pTerminal){RunCommand(&pTerminal, C_RM);}},
	    {"touch", [](Terminal &pTerminal){RunCommand(&pTerminal, C_TOUCH);}},
	    {"ls", [](Terminal &pTerminal){RunCommand(&pTerminal, C_LS);}},
	    {"open", [](Terminal &pTerminal){RunCommand(&pTerminal, C_OPEN);}},
	    {"exit", [](Terminal &pTerminal){RunCommand(&pTerminal, C_EXIT);}},
	    {"error", [](Terminal &pTerminal){RunCommand(&pTerminal, ERROR);}}
	};
    public:
	Terminal *pTerminal;
	void CheckCommand(void){
	    if(pTerminal->isCommandActive){
		if(!pTerminal->commandStrings.empty()){
		    auto it = commandTree.find(pTerminal->commandStrings[0]);
		    if(it != commandTree.end()){
			commandTree[pTerminal->commandStrings[0]](*pTerminal);
		    }
		    else{
			commandTree["error"](*pTerminal);
		    }
		}

		pTerminal->terminalStrings.push_back(pTerminal->terminalStart);
		pTerminal->commandStrings.clear();
		pTerminal->inputedCommand.clear();

		pTerminal->isCommandActive = false;
		int newTerminalLine = pTerminal->terminalStrings.size();
		while(newTerminalLine - pTerminal->currentTerminalScrolll >= 27){
		    pTerminal->currentTerminalScrolll++;
		}
	    }
	}
};

#endif
