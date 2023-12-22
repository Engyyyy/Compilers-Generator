#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include "NFA.h"
#include "DFA.h"
#include "input handler.h"
#include "input_parser.h"


using namespace std;

int main() {

    //------------------------------------- 1. input parsing -------------------------------------//

    InputParser parser("C:\\Users\\Lenovo\\Desktop\\f2.txt");
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

    string path = "C:\\Users\\Lenovo\\Desktop\\file1.txt";
    tokenGenerator t = tokenGenerator(priorityStrings,dfaTransitions,dfaFinalStates,path);
    set<string> ids = t.getIds() ;
    vector<string> tokens = t.getTokens();
    vector<string> values = t.getValues();
    vector<string> errors = t.getErrors();

    //----------------------------------------- 5. Output -----------------------------------------//
    //5.1 DFA table :
    ofstream outputFile1("DFA.txt");
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

    //5.2 Tokens :
    ofstream outputFile2("Tokens.txt");
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

    //5.3 Values :
    ofstream outputFile3("Values.txt");
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

}