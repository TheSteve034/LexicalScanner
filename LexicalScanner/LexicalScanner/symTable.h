#pragma once
#include <string>
#include <vector>
#include <iostream>
constexpr int MAXSYMBOLCOUNT = 100;
constexpr int MAXSCOPECOUNT = 20;
const std::string _SIMVAR = "simple variable";

class symTable {

private:
	bool scope[MAXSCOPECOUNT] = { false };
	int lastScope = 0;
	int level = 0;
	int arraySize = 0;
	int generateHash(std::string name);
	bool firstProc = true;
public:
	struct symbolInfo {
		std::string name = "";
		std::string storageType= "";
		std::string baseType = "";
		std::string passType = "Value";
		int scope = 0;
		int arrayDimensions = 0;
		struct symbolInfo* next = NULL;
	} *block[MAXSYMBOLCOUNT];

	void insertSymbol(struct symbolInfo *var);
	void setFirstProc(bool val);
	bool getFirstProc();
	int getLevel();
	void setLevel();
	void moveLevelDown();
	void setArraySize(int size);
	int searchForSymbol(std::string name, int scope, std::string sType, std::string type);
	int deleteSymbol(std::string name, int scope, std::string sType, std::string type);
	int updateSymbol(std::string name, int scope, std::string sType, std::string type);
	int calucalteIndexRange(int start, int end);
	void insertSimpleSyms(std::vector<std::string> Ids, std::string sType, std::string bType, std::string passType);
	void insertProc(std::vector<std::string> Ids);
	void printInsertedVar(struct symbolInfo* var);
};

