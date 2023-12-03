
#ifndef INPUT_PARSER_H_
#define INPUT_PARSER_H_

#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <set>
#include <map>
#include <assert.h>

#include "NFA.h"

using namespace std;

class InputParser {
private:
	string inPath;
	set<char> alphabet;
	const set<string> keywords = {"boolean", "float", "int", "if", "while", "else"};
	const set<string> operators = { "|", " ", "*", "+", "-", "(", ")" };
	map<string, int> precedence = { { "|", 1 }, { " ", 2 }, { "*", 3 }, { "+", 3 }, { "(", 4 } };
	map<string, NFA> basicExps;
	map<string, string> regDefs;
	vector<NFA> nfas;
	NFA combinedNFA;
	string removeSpaces(string inStr);
	vector<string> tokenize(string regDef);
	vector<string> transformToCanonicalReg(vector<string> tokens);
	vector<string> addConcatSymbol(vector<string> tokens);
	NFA buildNFA(vector<string> tokens);
	void nfaStep(stack<string>& operatorsStack, stack<NFA>& operandsStack);
	void handleKeywords(string line);
	void handlePunctuations(string line);
	void handleRegDef(string line, int equalIdx);
	void handleRegEx(string line, int colonIdc);

public:
	InputParser(string inputPath);
	void parse();
	NFA getCombinedNFA();

};

#endif /* INPUT_PARSER_H_ */
