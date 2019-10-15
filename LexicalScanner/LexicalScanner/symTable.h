#pragma once
#include <string>
#include <vector>
#include <iostream>
constexpr int MAXSYMBOLCOUNT = 100;
const std::string _SIMVAR = "simple variable";

class symTable {

private:
	std::vector<int> scope;
	int level = 0;
	int arraySize = 0;
	int generateHash(std::string name);
public:
	struct symbolInfo {
		std::string name = "";
		std::string storageType= "";
		std::string baseType = "";
		int scope = 0;
		int arrayDimensions = 0;
		struct symbolInfo* next = NULL;
	} *block[MAXSYMBOLCOUNT];

	void insertSymbol(struct symbolInfo *var);
	int getLevel();
	void setLevel(int level);
	void setArraySize(int size);
	int searchForSymbol(std::string name, int scope, std::string sType, std::string type);
	int deleteSymbol(std::string name, int scope, std::string sType, std::string type);
	int updateSymbol(std::string name, int scope, std::string sType, std::string type);
	int calucalteIndexRange(int start, int end);
	void insertSimpleSyms(std::vector<std::string> Ids, std::string sType, std::string bType);
	void printInsertedVar(struct symbolInfo* var);
};

