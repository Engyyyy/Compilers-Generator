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
#include "Parser/FirstFollow.h"
#include "Parser/ParserOutput.h"
#include "Parser/ParsingTable.h"
#include "parser/GrammarParser.h"

using namespace std;
using namespace LexicalAnalyzer;
using namespace Parser;

int main() {

    //------------------------------------- 1. input parsing -------------------------------------//
    InputParser parser(".\\input files\\ruls.txt");
    parser.parse();


    //------------------------------------------ 2. NFA ------------------------------------------//
    NFA nfa = parser.getCombinedNFA();
    map<int, string> priorityStrings = parser.getPriorityStrings();
    map<pair<int, char>, set<int>> nfaTransitions = nfa.getNfaTransitions();
    set<char> alphabet = nfa.getAlphabet();
    map<int, int> nfaFinalStates = nfa.getStatePriorities();


    //------------------------------------------ 3. DFA ------------------------------------------//
    DFA dfa = DFA(nfaTransitions,alphabet,nfaFinalStates);
    map<pair<int, char>, int> dfaTransitions = dfa.getDFA();
    map<int, int> dfaFinalStates = dfa.getDFAFinalStates();


    //------------------------------------ 4. Generate tokens ------------------------------------//
    string path = ".\\input files\\test.txt";
    tokenGenerator t = tokenGenerator(priorityStrings,dfaTransitions,dfaFinalStates,path);


    //----------------------------------------- 5. LL(1) ------------------------------------------//
    string grammerPath = ".\\input files\\grammer.txt";
    GrammarParser grammarParser(grammerPath);
    vector<std::string> terminals = grammarParser.getTerminals();
    terminals.push_back("$");    
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
       // use first , follow , CFG , terminals and nonTerminals to outpt the next please 
    map<pair<string,string>,vector<string>> table ;
    ParsingTable parsingTable(first,follow,comp);
    table=parsingTable.getTable();
    parsingTable.csvTable(grammar.terminals,grammar.nonTerminals);
    parsingTable.printFirstFollow();

    //------------------------------------- 8. stack handling -------------------------------------//
    bool val = parsingTable.isValid();
    vector<string> leftmostDerivation;
    if(val){
        ParserOutput po = ParserOutput(grammar.terminals,grammar.nonTerminals,start,t,table);
        leftmostDerivation = po.getOut();
        t=po.getT();

    }

    //----------------------------------------- 9. Output -----------------------------------------//
        //9.1 DFA table :
        ofstream outputFile1(".\\output files\\DFA.txt");
        for (const auto& ele : dfa.getDFAStates()) {
            for (const auto& ch : alphabet) {
                if(ch!='\0'){
                    if(dfaTransitions.find(make_pair(ele.first,ch))!=dfaTransitions.end())
                        outputFile1 << "state  " << ele.first<< " --(" << ch << ")-->: state " << dfaTransitions[make_pair(ele.first,ch)] << endl;
                    else
                        outputFile1 << "state  " << ele.first<< " --(" << ch << ")-->: Dead state"  << endl;
                }

            }
            outputFile1 << "--------------------------------------------------------------------------------------------" << endl;

        }
        outputFile1.close();
        
        //9.2 Tokens :
        set<string> ids = t.getIds() ;
        vector<string> tokens = t.getTokens();
        vector<string> values = t.getValues();
        vector<string> errors = t.getErrors();
        ofstream outputFile2(".\\output files\\Tokens.txt");
        int to =0 , e =0 , v=0;
        for (auto ct: t.getTypes()) {
            if(ct==0) {
                outputFile2 << "error" << endl;
            } else{
                outputFile2 << tokens[to] << endl;
                to++;
             }
        }
        outputFile2.close();
        //9.3 Values :
        ofstream outputFile3(".\\output files\\Values.txt");
        for (auto ct: t.getTypes()) {
            if(ct==0) {
                outputFile3 << errors[e] << endl;
                e++;
            } else{
                outputFile3 << values[v] << endl;
                v++;
            }
        }
        outputFile3.close();    

        //9.4 leftmostDerivation :
        ofstream outputFile4(".\\output files\\leftmostDerivation.txt");
        if(val){
            for (auto s: leftmostDerivation) 
                outputFile4<<s<<"\n";
        }
        else{
            outputFile4<<"Invalid Grammer !"<<endl;
            cout<<"Invalid Grammer !"<<endl;
        }
        outputFile4.close();
}