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
    vector<std::pair<std::pair<int, char>, int>> vec(dfaTransitions.begin(), dfaTransitions.end());
    auto customComparator = [](const auto& lhs, const auto& rhs) {
        return (lhs.first.first < rhs.first.first) ||
               (!(rhs.first.first < lhs.first.first) && lhs.first.second < rhs.first.second);
    };
    sort(vec.begin(), vec.end(), customComparator);
    int curr = 0;
    int statesNum = 1 ;
    ofstream outputFile1("DFA.txt");
    for (const auto& ele : vec) {
        if(ele.first.first!=curr){
            outputFile1 <<"-------------------------------------------------------------------------------------------------"<< endl;
            curr= ele.first.first;
            statesNum++;
        }
        outputFile1 << "state  " << ele.first.first << " --(" << ele.first.second << ")-->: state " << ele.second << endl;
    }
    outputFile1 <<"\n\n\n\n\n\n"<< endl;
    outputFile1 << "Number of states : "<< statesNum<< " state" << endl;

    outputFile1.close();

    //5.2 Tokens :
    ofstream outputFile2("Tokens.txt");
    for (auto s: tokens) {
        outputFile2 << s << endl;
    }
    outputFile2.close();

    //5.3 Values :
    ofstream outputFile3("Values.txt");
    for (auto s: values) {
        outputFile3 << s << endl;
    }
    outputFile3.close();

    //5.4 errors :
    ofstream outputFile4("Errors.txt");
    for (auto s: errors) {
        outputFile4 << s << endl;
    }
    outputFile4.close();


}
