#ifndef GrammarParser_H
#define GrammarParser_H

#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

namespace Parser
{
    class GrammarParser
    {
    private:
        vector<string> terminals;
        vector<string> nonTerminals;
        map<string, vector<string>> productions;
        string startSymbol;
        set<string> terminalsSet, derivedNonTerminalsSet, usedNonTerminalsSet;
        void parse(string inPath);
        bool isTerminal(string token);
        bool verifyCompleteGrammar();

    public:
        vector<string> getTerminals();
        vector<string> getNonTerminals();
        string getStartSymbol();
        map<string, vector<string>> getProductions();
        GrammarParser(string inPath);
        void parse();
    };
}

#endif /* GrammarParser_H_ */