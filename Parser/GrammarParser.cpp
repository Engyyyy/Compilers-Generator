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

    convertToPredictiveGrammar(PREDICTIVE_GRAMMAR_PROCEDURE::ELIMINATE_LEFT_RECURSION);
}

void GrammarParser::eliminateLeftRecursionStep(string Ai, vector<vector<string>> AiRules)
{
    // A -> Ab | Ac | d | f
    // A -> dA' | fA'
    // A' -> bA' | cA' | EPSILON
    vector<vector<string>> leftRecursionRules, LL1Rules;
    for (auto &AiRule : AiRules)
    {
        if (Ai == AiRule[0])
        { // left recursion
            leftRecursionRules.push_back(AiRule);
        }
        else
        {
            LL1Rules.push_back(AiRule);
        }
    }
    if (leftRecursionRules.empty())
        return;

    string newNonTerminal = Ai + "\'";
    vector<vector<string>> newNonTerminalRules;

    for (auto &rule : LL1Rules)
    {
        rule.push_back(newNonTerminal);
    }
    productions[Ai] = LL1Rules;

    for (auto &rule : leftRecursionRules)
    {
        vector<string> newRule;
        newRule.insert(newRule.end(), rule.begin() + 1, rule.end());
        newRule.push_back(newNonTerminal);
        newNonTerminalRules.push_back(newRule);
    }
    vector<string> epsilonRule = {""};
    newNonTerminalRules.push_back(epsilonRule);
    productions[newNonTerminal] = newNonTerminalRules;
}

void GrammarParser::leftFactorStep(string Ai, vector<vector<string>> AiRules)
{
}

void GrammarParser::convertToPredictiveGrammar(PREDICTIVE_GRAMMAR_PROCEDURE procedure)
{
    for (int i = 0; i < nonTerminals.size(); i++)
    {
        string Ai = nonTerminals[i];
        vector<vector<string>> AiRules = productions[Ai];
        for (int j = 0; j < i; j++)
        {
            string Aj = nonTerminals[j];
            for (int k = 0; k < AiRules.size(); k++)
            {
                vector<string> &AiRule = AiRules[k];
                if (AiRule[0] == Aj)
                {
                    vector<vector<string>> substitution = productions[Aj];
                    for (auto &rule : substitution)
                    {
                        rule.insert(rule.end(), AiRule.begin() + 1, AiRule.end());
                    }
                    AiRule = substitution[0];
                    AiRules.insert(AiRules.end(), substitution.begin() + 1, substitution.end());
                }
            }
        }
        if (procedure == ELIMINATE_LEFT_RECURSION)
            eliminateLeftRecursionStep(Ai, AiRules);
        else if (procedure == LEFT_FACTOR)
            leftFactorStep(Ai, AiRules);
    }
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
    string currNonTerminal;
    vector<string> rule;
    bool expectRule = false;
    vector<vector<string>> rules;
    while (inFile >> token)
    {
        if (token == "#")
        { // new production
            assert(!expectRule);
            if (!rule.empty())
            {
                rules.push_back(rule);
                rule.clear();
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
            assert(!rule.empty());
            rules.push_back(rule);
            rule.clear();
            expectRule = true;
        }
        else if (token == "\\L")
        {
            assert(!currNonTerminal.empty());
            rule.push_back("");
            expectRule = false;
        }
        else if (token[0] == '\\')
        {
            assert(!currNonTerminal.empty());
            assert(token.size() > 1);
            rule.push_back(token.substr(1));
            expectRule = false;
        }
        else
        {
            assert(!currNonTerminal.empty());
            expectRule = false;
            if (isTerminal(token))
            {
                token = token.substr(1, token.size() - 2);
                terminalsSet.insert(token);
                rule.push_back(token);
            }
            else
            {
                usedNonTerminalsSet.insert(token);
                rule.push_back(token);
            }
        }
    }

    assert(!expectRule);
    if (!rule.empty())
    {
        rules.push_back(rule);
        rule.clear();
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

map<string, vector<vector<string>>> GrammarParser::getProductions()
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
            for (auto token : rule)
            {
                if (token.empty())
                    cout << "EPOSILON";
                cout << token << " ";
            }
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