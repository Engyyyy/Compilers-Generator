#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <string>

using namespace std;

class NFA {
private:
    const char epsilon = '\0'; // Define epsilon as an empty character
    map<int, vector<pair<int, char>>> stateTransitions; // Map of state number to other states with input
    map<int, vector<int>> epsilonTransitions; // Epsilon transitions
    set<char> alphabet; // String alphabet
    unordered_map<int, int> statePriorities; // Map of state number to priority
    unordered_map<int, string> priorityStrings; // Map of priority number to the string
    set<char> punctuations; // Set of punctuations
    int numStates; // Total number of states in the NFA

public:
    NFA() : numStates(0) {} // Constructor initializing the number of states

    // Function to add transition for input symbol
    void addTransition(int from, int to, char input) {
        stateTransitions[from].push_back(make_pair(to, input)); // Add transition with input symbol
        alphabet.insert(input); // Add input symbol to the alphabet
        updateNumStates(from, to); // Update the total number of states
    }

    // Function to add epsilon transition
    void addEpsilonTransition(int from, int to) {
        epsilonTransitions[from].push_back(to); // Add epsilon transition
        updateNumStates(from, to); // Update the total number of states
    }

    // Set state priority
    void setStatePriority(int state, int priority) {
        statePriorities[state] = priority; // Set priority for a state
    }

    // Set priority string
    void setPriorityString(int priority, const string& str) {
        priorityStrings[priority] = str; // Set string for a priority
    }

    // Add punctuation
    void addPunctuation(char punctuation) {
        punctuations.insert(punctuation); // Add punctuation to the set
    }

    // Move function for current states and input
    set<int> move(const set<int>& currentStates, char input) {
        set<int> nextStates;
        for (int state : currentStates) {
            for (const auto& transition : stateTransitions[state]) {
                if (transition.second == input) {
                    nextStates.insert(transition.first); // Move to next states based on input
                }
            }
        }
        return nextStates; // Return the set of next states
    }

    // Epsilon closure function for given states
    set<int> epsilonClosure(const set<int>& states) {
        set<int> closure = states; // Initialize closure with given states
        vector<int> toCheck(states.begin(), states.end()); // States to check for epsilon transitions

        // Explore epsilon transitions
        for (size_t i = 0; i < toCheck.size(); ++i) {
            int state = toCheck[i];
            for (int nextState : epsilonTransitions[state]) {
                if (closure.find(nextState) == closure.end()) {
                    closure.insert(nextState); // Add new states to the closure
                    toCheck.push_back(nextState); // Add new states for further exploration
                }
            }
        }

        return closure; // Return the epsilon closure
    }

    // Kleene closure for given states
    set<int> kleeneClosure(const set<int>& states) {
        set<int> closure = epsilonClosure(states); // Epsilon closure of given states
        set<int> result = closure;

        // Compute Kleene closure iteratively until no new states are added
        while (true) {
            set<int> temp = result;
            for (int state : closure) {
                set<int> moves = move({ state }, epsilon); // Move from state via epsilon
                set<int> epsilonMoves = epsilonClosure(moves); // Get epsilon closure of moves
                result = setUnion(result, epsilonMoves); // Union of current result and epsilon moves
            }
            if (result == temp) {
                break;
            }
        }

        return result; // Return Kleene closure
    }

    // Positive closure for given states
    set<int> positiveClosure(const set<int>& states) {
        return kleeneClosure(states); // Positive closure using Kleene closure
    }

    // Concatenation operation
    void concatenate(NFA& other) {
        // Merge transitions from the 'other' NFA to this NFA
        for (const auto& transition : other.stateTransitions) {
            int from = transition.first;
            for (const auto& pair : transition.second) {
                int to = pair.first;
                char input = pair.second;
                addTransition(from, to, input); // Add transitions from 'other' NFA
            }
        }

        // Merge epsilon transitions from 'other' NFA to this NFA
        for (const auto& transition : other.epsilonTransitions) {
            int from = transition.first;
            for (int to : transition.second) {
                addEpsilonTransition(from, to); // Add epsilon transitions from 'other' NFA
            }
        }

        // Merge priorities and priority strings from 'other' NFA to this NFA
        statePriorities.insert(other.statePriorities.begin(), other.statePriorities.end());
        priorityStrings.insert(other.priorityStrings.begin(), other.priorityStrings.end());
        numStates += other.numStates - 1; // Update the total number of states
    }

    // Union (OR) operation
    void Or(NFA& other) {
        int newStartState = numStates;
        addEpsilonTransition(newStartState, 0); // Add epsilon transition to 'other' NFA's start state
        addEpsilonTransition(newStartState, other.numStates); // Add epsilon transition to this NFA's start state

        // Merge transitions from 'other' NFA to this NFA
        for (const auto& transition : other.stateTransitions) {
            int from = transition.first;
            for (const auto& pair : transition.second) {
                int to = pair.first;
                char input = pair.second;
                addTransition(from + newStartState, to + newStartState, input); // Add transitions from 'other' NFA
            }
        }

        // Merge epsilon transitions from 'other' NFA to this NFA
        for (const auto& transition : other.epsilonTransitions) {
            int from = transition.first;
            for (int to : transition.second) {
                addEpsilonTransition(from + newStartState, to + newStartState); // Add epsilon transitions from 'other' NFA
            }
        }

        // Merge priorities and priority strings from 'other' NFA to this NFA
        statePriorities.insert(other.statePriorities.begin(), other.statePriorities.end());
        priorityStrings.insert(other.priorityStrings.begin(), other.priorityStrings.end());
        numStates += other.numStates; // Update the total number of states
    }

    // Utility function for set union
    set<int> setUnion(const set<int>& set1, const set<int>& set2) {
        set<int> result = set1;
        result.insert(set2.begin(), set2.end());
        return result;
    }

    // Getters for accessing NFA components
    map<int, vector<pair<int, char>>> getStateTransitions() const {
        return stateTransitions;
    }

    set<char> getAlphabet() const {
        return alphabet;
    }

    unordered_map<int, int> getStatePriorities() const {
        return statePriorities;
    }

    unordered_map<int, string> getPriorityStrings() const {
        return priorityStrings;
    }

    set<char> getPunctuations() const {
        return punctuations;
    }

private:
    // Helper function to update the number of states
    void updateNumStates(int from, int to) {
        numStates = max(numStates, max(from, to) + 1);
    }
};

int main() {

    return 0;
}

