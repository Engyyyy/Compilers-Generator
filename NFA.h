#ifndef NFA_H
#define NFA_H

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>

using namespace std;

class NFA {
private:
    map<int, vector<pair<int, char>>> stateTransitions;
    map<int, vector<int>> epsilonTransitions;
    set<char> alphabet;
    map<int, int> statePriorities;
    map<int, string> priorityStrings;
    int startState = 0, endState = 0;
    int priority = -1;

public:
    void setPriority(int p);
    void addSympol(char input);
    void addTransition(int from, int to, char input);
    void addEpsilonTransition(int from, int to);
    static NFA positiveClosure(NFA &nfa1);
    static NFA kleeneClosure(NFA &nfa1);
    static NFA concatenate(NFA &nfa1, NFA &nfa2);
    static NFA Or(NFA &nfa1, NFA &nfa2);
    map<int, int> getStatePriorities() const;
    map<int, vector<pair<int, char>>> getStateTransitions() const;
    map<int, vector<int>> getEpsilonTransitions() const;
    set<char> getAlphabet() const;
    const map<int, string> getPriorityStrings() const;
    static NFA combine(vector<NFA> vec);
    map<pair<int,char>,set<int>> getNfaTransitions() const;
};

#endif // NFA_H
