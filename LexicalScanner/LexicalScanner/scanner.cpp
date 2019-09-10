//Class definition for the scanner class
#include <algorithm>
#include "scanner.h"

std::regex idRE("[A-Z][A-Z 0-9 _]*");
std::regex singelLineCommenetRE("\/\/.*");
std::regex intConstantRE("[1-9][0-9]*");
std::regex stringConstantRE("\"[A-Z]+\"");

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

bool scanner::isIntConst(std::string token) {
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
	for (auto& token : tokens) {
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
	}
}
