//Class definition for the scanner class
#include <algorithm>
#include "scanner.h"

std::regex idRE("[A-Z][A-Z 0-9 _]*");
std::regex singelLineCommenetRE("\/\/.*");

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
		std::cout << "SINGLE LINE COMMENT\t" + token << std::endl;
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
	for (auto& token : tokens) {
		if (isReserved(token)) {
			lexemeFound = true;
			continue;
		}
		if (isSingelLineComment(token)) {
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
	}
}
