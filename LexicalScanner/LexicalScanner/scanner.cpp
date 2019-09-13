//Class definition for the scanner class
#include <algorithm>
#include "scanner.h"

std::regex idRE("[A-Z][A-Z 0-9 _]*");
std::regex singelLineCommenetRE("\/\/.*");
std::regex intConstantRE("[1-9][0-9]*");
std::regex stringConstantRE("\"[A-Z ]*\"");
std::regex missingEndQuote("\"[A-Z ]*");

std::vector<std::string> scanner::getTokens(std::string filePath) {
	std::ifstream file(filePath.c_str(), std::ifstream::in);
	std::string line = "";
	std::vector<std::string> tokens = {};

	while (std::getline(file, line)) {
		std::string token = "";
		bool inStringConst = false;
		int lineLength = line.length();
		for (int i = 0; i < lineLength; i++) {
			if (line[i] == '/' && line[i + 1] == '*') {
				if (token != "") {
					tokens.push_back(token);
					token = "";
					token += line[i];
					token += line[i + 1];
					tokens.push_back(token);
					i = i + 1;
					break;
				}
				else if (token == "") {
					tokens.push_back(token);
					token = "";
					token += line[i];
					token += line[i + 1];
					tokens.push_back(token);
					i = i + 1;
					break;
				}
			}
			if (line[i] == '*' && line[i + 1] == '/') {
				if (token != "") {
					tokens.push_back(token);
					token = "";
					token += line[i];
					token += line[i + 1];
					tokens.push_back(token);
					i = i + 1;
					break;
				}
				else if (token == "") {
					tokens.push_back(token);
					token = "";
					token += line[i];
					token += line[i + 1];
					tokens.push_back(token);
					i = i + 1;
					break;
				}
			}
			if (line[i] == '/' && line[i + 1] == '/') {
				if (token != "") {
					tokens.push_back(token);
					token = "";
					for (int j = i; j < lineLength; j++) {
						token += line[j];
					}
					tokens.push_back(token);
					i = i + 1;
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
				|| line[i] == ';' || line[i] == '\'' || line[i] == '/') {
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
			if (line[i] == '\"' && inStringConst == false) {
				if (token != "") {
					tokens.push_back(token);
					token = line[i];
				}
				else {
					token = line[i];
					inStringConst = true;
				}
				inStringConst = true;
				continue;
			}
			if (inStringConst == true && line[i + 1] != '\"') {
				token += line[i];
				continue;
			}
			if (inStringConst == true && line[i + 1] == '\"') {
				token += line[i];
				token += line[i + 1];
				tokens.push_back(token);
				token = "";
				i = i + 1;
				continue;
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


std::string scanner::toCaps(std::string token) {
	std::string capString = "";
	for (int i = 0; i < token.size(); i++) {
		capString+= std::toupper(token[i]);
	}
	return capString;
}

bool scanner::isReserved(std::string token) {
	bool isReserved = false;
	for (const auto & reservedWord : reservedWords) {
		std::string capToken = toCaps(token);
		if (capToken == reservedWord) {
			std::cout << "RESERVED WORD\t" + capToken << std::endl;
			isReserved = true;
			break;
		}
	}
	return isReserved;
}

bool scanner::isSingelLineComment(std::string token) {
	if (std::regex_match(token, singelLineCommenetRE)) {
		return true;
	}
	else {
		return false;
	}
}

bool scanner::isMultiLineCommentStart(std::string token) {
	if (token == "/*") {
		return true;
	}
	else {
		return false;
	}
}

bool scanner::isMultiLineCommentEnd(std::string token) {
	if (token == "*/") {
		return true;
	}
	else {
		return false;
	}
}

bool scanner::isSpecial(std::string token) {
	bool isSpecial = false;
	for (const auto& specialChar : specialSymbols) {
		if (token == specialChar) {
			std::cout << "SPECIAL SYMBOL\t" + token << std::endl;
			return true;
			break;
		}
	}
	return isSpecial;
}

bool scanner::isStringConst(std::string token) {
	std::string capToken = toCaps(token);
	if (std::regex_match(capToken, stringConstantRE)) {
		std::cout << "STRING CONSTANT\t" + capToken << std::endl;
		return true;
	}
	else {
		return false;
	}
}

bool scanner::isStringConstStart(std::string token) {
	std::string captoken = toCaps(token);
	if (std::regex_match(captoken, missingEndQuote)) {
		return true;
	}
	else {
		return false;
	}
}

bool scanner::isStringContEnd(std::string token) {
	if (token == "\"") {
		return true;
	}
	else {
		return false;
	}
}

bool scanner::isIntConst(std::string token) {
	if (token == "0") {
		std::cout << "INTEGER CONSTANT\t" + token << std::endl;
		return true;
	}
	if (std::regex_match(token, intConstantRE)) {
		std::cout << "INTEGER CONSTANT\t" + token << std::endl;
		return true;
	}
	else {
		return false;
	}
}

bool scanner::isId(std::string token) {
	std::string capToken = toCaps(token);
	if (std::regex_match(capToken, idRE)) {
		std::cout << "IDENTIFIER\t" + capToken << std::endl;
		return true;
	}
	else {
		return false;
	}
}

void scanner::scan(std::vector<std::string> tokens) {
	bool lexemeFound = false;
	bool inComment = false;
	bool inStringConst = false;
	for (auto& token : tokens) {
		std::string tempStringConst = "";
		if (isReserved(token)) {
			lexemeFound = true;
			continue;
		}
		if (isSingelLineComment(token)) {
			lexemeFound = false;
			continue;
		}
		if (isMultiLineCommentStart(token)) {
			lexemeFound = false;
			inComment = true;	
		}
		if (isMultiLineCommentEnd(token)) {
			lexemeFound = false;
			inComment = false;
			continue;
		}
		if (inComment) {
			continue;
		}
		if (isSpecial(token)) {
			lexemeFound = true;
			continue;
		}
		if (isId(token)) {
			lexemeFound = true;
			continue;
		}
		if (isIntConst(token)) {
			lexemeFound == true;
			continue;
		}
		if (isStringConst(token)) {
			lexemeFound == true;
			continue;
		}
		lexemeFound = false;
		if (lexemeFound == false) {
			std::cout << "ILLEGAL TOKEN\t" + token << std::endl;
		}
	}

	std::cout << "\n\n" << std::endl;
	for (auto& token : tokens) {
		std::cout << token << std::endl;
	}
}
