#ifndef NFA_H
#define NFA_H

#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <string>

using namespace std;

class NFA
{
private:
    const char epsilon = '\0';                          // Define epsilon as an empty character
    map<int, vector<pair<int, char>>> stateTransitions; // Map of state number to other states with input
    map<int, vector<int>> epsilonTransitions;           // Epsilon transitions
    set<char> alphabet;                                 // String alphabet
    unordered_map<int, int> statePriorities;            // Map of state number to priority
    unordered_map<int, string> priorityStrings;         // Map of priority number to the string
    set<char> punctuations;                             // Set of punctuations
    int numStates;                                      // Total number of states in the NFA
    void updateNumStates(int from, int to);

public:
    NFA();
    void addTransition(int from, int to, char input);
    void addEpsilonTransition(int from, int to);
    void setStatePriority(int state, int priority);
    void setPriorityString(int priority, const string &str);
    void addPunctuation(char punctuation);
    set<int> move(const set<int> &currentStates, char input);
    set<int> epsilonClosure(const set<int> &states);
    set<int> kleeneClosure(const set<int> &states);
    set<int> positiveClosure(const set<int> &states);
    void concatenate(NFA &other);
    void Or(NFA &other);
    set<int> setUnion(const set<int> &set1, const set<int> &set2);
    map<int, vector<pair<int, char>>> getStateTransitions() const;
    set<char> getAlphabet() const;
    unordered_map<int, int> getStatePriorities() const;
    unordered_map<int, string> getPriorityStrings() const;
    set<char> getPunctuations() const;
};

#endif