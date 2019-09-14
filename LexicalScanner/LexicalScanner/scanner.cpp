//Class definition for the scanner class
#include <algorithm>
#include "scanner.h"

std::regex idRE("[A-Z][A-Z 0-9 _]*");
std::regex singelLineCommenetRE("\/\/.*");
std::regex intConstantRE("[1-9][0-9]*");
std::regex stringConstantRE("\".*\"");
std::regex missingEndQuote("\"[A-Z ]+");

std::vector<std::string> scanner::getTokens(std::string filePath) {
	std::ifstream file(filePath.c_str(), std::ifstream::in);
	std::string line = "";
	std::vector<std::string> tokens = {};

	while (std::getline(file, line)) {
		std::string token = "";
		bool inStringConst = false;
		int lineLength = line.length();
		for (int i = 0; i < lineLength; i++) {
			//Checks for all two character tokens. In the case of a two char token a token will be made of the currently
			//checked char and the next and the loop variable will be incremented. Two char tokens are defined as special 
			//symbols. This section will also test for symbols that start coments. This will be tracked so that the scanner 
			//method can discard them.

			//tests for the "/*" token
			if (line[i] == '/' && line[i + 1] == '*') {
				token = line[i];
				token += line[i + 1];
				tokens.push_back(token);
				token = "";
				i = i + 1;
				continue;
			}
			//tests for the "*/" token
			if (line[i] == '*' && line[i + 1] == '/') {
				token = line[i];
				token += line[i + 1];
				tokens.push_back(token);
				token = "";
				i = i + 1;
				continue;
			}
			//tests fot he "//" token
			if (line[i] == '/' && line[i + 1] == '/') {
				token = line[i];
				token += line[i + 1];
				tokens.push_back(token);
				token = "";
				i = i + 1;
				continue;
			}
			//tests for all special two char tokens as decribed by the grammar
			if ((line[i] == '<' && line[i + 1] == '>') || (line[i] == '<' && line[i + 1] == '=') || (line[i] == '>' && line[i + 1] == '=')
				|| (line[i] == ':' && line[i + 1] == '=') || (line[i] == '.' && line[i + 1] == '.')) {
				if (token.empty()) {
					token = line[i];
					token += line[i + 1];
					tokens.push_back(token);
					token = "";
					i = i + 1;
					continue;
				}
				else {
					tokens.push_back(token);
					token = line[i];
					token += line[i + 1];
					tokens.push_back(token);
					token = "";
					i = i + 1;
					continue;
				}
			}

			//check for all on char special tokens. These tokens as defined by the grammar.
			if (line[i] == '+' || line[i] == '-' || line[i] == '*' || line[i] == '=' || line[i + 1] == '<' || line[i + 1] == '>'
				|| line[i] == '(' || line[i] == ')' || line[i] == '[' || line[i] == ']' || line[i] == '.' || line[i] == ','
				|| line[i] == ';' || line[i] == '\'' || line[i] == '/' || line[i] == ':') {
				if (token.empty()) {
					token = line[i];
					tokens.push_back(token);
					token = "";
					continue;
				}
				else {
					tokens.push_back(token);
					token = line[i];
					tokens.push_back(token);
					token = "";
					continue;
				}
			}

			/*
			now all remaning chars will be processed as follows:
			if char is not white space and the next char is also not white space add char to token.

			if char is not white space and the nexr char is white space add char to the token and push
			the token into token. Then set token to empty

			if char is white space skip it
			*/
			if (line[i] != ' ' && line[i] != '\t') {
				token += line[i];
				continue;
			}
			if (line[i] == ' ' || line[i] == '\t') {
				if (token.empty()) {
					continue;
				}
				else {
					tokens.push_back(token);
					token = "";
					continue;
				}
			}

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
	if (token == "\"") {
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
	std::string tempStringConst = "";
	/*for (auto& token : tokens) {
		if (isSingelLineComment(token) && (inComment == false && inStringConst == false)) {
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
		if (isStringConstStart(token) && inStringConst == false) {
			inStringConst = true;
			tempStringConst += token;
			continue;
		}
		if (isStringContEnd(token) && inStringConst == true) {
			inStringConst = false;
			tempStringConst += token;
			isStringConst(tempStringConst);
			tempStringConst = "";
			continue;
		}
		if (inStringConst) {
			std::regex numORChar("[A-Z0-9]*");
			std::string capToken = toCaps(token);
			if (std::regex_match(capToken, numORChar)) {
				if (tempStringConst == "\"") {
					tempStringConst += capToken;
				}
				else {
					tempStringConst += " ";
					tempStringConst += capToken;
				}
			}
			else {
				tempStringConst += capToken;
			}
			continue;
		}
		if (inComment) {
			continue;
		}
		if (isReserved(token) && (inComment == false && inStringConst == false)) {
			lexemeFound = true;
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
		if (inStringConst == true || inComment == true) {
			lexemeFound = false;
		}
		lexemeFound = false;
		if (lexemeFound == false) {
			std::cout << "ILLEGAL TOKEN\t" + token << std::endl;
		}
	}*/

	std::cout << "Tokens" << std::endl;
	for (auto& token : tokens) {
		std::cout << token << std::endl;
	}
}
