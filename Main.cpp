#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include "Lexical Analyzer/NFA.h"
#include "Lexical Analyzer/DFA.h"
#include "Lexical Analyzer/input handler.h"
#include "Lexical Analyzer/input_parser.h"
#include "Parser/GrammarParser.h"
#include "Parser/FirstFollow.h"
#include "Parser/ParserOutput.h"
#include "Parser/ParsingTable.h"

using namespace std;
using namespace LexicalAnalyzer;
using namespace Parser;

int main()
{

    //------------------------------------- 1. input parsing -------------------------------------//
    InputParser parser("./f2.txt");
    parser.parse();

    //------------------------------------------ 2. NFA ------------------------------------------//
    NFA nfa = parser.getCombinedNFA();
    map<int, string> priorityStrings = parser.getPriorityStrings();
    map<pair<int, char>, set<int>> nfaTransitions = nfa.getNfaTransitions();
    set<char> alphabet = nfa.getAlphabet();
    map<int, int> nfaFinalStates = nfa.getStatePriorities();

    //------------------------------------------ 3. DFA ------------------------------------------//
    DFA dfa = DFA(nfaTransitions, alphabet, nfaFinalStates);
    map<pair<int, char>, int> dfaTransitions = dfa.getDFA();
    map<int, int> dfaFinalStates = dfa.getDFAFinalStates();

    //------------------------------------ 4. Generate tokens ------------------------------------//
    string path = "./f1.txt";
    tokenGenerator t = tokenGenerator(priorityStrings, dfaTransitions, dfaFinalStates, path);

    //----------------------------------------- 5. LL(1) ------------------------------------------//
    string grammarPath = "Grammar Rules.txt";
    // use the prev path for grammer and return nrxt outpts pleas
    GrammarParser grammarParser(grammarPath);
    vector<std::string> terminals = grammarParser.getTerminals();
    vector<std::string> nonTerminals = grammarParser.getNonTerminals();
    map<std::string, std::vector<std::string>> CFG = grammarParser.getProductions();
    string start = grammarParser.getStartSymbol();

    //-------------------------------------- 6. First&Follow --------------------------------------//
    FirstFollow grammar;
    grammar.terminals = terminals;
    grammar.nonTerminals = nonTerminals;
    grammar.CFG = CFG;
    map<string, vector<string>> first = grammar.computeFirstSets();
    map<string, vector<string>> follow = grammar.computeFollowSets();
    map<string, vector<string>> comp = grammar.first_complementary;

    //-------------------------------------- 7. create table --------------------------------------//
    map<pair<string, string>, vector<string>> table;
    ParsingTable parsingTable(first, follow, comp);
    table = parsingTable.getTable();

    //------------------------------------- 8. stack handling -------------------------------------//
    ParserOutput po = ParserOutput(terminals, nonTerminals, start, t, table);
    vector<vector<string>> leftmostDerivation = po.getOut;

    //----------------------------------------- 9. Output -----------------------------------------//
    // 9.1 DFA table :
    ofstream outputFile1("DFA.txt");
    for (const auto &ele : dfa.getDFAStates())
    {
        for (const auto &ch : alphabet)
        {
            if (ch != '\0')
            {
                if (dfaTransitions.find(make_pair(ele.first, ch)) != dfaTransitions.end())
                    outputFile1 << "state  " << ele.first << " --(" << ch << ")-->: state " << dfaTransitions[make_pair(ele.first, ch)] << endl;
                else
                    outputFile1 << "state  " << ele.first << " --(" << ch << ")-->: Dead state" << endl;
            }
        }
        outputFile1 << "--------------------------------------------------------------------------------------------" << endl;
    }
    outputFile1.close();

    // 9.2 Tokens :
    set<string> ids = t.getIds();
    vector<string> tokens = t.getTokens();
    vector<string> values = t.getValues();
    vector<string> errors = t.getErrors();
    ofstream outputFile2("Tokens.txt");
    int to = 0, e = 0, v = 0;
    for (auto ct : t.getTypes())
    {
        if (ct == 0)
        {
            outputFile2 << "error" << endl;
        }
        else
        {
            outputFile2 << tokens[to] << endl;
            to++;
        }
    }
    outputFile2.close();
    // 9.3 Values :
    ofstream outputFile3("Values.txt");
    for (auto ct : t.getTypes())
    {
        if (ct == 0)
        {
            outputFile3 << errors[e] << endl;
            e++;
        }
        else
        {
            outputFile3 << values[v] << endl;
            v++;
        }
    }
    outputFile3.close();

    // 9.4 leftmostDerivation :
    ofstream outputFile4("leftmostDerivation.txt");
    for (auto s : leftmostDerivation)
    {
        for (auto w : s)
        {
            outputFile4 << w << " ";
        }
        outputFile4 << endl;
    }
    outputFile4.close();
}
