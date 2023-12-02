#include <iostream>
#include <utility>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <fstream>
#include <sstream>


using namespace std;

class DFA{
private:
    map<pair<int,char>,set<int>> NFA ; // state number --> next states
    set<char> alphabet ;
    map<int ,int > nfaFinalStates; // state number --> priority
    map<int,string> priority; // priority number --> string (0 maps to keywords)

    map<int,set<int>> dfaStates;
    map<pair<int,char>,int> Dfa ;
    map<int ,int > dfaFinalStates;
    set<string> ids ;
    set<int> epsilonClosure(int state) {
        set<int> closures = {state};
        queue<int> q;
        q.push(state);

        // Perform epsilon closure using BFS
        while (!q.empty()) {
            int currentState = q.front();
            q.pop();
            // Iterate over states reachable through epsilon transitions
            for (int nextState : NFA[make_pair(currentState, '\0')]) {
                // If nextState is not already in closures, add it and enqueue for further exploration
                if (closures.find(nextState) == closures.end()) {
                    closures.insert(nextState);
                    q.push(nextState);
                }
            }
        }
        return closures;
    }

    set<int> getNextStates(set<int> states, char symbol) {
        set<int> nextStates;
        // Iterate over input states , and Find transitions for each state with the given symbol
        for (int state : states) {
            auto it = NFA.find(make_pair(state, symbol));
            if (it != NFA.end()) {
                nextStates.insert(it->second.begin(), it->second.end());
            }
        }
        return nextStates;
    }

    void subsetConstructionAlgorithm() {
        pair<set<int>, int> initialState = make_pair(epsilonClosure(0), 0);
        dfaStates[0] = initialState.first;
        queue<pair<set<int>, int>> unprocessedStates;
        unprocessedStates.push(initialState);

        // Perform subset construction algorithm
        while (!unprocessedStates.empty()) {
            pair<set<int>, int> currentState = unprocessedStates.front();
            unprocessedStates.pop();

            // Iterate over alphabet symbols
            for (char symbol : alphabet) {
                // Get the set of next states for the current symbol
                set<int> nextState = getNextStates(currentState.first, symbol);
                // Compute the epsilon closure of the next states
                set<int> closure;
                for (int state : nextState) {
                    set<int> ep = epsilonClosure(state);
                    closure.insert(ep.begin(), ep.end());
                }

                // If the closure set is empty, skip to the next symbol
                if (closure.empty()) continue;

                // Check if the closure is a new state in the DFA
                bool isNewState = true;

                // Iterate over existing DFA states
                for (int i = 0; i < dfaStates.size(); ++i) {
                    // If the closure matches an existing state, update transition and break
                    if (closure == dfaStates[i]) {
                        isNewState = false;
                        Dfa[make_pair(currentState.second, symbol)] = i;
                        break;
                    }
                }

                // If the closure is a new state, add it to the DFA and enqueue for further exploration
                if (isNewState) {
                    dfaStates[dfaStates.size()] = closure;
                    unprocessedStates.push(make_pair(closure, dfaStates.size() - 1));
                    Dfa[make_pair(currentState.second, symbol)] = dfaStates.size() - 1;

                    // Update final states in DFA based on NFA final states
                    for (int s : closure) {
                        if (nfaFinalStates.find(s) != nfaFinalStates.end()) {
                            if (dfaFinalStates.find(dfaStates.size() - 1) == dfaFinalStates.end())
                                dfaFinalStates[dfaStates.size() - 1] = nfaFinalStates[s];
                            dfaFinalStates[dfaStates.size() - 1] = min(dfaFinalStates[dfaStates.size() - 1], nfaFinalStates[s]);
                        }
                    }
                }
            }
        }
    }


    // Function to perform partition refinement
    void dfaMinimization() {
        int mx=0;
        for(auto& x:dfaFinalStates){
            mx=max(mx,x.second);
        }
        // Step 1: Initialize PInew with the initial partition PI
        vector<set<int>> PInew(mx+2); // Initial partition with final and non-final states
        for(auto& x:dfaFinalStates){
            PInew[x.second].insert(x.first);
        }
        set<int> nonFinalStates;
        for (const auto& state : dfaStates) {
            if (dfaFinalStates.find(state.first) == dfaFinalStates.end()) {
                nonFinalStates.insert(state.first);
            }
        }
        PInew[mx+1] = nonFinalStates;

        while (true) {
            // Step 2: Create a new partition PInew
            vector<set<int>> newPartition;

            // For each group G in PInew
            for (auto& group : PInew) {
                map<set<int>, set<int>> subgroups;

                // Partition G into subgroups based on transitions
                for (int state : group) {
                    set<int> transitions;
                    for (const auto& symbol : Dfa) {
                        if (symbol.first.first == state) {
                            transitions.insert(Dfa[symbol.first]);
                        }
                    }
                    subgroups[transitions].insert(state);
                }

                // Add subgroups to the new partition
                for (const auto& subgroup : subgroups) {
                    newPartition.push_back(subgroup.second);
                }
            }

            // Check if PInew is the same as the previous partition
            if (newPartition == PInew) {
                break; // Convergence reached
            } else {
                PInew = newPartition;
            }
        }

        // PInew now contains the refined partition
        bool final = false;
        for(auto& group : PInew){
            if (group.size()>1){
                set<int>states;
                int groupName = *group.begin();
                states.insert(dfaStates[groupName].begin(),dfaStates[groupName].end());
                final = dfaFinalStates.find(groupName) != dfaFinalStates.end();
                for (int s : group) {
                    if(s != groupName) {
                        for (auto& symbol : alphabet) {
                            Dfa.erase(make_pair(s, symbol));
                        }

                        for (auto& transition : Dfa) {
                            if(transition.second == s)
                                transition.second = groupName;
                        }

                        states.insert(dfaStates[s].begin(),dfaStates[s].end());
                        dfaStates.erase(s);

                        if (final){
                            dfaFinalStates.erase(s);
                        }
                    }
                }
                dfaStates[groupName] = states;
            }
        }
    }
public:
    DFA(const map<pair<int, char>,set<int>>& nfa,
        const set<char>& alph,
        const map<int ,int > nfaFinal
        ) :
            NFA(nfa),alphabet(alph),nfaFinalStates(nfaFinal){
            subsetConstructionAlgorithm();
            dfaMinimization();

    }


    map<int, set<int>> getDFAStates() const {
        return dfaStates;
    }

    // Getter for DFA
    map<pair<int, char>, int> getDFA() const {
        return Dfa;
    }

    // Getter for dfaFinalStates
    map<int, int> getDFAFinalStates() const {
        return dfaFinalStates;
    }
};


//int main() {
//    map<pair<int,char>,set<int>> NFA ; // state number --> next states
//    string alphabet = "ab";
//    map<int ,int > nfaFinalStates; // state number --> priority
//    map<int,string> priority; // priority number --> string (0 maps to keywords)
//    NFA[make_pair(0,' ')]={1,7};
//    NFA[make_pair(1,' ')]={2,4};
//    NFA[make_pair(2,'a')]={3};
//    NFA[make_pair(3,' ')]={6};
//    NFA[make_pair(4,'b')]={5};
//    NFA[make_pair(5,' ')]={6};
//    NFA[make_pair(6,' ')]={1,7};
//    NFA[make_pair(7,'a')]={8};
//    NFA[make_pair(8,'b')]={9};
//    NFA[make_pair(9,'b')]={10};
//    nfaFinalStates[10]=0;
//    DFA d = DFA(NFA,alphabet,nfaFinalStates);
//    map<int,set<int>> dfaStates=d.getDFAStates();
//    cout << "DFA transitions :"<< endl;
//    for (int i = 0 ; i<dfaStates.size();i++) {
//        for (char symbol : alphabet) {
//            if(d.getDFA().find(make_pair(i,symbol))!=d.getDFA().end()){
//                map<pair<int,char>,int> Dfa =  d.getDFA();
//                cout << "state "<< i << " --"<< symbol << "--> : "<< Dfa[make_pair(i,symbol)]<<endl;
//            }
//        }
//    }
//
//    cout << "DFA final states :"<< endl;
//    for (pair<const int, int> s:d.getDFAFinalStates()) {
//        cout << s.first <<" ";
//    }
//    cout << "DFA states :"<< endl;
//
//    for(int i = 0 ; i< dfaStates.size() ; i++){
//        cout << endl;
//        cout << "state : "<< i << " : ";
//        for (int s : dfaStates[i]) {
//            cout << s << " ";
//        }
//    }
//}