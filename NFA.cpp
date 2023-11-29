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

    // Function to generate a new state ID
    int generateNewState() {
        int nextStateId = 0; // Variable to keep track of the next state ID
        return nextStateId++; // Return the current state ID and increment for the next state
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



// Function to apply positive closure operation
    void positiveClosure() {
        set<int> finalStates;

        // Find all final states and store them in a set
        for (const auto& priority : statePriorities) {
            if (priority.second == 1) {
                finalStates.insert(priority.first);
            }
        }

        // Add epsilon transitions from final states to the initial state (state 0)
        for (int finalState : finalStates) {
            addEpsilonTransition(finalState, 0);
        }
    }

    // Function to apply Kleene closure operation
    void kleeneClosure() {
        set<int> finalStates;

        // Find all final states and store them in a set
        for (const auto& priority : statePriorities) {
            if (priority.second == 1) {
                finalStates.insert(priority.first);
            }
        }

        // Add epsilon transitions from final states to the initial state (state 0)
        // Add epsilon transitions from the initial state to final states
        for (int finalState : finalStates) {
            addEpsilonTransition(finalState, 0);
            addEpsilonTransition(0, finalState);
        }
    }

    void concatenate(NFA& other) {
        int offset = numStates - 2; // Store the current total number of states excluding the duplicated state

        // Merge transitions from the 'other' NFA to this NFA
        for (const auto& transition : other.stateTransitions) {
            int from = transition.first + offset; // Adjust the 'from' state by the offset
            for (const auto& pair : transition.second) {
                int to = pair.first + offset; // Adjust the 'to' state by the offset
                char input = pair.second;
                addTransition(from, to, input); // Add transitions from 'other' NFA
            }
        }

        // Merge epsilon transitions from 'other' NFA to this NFA
        for (const auto& transition : other.epsilonTransitions) {
            int from = transition.first + offset; // Adjust the 'from' state by the offset
            for (int to : transition.second) {
                addEpsilonTransition(from, to + offset); // Adjust the 'to' state by the offset
            }
        }


        // Merge priorities and priority strings from 'other' NFA to this NFA
        for (const auto& priority : other.statePriorities) {
            statePriorities[priority.first + offset] = priority.second; // Adjust the state priority by the offset
        }

        for (const auto& priorityString : other.priorityStrings) {
            priorityStrings[priorityString.first] = priorityString.second; // Merge priority strings
        }

        numStates += other.numStates - (other.numStates > 0 ? 1 : 0); // Update the total number of states, excluding the duplicated state
    }

    static NFA Or(const NFA& nfa1, const NFA& nfa2) {
        NFA result;

        // Add a new start state
        int newStartState = 0;
        result.addEpsilonTransition(newStartState, 1); // Connect to the start state of the first NFA
        result.addEpsilonTransition(newStartState, nfa1.stateTransitions.size() + 2); // Connect to the start state of the second NFA

        // Merge transitions from the first NFA
        for (const auto& transition : nfa1.stateTransitions) {
            result.stateTransitions[transition.first] = transition.second;
        }

        // Merge transitions from the second NFA
        int offset = nfa1.stateTransitions.size() + 1;
        for (const auto& transition : nfa2.stateTransitions) {
            vector<pair<int, char>> adjustedTransitions;
            for (const auto& trans : transition.second) {
                adjustedTransitions.emplace_back(trans.first + offset, trans.second);
            }
            result.stateTransitions[transition.first + offset] = adjustedTransitions;
        }

        // Merge epsilon transitions from the first NFA
        for (const auto& epsilonTransition : nfa1.epsilonTransitions) {
            for (const auto& toState : epsilonTransition.second) {
                result.addEpsilonTransition(epsilonTransition.first + 1, toState + 1);
            }
        }

        // Merge epsilon transitions from the second NFA
        for (const auto& epsilonTransition : nfa2.epsilonTransitions) {
            for (const auto& toState : epsilonTransition.second) {
                result.addEpsilonTransition(epsilonTransition.first + offset, toState + offset);
            }
        }

        // Create a new final state and connect it to the final states of both NFAs via epsilon transitions
        int newFinalState = nfa1.stateTransitions.size() + nfa2.stateTransitions.size() + 3;
        result.addEpsilonTransition(nfa1.stateTransitions.size() + 1, newFinalState); // Connect to the final state of the first NFA
        result.addEpsilonTransition(nfa1.stateTransitions.size() + offset + 1, newFinalState); // Connect to the final state of the second NFA

        return result;
    }


    // Getters for accessing NFA components
    map<int, vector<pair<int, char>>> getStateTransitions() const {
        return stateTransitions;
    }
    map<int, vector<int>> getEpsilonTransitions() const {
        return epsilonTransitions;
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
    // Create NFAs and add transitions
    NFA nfa1, nfa2;

    nfa1.addTransition(1, 2, 'a');
    nfa1.addTransition(2, 3, 'b');
    nfa1.addTransition(3, 4, 'c');
    nfa1.addTransition(4, 5, 'd');
    nfa1.setStatePriority(5, 1);

    nfa2.addTransition(1, 2, 'e');
    nfa2.addTransition(2, 3, 'f');
    nfa2.addTransition(3, 4, 'g');
    nfa2.addTransition(4, 5, 'h');
    nfa2.setStatePriority(5, 2);

    //Test OR operation between NFAs
    NFA result = NFA::Or(nfa1, nfa2);


    // Retrieve and print transitions of the resulting NFA
    auto transitions = result.getStateTransitions();
    cout << "Transitions:" << endl;
    for (const auto& transition : transitions) {
        int fromState = transition.first;
        for (const auto& pair : transition.second) {
            int toState = pair.first;
            char input = pair.second;
            cout << "Transition: State " << fromState << " --(" << input << ")--> State " << toState << endl;
        }
    }

    cout << "*******************************************************************" << endl;

    // Retrieve and print epsilon transitions of the resulting NFA
    auto epsilonTransitions = result.getEpsilonTransitions();
    cout << "Epsilon Transitions:" << endl;
    for (const auto& epsilonTransition : epsilonTransitions) {
        int fromState = epsilonTransition.first;
        for (int toState : epsilonTransition.second) {
            cout << "Epsilon Transition: State " << fromState << " --(epsilon)--> State " << toState << endl;
        }
    }

    return 0;
}

