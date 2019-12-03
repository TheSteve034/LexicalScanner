#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "scanner.h"
#include "symTable.h"
#include "codeGenerator.h"


class parser {
private:
	scanner sc = scanner();
	codeGen cg = codeGen();
	symTable st = symTable();
	std::vector<std::string> tokens;
	std::vector<std::string> assingment;
	std::vector<std::string> mulFactorCalc;
	int tokenCount = 0;
	int varCount = 0;
	int aStart = 0;
	int aEnd = 0;
	int aSize = 0;
	int lineCount = 0;
	int varGenCount = 0;
	int varGenLocalCount = 0;
	int compGenCount = 0;
	bool inProc = false;
	bool ediInuse = false;
	bool mulFactor = false;
	bool inAddOp = false;
	bool inWriteRule = false;
	bool inParens = false;
	bool assingmentDone = false;
	bool checkedOnce = false;
	int procCount = 0;
	std::string lastOperand = "";
	std::string operation = "";
	std::string procName = "";
	std::string lValue = "";
	std::string rValue = {};
	std::string lAddOp = "";
	std::string op = "";
	std::string addOppLval = "";
	std::string addOppRval = "";
	std::string rValueP1 = "";
	std::string storageType = "";
	std::string baseType = "";
	std::string passType = "Value";
	std::vector<std::string> symIds = {};
	std::string currToken = "";
	std::string nextToken = "";
	std::fstream assembly = {};
	std::fstream error = {};
	void resetSymVars();
	void resetLandRVars();

	int programRule();
	int blockRule();
	int varDecRule();
	int moreDeclsRule();
	//int moreVarsRule();
	int arrayDefRule();
	int indexRangeRule();
	int indexListRule();
	int procDeclPartRule();
	int procDeclRule();
	int paramListsRule();
	int paramPassingRule();
	int passByValue();
	int passByReference();
	int moreParamsRule();
	int statmentPartRule();
	int compoundStatmentRule();
	int statmentRule();
	int simpleStatmentRule();
	int assingmentStatmentRule();
	int procedureCallRule();
	int argListRule();
	int moreArgListRule();
	int variableRule();
	int expressionRule();
	int simpleExpressionRule();
	int relExpressionRule();
	int termRule();
	int factorRule();
	int mulFactorRule();
	int moreStatmentsRule();
	int indexedVarRule();
	int arrayIdxRule();
	int addTermRule();
	int structuredStatmentRule();
	int whileStatmentRule();
	int ifStatmentRule();
	int elseRule();
	int writeRule();
	int readRule();
	int caseRule();
	int casePartRule();

	int isValidAssingmnet();


public:
	//gathers the tokens from the scanner.
	void getTokenList(std::vector<std::string> scanedTokens);
	//sets the value of currToken and nexToken using the tokenCount number;
	void getNextToken();
	//creates .asm and .err files
	void createFiles(std::string progName);
	int parseFile();
	void printTable();
};
