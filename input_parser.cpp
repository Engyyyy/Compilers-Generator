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
public:
	string inPath;
	set<char> alphabet;
	map<char, int> precedence = { { '|', 1 }, { ' ', 2 }, { '*', 3 },
			{ '+', 3 } };
	set<string> operators = { "|", " ", "*", "+", "-" };
	map<string, NFA*> basicExps;
	map<string, string> regDefs;
	string removeSpaces(string inStr) {
		string outStr = "";
		for (auto ch : inStr) {
			if (ch == ' ')
				continue;
			outStr += ch;
		}
		return outStr;
	}

	vector<string> tokenize(string regDef) {
		int regDefLen = regDef.size();
		vector<string> tokens;
		string currToken = "", validToken = "";
		int currIdx = 0, validIdx = -1;
        while (validIdx < regDefLen)
        {
		if (currIdx == regDefLen) {
			tokens.push_back(validToken);
			if(basicExps[validToken] == NULL) {
							alphabet.insert(validToken[0]);
						}
			validToken = "";
			currToken = "";
			currIdx = validIdx + 1;
		}
		string ch = string(1, regDef[currIdx]);
		if (operators.count(ch) && validToken.empty()) {
			validIdx = currIdx;
			currIdx++;
			tokens.push_back(ch);
		} else if (operators.count(ch)) {
			tokens.push_back(validToken);
			if(basicExps[validToken] == NULL) {
				alphabet.insert(validToken[0]);
			}
			validToken = "";
			currToken = "";
			currIdx = validIdx + 1;
		} else {
			currToken += ch;
			currIdx++;
			if (basicExps[currToken] != NULL || currToken.size() == 1) {
				validToken = currToken;
				validIdx = currIdx - 1;
			}
		}
	}
	return tokens;
}

	vector<string> transformToCanonicalReg(vector<string> tokens) {
		vector<string> canonicalTokens;
		int tokensLen = tokens.size();
		for(int i = 0; i < tokensLen; i++) {
			if(tokens[i] == "-") {
				if(i == 0 || i == tokensLen - 1) throw runtime_error("Invalid Position for -");

				// figure out whether operands are regular expressions or regular definitions.
				// operands of - must be symbols
				string start = tokens[i-1], end = tokens[i+1];
				if(regDefs[start].size() == 1) start = regDefs[start];
				else if(start.size() != 1 || !alphabet.count(start[0])) throw runtime_error("invalid first operand for -");
				if(regDefs[end].size() == 1) end = regDefs[end];
				else if(end.size() != 1 || !alphabet.count(start[0])) throw runtime_error("invalid second operand for -");

				if(start[0] > end[0]) throw runtime_error("Invalid Range for -");
				else if(start[0] == end[0]) i++;	// skip end token
				else {
					canonicalTokens.push_back("|");
					for(char ch = start[0] + 1; ch < end[0]; ch++) {
						canonicalTokens.push_back(string(1, ch));
						canonicalTokens.push_back("|");
						alphabet.insert(ch);
					}
				}
			}
			else canonicalTokens.push_back(tokens[i]);
		}
		return canonicalTokens;
	}


	NFA buildNFA(vector<string> tokens) {
		stack<string> operatorsStack;
		stack<NFA> operandsStack;

		for(auto token : tokens) {
			if(operators.count(token)) {
				if (operatorsStack.empty() || precedence[token] > precedence[operatorsStack.top()]) operatorsStack.push(token);
			}
			else if(!regDefs[token].empty()) { // regular definition
				operandsStack.push(*basicExps[token]);
			}
			else { // alphabet symbol
				NFA nfa;
				nfa.addTransition(0, 1, token[0]);
				operandsStack.push(nfa);
			}
		}
	}

    void handleRegDef(string line, int equalIdx)
    {
        string name = line.substr(0, equalIdx);
        string regDef = line.substr(equalIdx + 1);
        regDefs[name] = regDef;

        vector<string> tokens = transformToCanonicalReg(tokenize(regDef));
    }

public:
InputParser(string inputPath)
{
	inPath = inputPath;
}
void parse() {
        string regex;
        ifstream inFile(inPath);
        getline(inFile, regex);
        int lineNum = 0;
        regex = removeSpaces(regex);

        int equalIndex = regex.find('=');
        int colonIndex = regex.find(':');
        // regular definition
        if ((equalIndex != -1 && colonIndex != -1 && equalIndex < colonIndex) || (equalIndex != -1 && colonIndex == -1))
        {
            handleRegDef(regex, equalIndex);
        }
	 // regular expression
	     else if ((equalIndex != -1 && colonIndex != -1 && colonIndex < equalIndex) || (colonIndex != -1 && equalIndex == -1))
	     {
	     }
	     // neither a regular definition nor a regular expression
	     else
	     {
	         throw runtime_error("line number " + to_string(lineNum) + " is neither a regular definition nor a regular expression");
	     }
}
};

int main() {
InputParser parser("input.txt");
parser.parse();
}
