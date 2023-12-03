#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <set>
#include <map>
#include <assert.h>

#include "input_parser.h"
#include "NFA.h"

using namespace std;

string InputParser::removeSpaces(string inStr)
{
	string outStr = "";
	for (auto ch : inStr)
	{
		if (ch == ' ')
			continue;
		outStr += ch;
	}
	return outStr;
}

vector<string> InputParser::tokenize(string regDef)
{
	int regDefLen = regDef.size();
	vector<string> tokens;
	string currToken = "", validToken = "";
	int currIdx = 0, validIdx = -1;
	while (validIdx < regDefLen - 1)
	{
		if (currIdx == regDefLen)
		{
			tokens.push_back(validToken);
			if (regDefs[validToken].empty())
			{
				alphabet.insert(validToken[0]);
			}
			validToken = "";
			currToken = "";
			currIdx = validIdx + 1;
		}
		string ch = string(1, regDef[currIdx]);
		if (operators.count(ch) && validToken.empty())
		{
			validIdx = currIdx;
			currIdx++;
			tokens.push_back(ch);
		}
		else if (operators.count(ch))
		{
			tokens.push_back(validToken);
			if (regDefs[validToken].empty())
			{
				alphabet.insert(validToken[0]);
			}
			validToken = "";
			currToken = "";
			currIdx = validIdx + 1;
		}
		else
		{
			currToken += ch;
			currIdx++;
			if (!regDefs[currToken].empty() || currToken.size() == 1)
			{
				validToken = currToken;
				validIdx = currIdx - 1;
			}
		}
	}
	if (!validToken.empty())
		tokens.push_back(validToken);
	return tokens;
}

vector<string> InputParser::transformToCanonicalReg(vector<string> tokens)
{
	vector<string> canonicalTokens;
	int tokensLen = tokens.size();
	for (int i = 0; i < tokensLen; i++)
	{
		if (tokens[i] == "-")
		{
			if (i == 0 || i == tokensLen - 1)
				throw runtime_error("Invalid Position for -");

			// figure out whether operands are regular expressions or regular definitions.
			// operands of - must be symbols
			string start = tokens[i - 1], end = tokens[i + 1];
			if (regDefs[start].size() == 1)
				start = regDefs[start];
			else if (start.size() != 1 || !alphabet.count(start[0]))
				throw runtime_error("invalid first operand for -");
			if (regDefs[end].size() == 1)
				end = regDefs[end];
			else if (end.size() != 1 || !alphabet.count(end[0]))
				throw runtime_error("invalid second operand for -");

			if (start[0] > end[0])
				throw runtime_error("Invalid Range for -");
			else if (start[0] == end[0])
				i++; // skip end token
			else
			{
				canonicalTokens.push_back("|");
				for (char ch = start[0] + 1; ch < end[0]; ch++)
				{
					canonicalTokens.push_back(string(1, ch));
					canonicalTokens.push_back("|");
					alphabet.insert(ch);
				}
			}
		}
		else
			canonicalTokens.push_back(tokens[i]);
	}
	return canonicalTokens;
}

vector<string> InputParser::addConcatSymbol(vector<string> tokens)
{
	int n = tokens.size();
	vector<string> modifiedTokens;
	modifiedTokens.push_back(tokens[0]);
	for (int i = 1; i < n; i++)
	{
		if ((!operators.count(tokens[i]) && !operators.count(tokens[i - 1])) || (!operators.count(tokens[i]) && tokens[i - 1] == ")") || (tokens[i] == "(" && !operators.count(tokens[i - 1])))
		{
			modifiedTokens.push_back(" ");
		}
		modifiedTokens.push_back(tokens[i]);
	}
	return modifiedTokens;
}

void InputParser::nfaStep(stack<string> &operatorsStack, stack<NFA> &operandsStack)
{
	string op = operatorsStack.top();
	operatorsStack.pop();
	NFA finalNfa;
	if (op == "+")
	{
		NFA operand = operandsStack.top();
		operandsStack.pop();
		finalNfa = NFA::positiveClosure(operand);
	}
	else if (op == "*")
	{
		NFA operand = operandsStack.top();
		operandsStack.pop();
		finalNfa = NFA::kleeneClosure(operand);
	}
	else if (op == " ")
	{
		NFA operand2 = operandsStack.top();
		operandsStack.pop();
		NFA operand1 = operandsStack.top();
		operandsStack.pop();
		finalNfa = NFA::concatenate(operand1, operand2);
	}
	else if (op == "|")
	{
		NFA operand2 = operandsStack.top();
		operandsStack.pop();
		NFA operand1 = operandsStack.top();
		operandsStack.pop();
		finalNfa = NFA::Or(operand1, operand2);
	}
	else
		throw runtime_error("Unexpected Operator: " + op);
	operandsStack.push(finalNfa);
}

NFA InputParser::buildNFA(vector<string> tokens)
{
	stack<string> operatorsStack;
	stack<NFA> operandsStack;

	for (auto token : tokens)
	{
		if (operators.count(token))
		{
			if (token == ")")
			{
				while (operatorsStack.top() != "(")
				{
					nfaStep(operatorsStack, operandsStack);
				}
				operatorsStack.pop();
			}
			else
			{
				while (!operatorsStack.empty() && precedence[token] <= precedence[operatorsStack.top()] && operatorsStack.top() != "(")
				{
					nfaStep(operatorsStack, operandsStack);
				}
				operatorsStack.push(token);
			}
		}
		else if (!regDefs[token].empty())
		{ // regular definition
			operandsStack.push(basicExps[token]);
		}
		else
		{ // alphabet symbol
			NFA nfa;
			nfa.addSympol(token[0]);
			operandsStack.push(nfa);
		}
	}
	while (!operatorsStack.empty())
	{
		nfaStep(operatorsStack, operandsStack);
	}
	return operandsStack.top();
}

void InputParser::handleRegDef(string line, int equalIdx)
{
	string name = line.substr(0, equalIdx);
	string regDef = line.substr(equalIdx + 1);
	regDefs[name] = regDef;

	vector<string> tokens = addConcatSymbol(transformToCanonicalReg(tokenize(regDef)));
	NFA nfa = buildNFA(tokens);
	basicExps[name] = nfa;
}

void InputParser::handleRegEx(string line, int colonIdx)
{
	string name = line.substr(0, colonIdx);
	string regex = line.substr(colonIdx + 1);
	vector<string> tokens = addConcatSymbol(transformToCanonicalReg(tokenize(regex)));
	NFA nfa = buildNFA(tokens);
	nfas.push_back(nfa);
}

InputParser::InputParser(string inputPath)
{
	inPath = inputPath;
}

void InputParser::handleKeywords(string line)
{
	int n = line.size();
	string keyword;
	NFA nfa;
	for (int i = 1; i < n - 1; i++)
	{
		cout << keyword << endl;
		if (keyword.empty())
		{
			keyword += line[i];
			nfa.addSympol(line[i]);
		}
		else
		{
			keyword += line[i];
			NFA currSymbolNfa;
			currSymbolNfa.addSympol(line[i]);
			nfa = NFA::concatenate(nfa, currSymbolNfa);
		}
		if (keywords.count(keyword))
		{
			nfas.push_back(nfa);
			NFA newNfa;
			nfa = newNfa;
			keyword = "";
			cout << "Match keyword" << endl;
		}
	}
}

void InputParser::handlePunctuations(string line)
{
	int n = line.size();
	for (int i = 1; i < n - 1; i++)
	{
		if (line[i] == '\\')
			continue;
		NFA nfa;
		nfa.addSympol(line[i]);
		nfas.push_back(nfa);
	}
}
void InputParser::parse()
{
	string line;
	ifstream inFile(inPath);
	int lineNum = 0;
	while (getline(inFile, line))
	{
		lineNum++;
		line = removeSpaces(line);
		int n = line.size();
		if (line[0] == '\n')
			continue;
		if (line[0] == '{' && line[n - 1] == '}')
		{ // keywords
			handleKeywords(line);
		}
		else if (line[0] == '[' && line[n - 1] == ']')
		{ // punctuations
			handlePunctuations(line);
		}
		else
		{
			int equalIndex = line.find('=');
			int colonIndex = line.find(':');
			// regular definition
			if ((equalIndex != -1 && colonIndex != -1 && equalIndex < colonIndex) || (equalIndex != -1 && colonIndex == -1))
			{
				handleRegDef(line, equalIndex);
			}
			// regular expression
			else if ((equalIndex != -1 && colonIndex != -1 && colonIndex < equalIndex) || (colonIndex != -1 && equalIndex == -1))
			{
				handleRegEx(line, colonIndex);
			}
			// neither a regular definition nor a regular expression
			else
			{
				throw runtime_error(
					"line number " + to_string(lineNum) + " is invalid");
			}
		}
	}
	combinedNFA = NFA::combine(nfas);
}

NFA InputParser::getCombinedNFA()
{
	return combinedNFA;
}

int main()
{

	InputParser parser("input.txt");
	parser.parse();
	NFA nfa = parser.getCombinedNFA();
	// Retrieve and print transitions of the resulting NFA
	auto transitions = nfa.getStateTransitions();
	cout << "Transitions:" << endl;
	for (const auto &transition : transitions)
	{
		int fromState = transition.first;
		for (const auto &pair : transition.second)
		{
			int toState = pair.first;
			char input = pair.second;
			cout << "Transition: State " << fromState << " --(" << input << ")--> State " << toState << endl;
		}
	}

	cout << "**************************************************" << endl;

	// Retrieve and print epsilon transitions of the resulting NFA
	auto epsilonTransitions = nfa.getEpsilonTransitions();
	cout << "Epsilon Transitions:" << endl;
	for (const auto &epsilonTransition : epsilonTransitions)
	{
		int fromState = epsilonTransition.first;
		for (int toState : epsilonTransition.second)
		{
			cout << "Epsilon Transition: State " << fromState << " --(epsilon)--> State " << toState << endl;
		}
	}
}
