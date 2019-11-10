#include "codeGenerator.h"

std::string codeGen::getExportHeader() {
	return exports;
}
std::string codeGen::getImportHeader() {
	return imports;
}
std::string codeGen::getiDataHeader() {
	return iDataHeader;
}
std::string codeGen::getuDataHeader() {
	return uDataHeader;
}
std::string codeGen::getCodeDataHeader() {
	return codeHeader;
}

/*
this method returns the assemby code needed to add an integer variable. This is used to add 
varaibles from a var section of the source code.
*/
std::string codeGen::addIntVar(std::string varName) {
	std::string line = "\t_" + varName + "\t\tresd\t1\t;an int";
	return line;
}
