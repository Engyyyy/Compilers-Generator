#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>

using namespace std;

class NFA {
private:
    const char epsilon = '\0'; // Define epsilon as an empty character
    map<int, vector<pair<int, char>>> stateTransitions; // Map of state number to other states with input
    map<int, vector<int>> epsilonTransitions; // Epsilon transitions
    set<char> alphabet; // String alphabet
    map<int, int> statePriorities; // Map of state number to priority
    map<int, string> priorityStrings; // Map of priority number to the string
    int startState = 0 , endState = 0;


public:
    void addSympol(char input){
        stateTransitions[0].push_back(make_pair(1, input)); // Add transition with input symbol
        endState=1;
    }


    void addTransition(int from, int to, char input) {
        stateTransitions[from].push_back(make_pair(to, input)); // Add transition with input symbol
        alphabet.insert(input); // Add input symbol to the alphabet
    }

    // Function to add epsilon transition
    void addEpsilonTransition(int from, int to) {
        epsilonTransitions[from].push_back(to); // Add epsilon transition
    }

    void positiveClosure() {
        addEpsilonTransition(endState,startState);
    }

    // Function to apply Kleene closure operation
    void kleeneClosure() {
        addEpsilonTransition(startState,endState);
        addEpsilonTransition(endState,startState);
    }

    void concatenate(NFA& other) {
        map<int,int> m ;
        m[other.startState] = endState ;
        for(int i = other.startState+1 ; i<=other.endState ; i++) m[i] = m[i-1] + 1 ;

        // Merge transitions from the 'other' NFA to this NFA
        for (const auto& transition : other.stateTransitions) {
            int from = m[transition.first]; // Adjust the 'from' state by the offset
            for (const auto& pair : transition.second) {
                int to = m[pair.first]; // Adjust the 'to' state by the offset
                char input = pair.second;
                addTransition(from, to, input); // Add transitions from 'other' NFA
            }
        }

        // Merge epsilon transitions from 'other' NFA to this NFA
        for (const auto& transition : other.epsilonTransitions) {
            int from = m[transition.first]; // Adjust the 'from' state by the offset
            for (int to : transition.second) {
                addEpsilonTransition(from, m[to]); // Adjust the 'to' state by the offset
            }
        }
        endState = m[other.endState];

    }

    static NFA Or( NFA& nfa1,  NFA& nfa2) {
        NFA result;
        map<int,int> m1 , m2 ;
        m1[nfa1.startState] = 1 ;
        for(int i = nfa1.startState+1 ; i<=nfa1.endState ; i++) m1[i] = m1[i-1] + 1 ;

        m2[nfa2.startState] = m1[nfa1.endState]+1;
        for(int i = nfa2.startState+1 ; i<=nfa2.endState ; i++) m2[i] = m2[i-1] + 1 ;
        // Add a new start state
        int newStartState = 0;
        result.addEpsilonTransition(newStartState, m1[nfa1.startState]); // Connect to the start state of the first NFA
        result.addEpsilonTransition(newStartState, m2[nfa2.startState]); // Connect to the start state of the second NFA

        for (const auto& transition : nfa1.stateTransitions) {
            int from = m1[transition.first]; // Adjust the 'from' state by the offset
            for (const auto& pair : transition.second) {
                int to = m1[pair.first]; // Adjust the 'to' state by the offset
                char input = pair.second;
                result.addTransition(from, to, input); // Add transitions from 'other' NFA
            }
        }

        // Merge epsilon transitions from 'other' NFA to this NFA
        for (const auto& transition : nfa1.epsilonTransitions) {
            int from = m1[transition.first]; // Adjust the 'from' state by the offset
            for (int to : transition.second) {
                result.addEpsilonTransition(from, m1[to]); // Adjust the 'to' state by the offset
            }
        }

        for (const auto& transition : nfa2.stateTransitions) {
            int from = m2[transition.first]; // Adjust the 'from' state by the offset
            for (const auto& pair : transition.second) {
                int to = m2[pair.first]; // Adjust the 'to' state by the offset
                char input = pair.second;
                result.addTransition(from, to, input); // Add transitions from 'other' NFA
            }
        }

        // Merge epsilon transitions from 'other' NFA to this NFA
        for (const auto& transition : nfa2.epsilonTransitions) {
            int from = m2[transition.first]; // Adjust the 'from' state by the offset
            for (int to : transition.second) {
                result.addEpsilonTransition(from, m2[to]); // Adjust the 'to' state by the offset
            }
        }
        result.startState=0;

        // Create a new final state and connect it to the final states of both NFAs via epsilon transitions
        result.endState = m2[nfa2.endState]+1;

        result.addEpsilonTransition(m1[nfa1.endState], result.endState); // Connect to the final state of the first NFA
        result.addEpsilonTransition(m2[nfa2.endState], result.endState); // Connect to the final state of the second NFA

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

    const map<int, int> getStatePriorities() const {
        return statePriorities;
    }

    const map<int, string> getPriorityStrings() const {
        return priorityStrings;
    }


};


int main() {

    NFA nfa1, nfa2 ,nfa3, nfa4 ,nfa5, nfa6 , nfa7, nfa8 , nfa9 , nfa10 ;

    nfa1.addSympol('a');
    nfa2.addSympol('b');
    nfa3.addSympol('c');
    nfa4.addSympol('d');
    nfa5.addSympol('e');
    nfa6.addSympol('f');
    nfa7.addSympol('g');
    nfa8.addSympol('h');
    nfa9.addSympol('z');
    nfa10.addSympol('y');
    nfa1.concatenate(nfa2);
    nfa1.concatenate(nfa3);
    nfa1.concatenate(nfa4);
    nfa5.concatenate(nfa6);
    nfa5.concatenate(nfa7);
    nfa5.concatenate(nfa8);
    nfa9.concatenate(nfa10);
    NFA res2 = NFA::Or(nfa1,nfa5);
    NFA res = NFA::Or(res2,nfa9);

    // Retrieve and print transitions of the resulting NFA
    auto transitions = res.getStateTransitions();
    cout << "Transitions:" << endl;
    for (const auto& transition : transitions) {
        int fromState = transition.first;
        for (const auto& pair : transition.second) {
            int toState = pair.first;
            char input = pair.second;
            cout << "Transition: State " << fromState << " --(" << input << ")--> State " << toState << endl;
        }
    }

    cout << "***************************************************************" << endl;

    // Retrieve and print epsilon transitions of the resulting NFA
    auto epsilonTransitions = res.getEpsilonTransitions();
    cout << "Epsilon Transitions:" << endl;
    for (const auto& epsilonTransition : epsilonTransitions) {
        int fromState = epsilonTransition.first;
        for (int toState : epsilonTransition.second) {
            cout << "Epsilon Transition: State " << fromState << " --(epsilon)--> State " << toState << endl;
        }
    }
    return 0;
}