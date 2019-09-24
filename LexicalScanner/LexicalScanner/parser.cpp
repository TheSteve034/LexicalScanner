#include "parser.h"

void parser::getTokenList(std::vector<std::string> scanedTokens) {
	tokens = scanedTokens;
	return;
}

void parser::getNextToken() {
	if (tokens[tokenCount] == "_EOF") {
		currToken = tokens[tokenCount];
		tokenCount++;
		return;
	}
	if (tokens[tokenCount] == "_EOL") {
		tokenCount++;
		currToken = tokens[tokenCount];
		tokenCount++;
		nextToken = tokens[tokenCount];
		tokenCount++;
	}
	else {
		currToken = tokens[tokenCount];
		tokenCount++;
		nextToken = tokens[tokenCount];
		tokenCount++;
	}
	return;
}

 void parser::getPath(std::string& path) {
	filepath = path;
}

void parser::createFiles() {
	std::filesystem::path p(filepath);
	std::string asmFileName = p.stem().string() + ".asm";
	std::string errorFileName = p.stem().string() + ".err";
 	assembly.open(asmFileName, std::ios::out | std::ios::trunc);
	error.open(errorFileName, std::ios::out | std::ios::trunc);
}

//these two functions should only be called after getNextToken;
void parser::progressPastSingleLineComments() {
	if (currToken == "//") {
		if (tokenCount != 0) {
			tokenCount--;
		}
		while (tokens[tokenCount] != "_EOL") {
			std::string temp = tokens[tokenCount];
			tokenCount++;
		}
		tokenCount = tokenCount + 2;
		getNextToken();
	}
}

int parser::progressPastMultiLineComments() {
	if (currToken == "/*") {
		if (tokenCount != 0) {
			tokenCount--;
		}
		while (tokens[tokenCount] != "*/") {
			if (tokens[tokenCount] == "_EOL") {
			}
			if (tokens[tokenCount] == "_EOF") {
				return -1;
			}
			tokenCount++;
		}
		tokenCount++;
		if (tokens[tokenCount] == "_EOL") {
			tokenCount++;
		}
		getNextToken();
	}
}

/*
< program >	::=	program   <identifier>   ;   <block>   .
*/
int parser::programRule() {
	if (currToken != "PROGRAM") {
		std::cout << "SYNTAX ERROR! Program must start with the word PROGRAM." << std::endl;
		return -1;
	}
	else {
	//check to see if the next token is a valid identifier
		//if()
	}
}

void parser::parseFile() {
	getNextToken();
}