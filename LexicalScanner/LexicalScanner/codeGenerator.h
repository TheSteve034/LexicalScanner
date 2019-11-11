#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class codeGen {
private:
	std::string exports = ";--------------------------------\n;exports\n;--------------------------------\nglobal _main\nEXPORT _main\n";
	std::string imports = ";--------------------------------\n;imports\n;--------------------------------\nextern _printf\nextern _ExitProcess@4\nextern _scanf\n";
	std::string iDataHeader = ";--------------------------------\n;initialized data\n;--------------------------------\nsection .data USE32\n\tnumberPrinter	db \" % d\",0x0d,0x0a,0\n\tstringPrinter	db \" % s\",0\n";
	std::string uDataHeader = ";--------------------------------\n;uninitialized data\n;--------------------------------\nsection .bss USE32\n";
	std::string codeHeader = ";--------------------------------\n;Code Execution starts at _main\n;--------------------------------\nsection .code USE32\n";
public :
	std::string getExportHeader();
	std::string getImportHeader();
	std::string getiDataHeader();
	std::string getuDataHeader();
	std::string getCodeDataHeader();

	std::string addIntVar(std::string varName);
	std::string integerAssingment(std::string lVal, std::string rVal, int constFlag);
};
