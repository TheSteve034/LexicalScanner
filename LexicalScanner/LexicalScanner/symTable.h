#pragma once
#include <string>
#include <vector>
constexpr int MAXSYMBOLCOUNT = 100;

class symTable {

private:
	std::vector<int> scope;
	int level = 0;

	struct symbolInfo{
		std::string name;
		std::string storageType;
		std::string baseType;
		int scope;
		int arrayDimensions;
		struct symbolInfo* next;
	} *block[MAXSYMBOLCOUNT];

	int generateHash(std::string name);
public:
	void insertSymbol(struct symbolInfo);
	int searchForSymbol(std::string name, int scope, std::string sType, std::string type);
	int deleteSymbol(std::string name, int scope, std::string sType, std::string type);
	int updateSymbol(std::string name, int scope, std::string sType, std::string type);
};

