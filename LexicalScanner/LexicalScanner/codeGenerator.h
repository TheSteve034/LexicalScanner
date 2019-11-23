#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class codeGen {
private:
	int compGenVarCount = 0;
	std::string exports = ";--------------------------------\n;exports\n;--------------------------------\nglobal _main\nEXPORT _main\n";
	std::string imports = ";--------------------------------\n;imports\n;--------------------------------\nextern _printf\nextern _ExitProcess@4\nextern _scanf\n\nsection .code USE32\n";
	std::string iDataHeader = ";--------------------------------\n;initialized data\n;--------------------------------\nsection .data USE32\n\tnumberPrinter	db \" %d\",0x0d,0x0a,0\n\tstringPrinter	db \" %s\",0x0d,0x0a,0\n\tstringReader	db \" %s\",0\n\tnumberReader	db \" %d%*c\",0\n"; //db \" % d\",/*0x0d,*/0
	std::string uDataHeader = ";--------------------------------\n;uninitialized data\n;--------------------------------\nsection .bss USE32\n";
	std::string codeHeader = ";--------------------------------\n;Code Execution starts at _main\n;--------------------------------";
	std::string exitHeader = "\nexit:\n\t\tmov\teax, 0x0\n\t\tcall\t_ExitProcess@4\n";
public :
	int compGenVars = 0;
	std::vector <std::string> uVar = {};
	std::vector <std::string> iVar = {};
	std::string getExportHeader();
	std::string getImportHeader(); 
	std::string getiDataHeader();
	std::string getuDataHeader();
	std::string getCodeDataHeader();
	std::string getExitHeader();

	std::string addIntVar(std::string varName);
	std::string addSringVar(std::string varName);
	std::string addIStringVar(std::string varName, int varNum);
	std::string integerAssingment(std::string lVal, std::string rVal, int constFlag);
	std::string writeCode(std::string var, int numFlag, int varNumFlag, std::string procName, int compCount, bool inProc,std::string currToken);
	std::string readCode(std::string var, std::string type);
	std::string generateLable(std::string labelName);
};
