#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include "NFA.h"
#include "DFA.h"
#include "input handler.h"
#include "input_parser.h"


using namespace std;

int main() {
    InputParser parser("input.txt");
    parser.parse();
    NFA nfa = parser.getCombinedNFA();

    //---------------------------------------------------------------------------//

    map<pair<int, char>, set<int>> nfaTransitions = nfa.getNfaTransitions();
    set<char> alphabet = nfa.getAlphabet();
    map<int, int> nfaFinalStates = nfa.getStatePriorities();
    map<int, string> priorityStrings = nfa.getPriorityStrings();

    //---------------------------------------------------------------------------//
    DFA dfa = DFA(nfaTransitions,alphabet,nfaFinalStates);
    map<pair<int, char>, int> dfaTransitions = dfa.getDFA();
    map<int, int> dfaFinalStates = dfa.getDFAFinalStates();
    map<int, set<int>> dfaStates = dfa.getDFAStates();


    //---------------------------------------------------------------------------//
    string path = "C:\\Users\\Lenovo\\Desktop\\file1.txt";
    tokenGenerator t = tokenGenerator(priorityStrings,dfaTransitions,dfaFinalStates,path);
    set<string> ids = t.getIds() ;
    vector<string> tokens = t.getTokens();
    vector<string> values = t.getValues();
    vector<string> errors = t.getErrors();
    //---------------------------------------------------------------------------//


}