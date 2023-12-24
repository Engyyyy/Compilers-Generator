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
        enum PREDICTIVE_GRAMMAR_PROCEDURE
        {
            ELIMINATE_LEFT_RECURSION,
            LEFT_FACTOR
        };
        vector<string> terminals;
        vector<string> nonTerminals;
        map<string, vector<vector<string>>> productions;
        string startSymbol;
        set<string> terminalsSet, derivedNonTerminalsSet, usedNonTerminalsSet;
        void parse(string inPath);
        bool isTerminal(string token);
        bool verifyCompleteGrammar();
        void convertToPredictiveGrammar(PREDICTIVE_GRAMMAR_PROCEDURE procedure);
        void eliminateLeftRecursionStep(string Ai, vector<vector<string>> AiRules);
        void leftFactorStep(string Ai, vector<vector<string>> AiRules);

    public:
        vector<string> getTerminals();
        vector<string> getNonTerminals();
        string getStartSymbol();
        map<string, vector<vector<string>>> getProductions();
        GrammarParser(string inPath);
        void parse();
    };
}

#endif /* GrammarParser_H_ */