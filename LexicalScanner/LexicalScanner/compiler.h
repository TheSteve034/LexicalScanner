#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


std::vector<std::string> tokens;
int tokenCount = 0;
int lineCount = 0;

std::string getNextToken();