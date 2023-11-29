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
        int offset = numStates - 1; // Store the current total number of states excluding the duplicated state

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


    void Or(NFA& other) {
        int offset = numStates; // Store the current total number of states for the first NFA
        int newFinalState = numStates + other.numStates; // New final state for both NFAs
        numStates = newFinalState ; // Update total number of states

        // Connect the final states of both NFAs to the new final state via epsilon transitions
        addEpsilonTransition(offset - 1, newFinalState); // Connect first NFA's final state to new final state
        addEpsilonTransition(other.numStates + offset - 1, newFinalState); // Connect second NFA's final state to new final state

        // Merge transitions from 'other' NFA to this NFA
        for (const auto& transition : other.stateTransitions) {
            int from = transition.first + offset;
            for (const auto& pair : transition.second) {
                int to = pair.first + offset;
                char input = pair.second;
                addTransition(from, to, input); // Add transitions from 'other' NFA
            }
        }

        // Merge epsilon transitions from 'other' NFA to this NFA
        for (const auto& transition : other.epsilonTransitions) {
            int from = transition.first + offset;
            for (int to : transition.second) {
                addEpsilonTransition(from, to + offset); // Adjust the 'to' state by the offset
            }
        }
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
    NFA nfa;

    // Define transitions
    nfa.addTransition(0, 1, 'a');
    nfa.addTransition(1, 2, 'b');
    nfa.addTransition(2, 3, 'c');
    // ... add more transitions as needed

    // Set priorities for states
    nfa.setStatePriority(3, 1); // Mark state 3 as a final state with priority 1

    // Apply positive closure
    nfa.kleeneClosure();

    // Retrieve and display the resulting transitions after positive closure
    cout << "Transitions after kleene closure:\n";
    map<int, vector<pair<int, char>>> transitions = nfa.getStateTransitions();
    map<int, vector<int>> epsilonTransitions = nfa.getEpsilonTransitions();

    for (const auto& transition : transitions) {
        int from = transition.first;
        for (const auto& pair : transition.second) {
            int to = pair.first;
            char input = pair.second;
            cout << "state " << from << " --" << input << "--> : " << to << "\n";
        }
    }

    // Display epsilon transitions
    cout << "\nEpsilon transitions after positive closure:\n";
    for (const auto& epsilon : epsilonTransitions) {
        int from = epsilon.first;
        for (int to : epsilon.second) {
            cout << "state " << from << "-- epsilon --> " << ": " << to << "\n";
        }
    }

    cout << "--------------------------------------------------------";
    // Create another NFA
    NFA nfa2;

    // Define transitions for the second NFA
    nfa2.addTransition(0, 1, 'd');
    nfa2.addTransition(1, 2, 'e');
    nfa2.addTransition(2, 3, 'f');
    // ... add more transitions as needed

    // Concatenate the first NFA with the second one
    nfa.Or(nfa2);

    // Display the resulting transitions after concatenation
    cout << "\nTransitions after union:\n";
    transitions = nfa.getStateTransitions();
    epsilonTransitions = nfa.getEpsilonTransitions();

    for (const auto& transition : transitions) {
        int from = transition.first;
        for (const auto& pair : transition.second) {
            int to = pair.first;
            char input = pair.second;
            cout << "state " << from << " --" << input << "--> : " << to << "\n";
        }
    }

    // Display epsilon transitions after concatenation
    cout << "\nEpsilon transitions after union:\n";
    for (const auto& epsilon : epsilonTransitions) {
        int from = epsilon.first;
        for (int to : epsilon.second) {
            cout << "state " << from << "-- epsilon --> " << ": " << to << "\n";
        }
    }

    return 0;
}


