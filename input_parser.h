
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
	set<string> operators = { "|", " ", "*", "+", "-" };
	map<string, int> precedence = { { "|", 1 }, { " ", 2 }, { "*", 3 }, { "+", 3 } };;
	map<string, NFA*> basicExps;
	map<string, string> regDefs;
	string removeSpaces(string inStr);
	vector<string> tokenize(string regDef);
	vector<string> transformToCanonicalReg(vector<string> tokens);
	NFA buildNFA(vector<string> tokens);
	void handleRegDef(string line, int equalIdx);

public:
	InputParser(string inputPath);
	void parse();

};

#endif /* INPUT_PARSER_H_ */
