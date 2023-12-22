#include <iostream>
#include <fstream>
#include <assert.h>
#include <set>

#include "GrammarParser.h"

using namespace std;
using namespace Parser;

GrammarParser::GrammarParser(string inPath)
{
    parse(inPath);
    assert(verifyCompleteGrammar());
    for (auto terminal : terminalsSet)
        terminals.push_back(terminal);
    for (auto nonTerminal : derivedNonTerminalsSet)
        nonTerminals.push_back(nonTerminal);
}

bool GrammarParser::verifyCompleteGrammar()
{
    for (auto nonTerminal : usedNonTerminalsSet)
    {
        if (!derivedNonTerminalsSet.count(nonTerminal))
        {
            return false;
        }
    }
    return true;
}

void GrammarParser::parse(string inPath)
{
    ifstream inFile(inPath);
    string token;
    string currNonTerminal, rule;
    bool hasRule = false;
    bool expectRule = false;
    vector<string> rules;
    while (inFile >> token)
    {
        if (token == "#")
        { // new production
            assert(!expectRule);
            if (hasRule)
            {
                rules.push_back(rule);
                rule = "";
                hasRule = false;
            }
            if (!currNonTerminal.empty() && !rules.empty())
            {
                if (productions[currNonTerminal].empty())
                {
                    productions[currNonTerminal] = rules;
                }
                else
                {
                    productions[currNonTerminal].insert(productions[currNonTerminal].end(), rules.begin(), rules.end());
                }
            }
            inFile >> currNonTerminal;
            assert(!isTerminal(currNonTerminal));

            // first rule
            if (rules.empty())
                startSymbol = currNonTerminal;

            inFile >> token;
            assert(token == "=");
            rules.clear();
            expectRule = true;
            derivedNonTerminalsSet.insert(currNonTerminal);
        }
        else if (token == "|")
        {
            assert(!currNonTerminal.empty());
            assert(!expectRule);
            assert(hasRule);
            rules.push_back(rule);
            rule = "";
            hasRule = false;
            expectRule = true;
        }
        else if (token == "\\L")
        {
            assert(!currNonTerminal.empty());
            hasRule = true;
            expectRule = false;
        }
        else if (token[0] == '\\')
        {
            assert(!currNonTerminal.empty());
            assert(token.size() > 1);
            if (!rule.empty())
                rule += " ";
            rule += token.substr(1);
            hasRule = true;
            expectRule = false;
        }
        else
        {
            assert(!currNonTerminal.empty());
            if (!rule.empty())
                rule += " ";
            hasRule = true;
            expectRule = false;

            if (isTerminal(token))
            {
                token = token.substr(1, token.size() - 2);
                terminalsSet.insert(token);
                rule += token;
            }
            else
            {
                usedNonTerminalsSet.insert(token);
                rule += token;
            }
        }
    }

    assert(!expectRule);
    if (hasRule)
    {
        rules.push_back(rule);
        rule = "";
        hasRule = false;
    }
    if (!currNonTerminal.empty() && !rules.empty())
    {
        if (productions[currNonTerminal].empty())
        {
            productions[currNonTerminal] = rules;
        }
        else
        {
            productions[currNonTerminal].insert(productions[currNonTerminal].end(), rules.begin(), rules.end());
        }
    }
}

bool GrammarParser::isTerminal(string token)
{
    return token[0] == '\'' && token[token.size() - 1] == '\'';
}

vector<string> GrammarParser::getTerminals()
{
    return terminals;
}

vector<string> GrammarParser::getNonTerminals()
{
    return nonTerminals;
}

string GrammarParser::getStartSymbol()
{
    return startSymbol;
}

map<string, vector<string>> GrammarParser::getProductions()
{
    return productions;
}

int main()
{
    GrammarParser grammarParser("Grammar Rules.txt");
    for (auto p : grammarParser.getProductions())
    {
        cout << p.first << " -> ";
        for (auto rule : p.second)
        {
            if (rule.empty())
                cout << "EPSILON";
            else
                cout << rule;
            cout << " | ";
        }
        cout << endl;
    }
    cout << "_____________________________________________________________________" << endl;
    for (auto t : grammarParser.getTerminals())
        cout << t << " ";
    cout << endl;
    for (auto nt : grammarParser.getNonTerminals())
        cout << nt << " ";
    cout << endl;
    cout << grammarParser.getStartSymbol();
}