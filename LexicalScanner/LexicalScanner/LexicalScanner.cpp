#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "scanner.h"

std::vector<std::string> getTokens(std::string filePath) {
	std::ifstream file(filePath.c_str(), std::ifstream::in);
	std::string line = "";
	std::vector<std::string> tokens = {};

	while (std::getline(file, line)) {
		std::string token = "";
		int lineLength = line.length();
		for (int i = 0; i < lineLength; i++) {
			if (line[i] == '*' && line[i + 1] == '/') {
				tokens.push_back(token);
				token = line[i];
				continue;
			}
			if (token == "*/") {
				tokens.push_back(token);
				token = "";
			}
			if (line[i] == '/' && line[i +1] == '/') {
				if (token != "") {
					tokens.push_back(token);
					token = "";
					for (int j = i; j < lineLength; j++) {
						token += line[j];
					}
					tokens.push_back(token);
					i = i + 2;
					break;
				}
				else if (token == "") {
					for (int j = i; j < lineLength; j++) {
						token += line[j];
					}
					tokens.push_back(token);
					i = i + 1;
					break;
				}
			}
			if (line[i] == '+' || line[i] == '-' || line[i] == '*' || line[i] == '=' || line[i + 1] == '<' || line[i + 1] == '>'
			|| line[i] == '(' || line[i] == ')' || line[i] == '[' || line[i] == ']' || line[i] == '.' || line[i] == ','
			|| line[i] == ';' || line[i] == '\'' || line[i] == '\"' || line[i] == '/') {
				tokens.push_back(token);
				token = line[i];
				tokens.push_back(token);
				token = "";
				continue;
			}
			else if ((line[i] == '<' && line[i + 1] == '>') || (line[i] == '<' && line[i + 1] == '=') || (line[i] == '>' && line[i + 1] == '=')
			|| (line[i] == ':' && line[i + 1] == '=') || (line[i] == '.' && line[i + 1] == '.')) {
				tokens.push_back(token);
				token = line[i];
				token += line[i + 1];
				tokens.push_back(token);
				token = "";
				i = i + 1;
				continue;
			}
			if (token == "/*") {
				tokens.push_back(token);
				token = line[i];
				continue;
			}
			if (i + 1 == lineLength) {
				token += line[i];
				tokens.push_back(token);
			}
			else if (line[i] == ' ' && line[i + 1] == ' ') {
				if (i < lineLength) {
					continue;
				}
				else if (token == "") {
					continue;
				}
			}
			else if (line[i] == '\t') {
				continue;
			}
			else if (line[i] == ' ' && line[i + 1] != ' ') {
				tokens.push_back(token);
				token = "";
				continue;
			}
			else if (line[i] != ' ' && line[i + 1] != ' ') {
				token += line[i];
			}
			else if (line[i] != ' ' && line[i + 1] == ' ') {
				token += line[i];
			}
		}
	}
	for (int i = 0; i < tokens.size(); i++) {
		if (tokens[i] == "") {
			tokens.erase(tokens.begin() + i);
		}
	}

	return tokens;
}

int main(int argc, char* argv[])
{/*
	if (argc < 2) {
		std::cout << "Please provide a file for scanning." << std::endl;
		return -1;
	}*/

	scanner sc = scanner();

	std::string filePath = "C:\\Users\\swree\\Desktop\\testFile.txt";
	std::vector<std::string> tokens = getTokens(filePath);
	
	sc.scan(tokens);

	std::cout << "\ntoken list\n" << std::endl;

	for (auto token : tokens) {
		std::cout << token << std::endl;
	}
}