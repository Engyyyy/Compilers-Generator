#ifndef ParserOutput_H_
#define ParserOutput_H_

#include <utility>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <fstream>
#include <sstream>
#include <iterator>
#include <stack>

#include "../Lexical Analyzer/input handler.h"

using namespace std;

namespace Parser
{
    class ParserOutput
    {
    private:
        map<pair<string, string>, vector<string>> mp;
        stack<string> inputs;
        stack<string> st;
        LexicalAnalyzer::tokenGenerator t;
        vector<vector<string>> out;
        string start;
        vector<string> terminals;
        vector<string> nonTerminals;

    public:
        ParserOutput(vector<string> term, vector<string> nonterm, string sta, LexicalAnalyzer::tokenGenerator to, const map<pair<string, string>, vector<string>> m);
        void getNextInput();
        void run();
        vector<vector<string>> getOut;
    };
}

#endif // Parser_output_H_
