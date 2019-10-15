#include "symTable.h"

/*
This method finds the bucket that a symbol should be in.
*/
int symTable::generateHash(std::string name) {
	int idx = 0;
	for (int i = 0; name[i]; ++i) {
		idx = idx + name[i];
	}
	return (idx % MAXSYMBOLCOUNT);
}

/*
This method adds a symbol to it's appropirate bucket. If the bucket is empty then it jsut adds a

*/
void symTable::insertSymbol(symbolInfo* var) {
	int pos = generateHash(var->name);

	if (block[pos] == NULL) {
		block[pos] = new symbolInfo();
		block[pos]->name = var->name;
		block[pos]->storageType = var->storageType;
		block[pos]->baseType = var->baseType;
		block[pos]->scope = var->scope;
		block[pos]->arrayDimensions = var->arrayDimensions;
		block[pos]->next = NULL;
	}
	else {
		symbolInfo* newNode = new symbolInfo();
		newNode->name = var->name;
		newNode->storageType = var->storageType;
		newNode->baseType = var->baseType;
		newNode->scope = var->scope;
		newNode->arrayDimensions = var->arrayDimensions;

		// pointer swap
		symbolInfo* nextNode = block[pos];
		block[pos] = newNode;
		newNode->next = nextNode;
	}
}

/*
this method allows us to search a given bucket for a symbol. We make sure that we match names, types, and scopes
*/
int symTable::searchForSymbol(std::string name, int scope, std::string sType, std::string type) {
	int chain = generateHash(name);

	symbolInfo* temp = block[chain];

	while (temp != NULL) {
		if (std::strcmp(temp->name.c_str(), name.c_str()) && std::strcmp(temp->storageType.c_str(), sType.c_str()) && std::strcmp(temp->baseType.c_str(), type.c_str()) && temp->scope == scope) {
			return 0;
		}
		temp = temp->next;
	}
	return -1;
}

int symTable::deleteSymbol(std::string name, int scope, std::string sType, std::string type) {
	return 0;
}

int symTable::updateSymbol(std::string name, int scope, std::string sType, std::string type) {
	//int position = generateHash(name);

	//symbolInfo* temp = block[position];

	//while (temp != NULL) {
	//	if (std::strcmp(temp->name.c_str(), name.c_str()) && std::strcmp(temp->storageType.c_str(), sType.c_str()) && std::strcmp(temp->baseType.c_str(), type.c_str()) && temp->scope == scope) {
	//		return 0;
	//	}
	//	temp = temp->next;
	//}
	return 0;
}

int symTable::getLevel() {
	return level;
}
void symTable::setLevel(int level) {
	this->level = this->level + level;
}

/*
This method is used to insert the following var types:
INT STRING and BOOLEANS
*/
void symTable::insertSimpleSyms(std::vector<std::string> Ids, std::string sType, std::string bType) {
	if (Ids.empty()) {
		return;
	}
	for (auto& symbol : Ids) {
		struct symTable::symbolInfo* var = new symbolInfo();
		var->name = symbol;
		var->storageType = sType;
		var->baseType = bType;
		var->scope = level;
		var->arrayDimensions = arraySize;
		insertSymbol(var);
		printInsertedVar(var);
	}
}

int symTable::calucalteIndexRange(int start, int end) {
	//we need to handle three cases here
	//1 - both end points are positive
	//2 - both end point are negative
	//3 - one end point is negative and the other is positive.
	//4 - case where one or both of the end points are 0.

	//case 4
	if (start == 0 && end == 0) {
		return 0;
	}
	if (start > 0 && end == 0) {
		return start;
	}
	if (start < 0 && end == 0) {
		return -1 * start;
	}
	if (start == 0 && end > 0) {
		return end;
	}
	if (start == 0 && end < 0) {
		return -1 * end;
	}
	//case 1
	if (start > 0 && end > 0) {
		return end - start;
	}
	//case 2
	if (start < 0 && end < 0) {
		start = start * -1;
		end = end * -1;
		return end - start;
	}
	//case 3
	if( start < 0 && end >= 0) {
 		return end + -1*start;
	}
	else if (start >= 0 && end < 0) {
		return start + -1*end;
	}
}

void symTable::setArraySize(int size) {
	arraySize = size;
}

void symTable::printInsertedVar(struct symbolInfo* var) {
	std::cout << "INSERTED SYMBOL" << std::endl;
	std::cout << "Name: " + var->name << std::endl;
	std::cout << "Base Type: " + var->baseType << std::endl;
	std::cout << "Storage Type: " +var->storageType << std::endl;
	std::cout << "Scope: " + std::to_string(var->scope) << std::endl;
	std::cout << "Array Dimensions: " + std::to_string(var->arrayDimensions) << std::endl;
}