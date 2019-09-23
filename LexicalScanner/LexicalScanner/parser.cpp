#include "parser.h"

void parser::getTokenList(std::vector<std::string> scanedTokens) {
	tokens = scanedTokens;
	return;
}

void parser::getNextToken() {
	if (tokens[tokenCount] == "_EOL") {
		lineCount++;
	}
	currToken = tokens[tokenCount];
	tokenCount++;
	nextToken = tokens[tokenCount];
	tokenCount++;
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