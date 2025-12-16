#include "terminalControl.hpp"

void RunCommand(Terminal *pTerminal, CommandID commandID){
    switch(commandID){
	case ERROR:
	    pTerminal->terminalStrings.push_back("Unknown Command");
	    break;
	case C_CLEAR:
	    pTerminal->terminalStrings.clear();
	    pTerminal->currentTerminalScrolll = 0;
	    break;
	case C_MKDIR:
	    pTerminal->workingDirectory->directoryFile.push_back((DirectoryFile){
		    .name = pTerminal->commandStrings[1],
		    .fileType = DIRECTORY
		    });
	    break;
	case C_RM:{
	    bool isFound = false;
	    if(pTerminal->commandStrings.size() > 1){
		for(auto it = pTerminal->workingDirectory->directoryFile.begin(); it != pTerminal->workingDirectory->directoryFile.end();){
		    if(it->name == pTerminal->commandStrings[1]){
			isFound = true;

			std::string fileType = "";
			if(it->fileType == TERM_FILE) fileType = "File";
			else if(it->fileType == DIRECTORY) fileType = "Directory";

			pTerminal->terminalStrings.push_back("Successfully removed " + fileType);

			pTerminal->workingDirectory->directoryFile.erase(it);
			break;
		    }
		    else{
			++it;
		    }
		}
	    }
	    if(!isFound) pTerminal->terminalStrings.push_back("Could not find file or directory: " + pTerminal->commandStrings[1] + " to remove");
	    break;
	}
	case C_OPEN:{
		bool fileFound = false;
		if(pTerminal->commandStrings.size() > 1){
		    for(int i = 0; i < pTerminal->workingDirectory->directoryFile.size(); i++){
			if(pTerminal->workingDirectory->directoryFile.at(i).fileType == TERM_FILE){
			    if(pTerminal->workingDirectory->directoryFile.at(i).name == pTerminal->commandStrings[1]){
				pTerminal->workingTermFile = &pTerminal->workingDirectory->directoryFile.at(i);
				pTerminal->isFile = true;
				fileFound = true;
				if(pTerminal->workingTermFile->savedFileStrings.empty()){
				    pTerminal->workingTermFile->savedFileStrings.push_back("_");
				    pTerminal->workingTermFile->unsavedFileStrings = pTerminal->workingTermFile->savedFileStrings;
				}
				else{
				    pTerminal->workingTermFile->unsavedFileStrings.at(pTerminal->workingTermFile->unsavedFileStrings.size() - 1).push_back('_');
				}
				pTerminal->fileLoading.isActive = true;
				break;
			    }
			}
		    }
		    if(!fileFound){
			pTerminal->terminalStrings.push_back("Could open file to: " + pTerminal->commandStrings[1]);
		    }
		}
	    break;
	}
	case C_TOUCH:
	    pTerminal->workingDirectory->directoryFile.push_back((DirectoryFile){
		    .name = pTerminal->commandStrings[1],
		    .fileType = TERM_FILE
		    });
	    break;
	case C_LS:
	    if(!pTerminal->workingDirectory->directoryFile.empty()){
		std::string fileType;
		for(DirectoryFile dn : pTerminal->workingDirectory->directoryFile){
		    if(dn.fileType == DIRECTORY) fileType = " (Directory)";
		    else if(dn.fileType == TERM_FILE) fileType = " (File)";
		    pTerminal->terminalStrings.push_back("-" + dn.name + fileType);
		}
	    }
	    else{
		pTerminal->terminalStrings.push_back("Directory is Empty");
	    }
	    break;
	case C_CD:
	    if(pTerminal->commandStrings.size() > 1){
		if(pTerminal->commandStrings[1] == ".."){
		    if(!pTerminal->directoryTree.empty()){
			pTerminal->directoryTree.pop_back();
			DirectoryFile *tempFile = &pTerminal->root;
			for(std::string s : pTerminal->directoryTree){
			    if(pTerminal->directoryTree.size() == 1){
				pTerminal->workingDirectory = &pTerminal->root;
				break;
			    }
			    else{
				for(int i = 0; i < tempFile->directoryFile.size(); ++i){
				    if(tempFile->directoryFile.at(i).name == s){
					tempFile = &tempFile->directoryFile.at(i);
				    }
				}
			    }
			}
			pTerminal->workingDirectory = tempFile;
			pTerminal->DirectoryChange();
		    }
		    else{
			pTerminal->terminalStrings.push_back("PERMISSION DENIED. You cannot go behind the root directory");
		    }
		}
		else{
		    bool isFound = false;
		    for(int i = 0; i < pTerminal->workingDirectory->directoryFile.size(); ++i){
			if(pTerminal->workingDirectory->directoryFile.at(i).fileType == DIRECTORY){
			    if(pTerminal->workingDirectory->directoryFile.at(i).name == pTerminal->commandStrings[1]){
				pTerminal->workingDirectory = &pTerminal->workingDirectory->directoryFile.at(i);
				pTerminal->directoryTree.push_back(pTerminal->workingDirectory->name);
				isFound = true;

				pTerminal->DirectoryChange();
				break;
			    }
			}
		    }
		    if(!isFound){
			pTerminal->terminalStrings.push_back("Could not change directory to: " + pTerminal->commandStrings[1]);
		    }
		}
	    }
	    break;
	case C_EXIT:
	    pTerminal->exit = true;
	    break;
    }
}
