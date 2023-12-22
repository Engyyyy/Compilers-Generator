#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include "FirstFollow.h"

using namespace std;
using namespace Parser;

// Check if the symbol is a terminal
bool FirstFollow ::isTerminal(string s)
{
    return find(terminals.begin(), terminals.end(), s) != terminals.end();
}

// Check if the symbol is a non-terminal
bool FirstFollow ::isNonTerminal(string s)
{
    return find(nonTerminals.begin(), nonTerminals.end(), s) != nonTerminals.end();
}

map<string, vector<string>> FirstFollow ::computeFirstSets()
{
    bool changes = true;

    // Initialize all FIRST sets as empty
    for (const string &nonTerminal : nonTerminals)
    {
        first[nonTerminal] = vector<string>();
    }

    while (changes)
    {
        changes = false;

        // For each production rule X -> α
        for (const auto &rule : CFG)
        {
            string nonTerminal = rule.first;
            for (const string &production : rule.second)
            {
                size_t i = 0;
                bool allDeriveEpsilon = true;

                // Iterate through symbols in the production
                while (i < production.size())
                {
                    string symbol = production.substr(i);
                    size_t spacePos = symbol.find(' ');

                    if (spacePos != string::npos)
                    {
                        symbol = symbol.substr(0, spacePos);
                        i += spacePos + 1;
                    }
                    else
                    {
                        i = production.size();
                    }

                    if (isTerminal(symbol))
                    {
                        // Rule 1: If X is a terminal symbol
                        if (find(first[nonTerminal].begin(), first[nonTerminal].end(), symbol) == first[nonTerminal].end())
                        {
                            first[nonTerminal].push_back(symbol);
                            changes = true;
                        }
                        allDeriveEpsilon = false;
                        break; // Stop processing further symbols in the production
                    }
                    else if (isNonTerminal(symbol))
                    {
                        // Rule 2: If X is a non-terminal symbol
                        bool epsilonFound = false;
                        for (const string &s : first[symbol])
                        {
                            // Rule 2.1: Check for terminal 'a' in FIRST(Yi)
                            if (s != Epsilon && find(first[nonTerminal].begin(), first[nonTerminal].end(), s) == first[nonTerminal].end())
                            {
                                first[nonTerminal].push_back(s);
                                changes = true;
                            }
                            // Rule 2.2: Check if epsilon is in all FIRST(Yj) for j=1,...,i-1
                            if (s == Epsilon)
                            {
                                epsilonFound = true;
                            }
                        }

                        if (!epsilonFound)
                        {
                            allDeriveEpsilon = false;
                            break;
                        }
                    }
                }

                // Rule 2.2 continued: Check if epsilon should be in FIRST(X)
                if (allDeriveEpsilon && find(first[nonTerminal].begin(), first[nonTerminal].end(), Epsilon) == first[nonTerminal].end())
                {
                    first[nonTerminal].emplace_back(Epsilon);
                    changes = true;
                }
            }
        }
    }
    return first;
}
map<string, vector<string>> FirstFollow ::computeFollowSets()
{
    bool changes = true;

    // Initialize all FOLLOW sets as empty
    for (const string &nonTerminal : nonTerminals)
    {
        follow[nonTerminal] = vector<string>();
    }

    // Add $ to FOLLOW of the start symbol
    follow[nonTerminals[0]].emplace_back("$");

    while (changes)
    {
        changes = false;

        // For each production rule A -> αBβ
        for (const auto &rule : CFG)
        {
            string nonTerminal = rule.first;
            for (const string &production : rule.second)
            {
                size_t i = 0;
                while (i < production.size())
                {
                    string symbol = production.substr(i);
                    size_t spacePos = symbol.find(' ');

                    if (spacePos != string::npos)
                    {
                        symbol = symbol.substr(0, spacePos);
                        i += spacePos + 1;
                    }
                    else
                    {
                        i = production.size();
                    }

                    if (isNonTerminal(symbol))
                    {
                        // Rule 1: Everything in FIRST(β) is in FOLLOW(B) except epsilon
                        size_t j = i;
                        bool allDeriveEpsilon = true;
                        while (j < production.size())
                        {
                            string nextSymbol = production.substr(j);
                            size_t nextSpacePos = nextSymbol.find(' ');

                            if (nextSpacePos != string::npos)
                            {
                                nextSymbol = nextSymbol.substr(0, nextSpacePos);
                                j += nextSpacePos + 1;
                            }
                            else
                            {
                                j = production.size();
                            }

                            if (isTerminal(nextSymbol))
                            {
                                if (find(follow[symbol].begin(), follow[symbol].end(), nextSymbol) == follow[symbol].end() && nextSymbol != Epsilon)
                                {
                                    follow[symbol].push_back(nextSymbol);
                                    changes = true;
                                }
                                allDeriveEpsilon = false;
                                break;
                            }
                            else if (isNonTerminal(nextSymbol))
                            {
                                bool epsilonFound = false;
                                for (const string &s : first[nextSymbol])
                                {
                                    if (s != Epsilon && find(follow[symbol].begin(), follow[symbol].end(), s) == follow[symbol].end())
                                    {
                                        follow[symbol].push_back(s);
                                        changes = true;
                                    }
                                    if (s == Epsilon)
                                    {
                                        epsilonFound = true;
                                    }
                                }

                                if (!epsilonFound)
                                {
                                    allDeriveEpsilon = false;
                                    break;
                                }
                            }
                        }

                        // Rule 2: Everything in FOLLOW(A) is in FOLLOW(B)
                        if (allDeriveEpsilon || i == production.size())
                        {
                            for (const string &s : follow[nonTerminal])
                            {
                                if (find(follow[symbol].begin(), follow[symbol].end(), s) == follow[symbol].end())
                                {
                                    follow[symbol].push_back(s);
                                    changes = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return follow;
}

// int main()
// {
//     // Create an instance of the LL1Grammar class
//     FirstFollow grammar;

//     cout << "Test Example one" << endl ;

//     // Define your grammar rules here
//     grammar.terminals = {"+", "*", ")", "(", "id"};
//     grammar.nonTerminals = {"E", "E'", "T", "T'", "F"};
//     grammar.CFG = {
//             {"E", {"T E'"}},
//             {"E'", {"+ T E'", ""}},
//             {"T", {"F T'"}},
//             {"T'", {"* F T'", ""}},
//             {"F", {"( E )", "id"}}
//     };
//     map<string, vector<string>> firstSets = grammar.computeFirstSets();

//     for (const auto& pair : firstSets) {
//         cout << "FIRST(" << pair.first << "): { ";
//         for (const string& s : pair.second) {
//             cout << s << " ";
//         }
//         cout << "}\n";
//     }
//     cout << "\n";

//     // Calculate FOLLOW sets
//     map<string, vector<string>> followSets = grammar.computeFollowSets();

//     for (const auto& pair : followSets) {
//         cout << "FOllOW(" << pair.first << "): { ";
//         for (const string& s : pair.second) {
//             cout << s << " ";
//         }
//         cout << "}\n";
//     }
//     cout << "\n";
//     cout << "-----------------------------------"<<endl;

//     // Define your grammar rules here

//     cout << "Test Example two" << endl ;
//     FirstFollow grammar_1;
//     grammar_1.terminals = {"i" ,"t" , "e" , "b" , "a" };
//     grammar_1.nonTerminals = {"E", "S" , "C"};
//     grammar_1.CFG = {
//             {"S", {"i C t S E" , "a"}},
//             {"E" , {"e S" , ""}},
//             {"C" , {"b"}}
//     };

//     grammar_1.computeFirstSets();

//     // Display the FIRST sets for each non-terminal
//     firstSets = grammar_1.computeFirstSets();
//     for (const auto& pair : firstSets) {
//         cout << "FIRST(" << pair.first << "): { ";
//         for (const string& s : pair.second) {
//             cout << s << " ";
//         }
//         cout << "}\n";
//     }
//     cout << "\n";

//     // Calculate FOLLOW sets
//     followSets = grammar_1.computeFollowSets();

//     for (const auto& pair : followSets) {
//         cout << "FOllOW(" << pair.first << "): { ";
//         for (const string& s : pair.second) {
//             cout << s << " ";
//         }
//         cout << "}\n";
//     }
//     cout << "\n";
//     cout << "-----------------------------------"<<endl;

//     cout << "Test Example three" << endl ;

//     FirstFollow grammar_2;
//     grammar_2.terminals = {"s", "u", "v", "t", "b"};
//     grammar_2.nonTerminals = {"S", "R", "U", "V", "T"};
//     grammar_2.CFG = {
//             {"S", {"R T"}},
//             {"R", {"s U R b", ""}},
//             {"U", {"u U" , ""}},
//             {"V", {"v V", ""}},
//             {"T", {"V t T", ""}}
//     };

//     // Display the FIRST sets for each non-terminal
//     firstSets = grammar_2.computeFirstSets();
//     for (const auto& pair : firstSets) {
//         cout << "FIRST(" << pair.first << "): { ";
//         for (const string& s : pair.second) {
//             cout << s << " ";
//         }
//         cout << "}\n";
//     }
//     cout << "\n";

//     // Calculate FOLLOW sets
//     followSets = grammar_2.computeFollowSets();

//     for (const auto& pair : followSets) {
//         cout << "FOllOW(" << pair.first << "): { ";
//         for (const string& s : pair.second) {
//             cout << s << " ";
//         }
//         cout << "}\n";
//     }
//     cout << "\n";

//     cout << "-----------------------------------"<<endl;
//     // Define your grammar rules here

//     cout << "Test Example four" << endl ;
//     FirstFollow grammar_3;
//     grammar_3.terminals = {"or" ,"not" , "and" , "(" , ")"  , "true" , "false"};
//     grammar_3.nonTerminals = {"bexpr", "bexpr'" , "bterm" , "bterm'" , "bfactor"};
//     grammar_3.CFG = {

//             {"bexpr", {"bterm bexpr'"}},
//             {"bexpr'", {"or bterm bexpr'" , ""}},
//             {"bterm", {"bfactor bterm'"}},
//             {"bterm'", {"and bfactor bterm'" , ""}},
//             {"bfactor", {"not bfactor" , "( bexpr )" , "true" , "false"}}

//     };

//     // Display the FIRST sets for each non-terminal
//     firstSets = grammar_3.computeFirstSets();
//     for (const auto& pair : firstSets) {
//         cout << "FIRST(" << pair.first << "): { ";
//         for (const string& s : pair.second) {
//             cout << s << " ";
//         }
//         cout << "}\n";
//     }
//     cout << "\n";

//     // Calculate FOLLOW sets
//     followSets = grammar_3.computeFollowSets();

//     // Display the FOLLOW sets for each non-terminal
//     for (const auto& pair : followSets) {
//         cout << "FOllOW(" << pair.first << "): { ";
//         for (const string& s : pair.second) {
//             cout << s << " ";
//         }
//         cout << "}\n";
//     }
//     cout << "\n";

//     cout << "-----------------------------------"<<endl;

//     // Define your grammar rules here

//     cout << "Test Example five" << endl ;
//     FirstFollow grammar_4;
//     grammar_4.terminals = {"+" , "*" , "(" , ")" , "a" , "b"};
//     grammar_4.nonTerminals = {"E" , "E'" , "T" , "T'" , "F" , "F'" , "P"};
//     grammar_4.CFG = {

//             {"E", {"T E'"}},
//             {"E'", {"+ E" , ""}},
//             {"T", {"F T'"}},
//             {"T'", {"T" , ""}},
//             {"F", {"P F'"}},
//             {"F'", {"* F" , ""}},
//             {"P", {"( E )" , "a" , "b" , "epsilon"}}

//     };

//     // Display the FIRST sets for each non-terminal
//     firstSets =grammar_4.computeFirstSets();
//     for (const auto& pair : firstSets) {
//         cout << "FIRST(" << pair.first << "): { ";
//         for (const string& s : pair.second) {
//             cout << s << " ";
//         }
//         cout << "}\n";
//     }
//     cout << "\n";

//     // Calculate FOLLOW sets
//     followSets = grammar_4.computeFollowSets();

//     // Display the FOLLOW sets for each non-terminal
//     for (const auto& pair : followSets) {
//         cout << "FOllOW(" << pair.first << "): { ";
//         for (const string& s : pair.second) {
//             cout << s << " ";
//         }
//         cout << "}\n";
//     }
//     cout << "\n";
//     cout << "-----------------------------------"<<endl;

//     // Define your grammar rules here

//     cout << "Test Example six" << endl ;

//     FirstFollow grammar_5;
//     grammar_5.terminals = {"b" ,"e" , "a" , "c" , "d" };
//     grammar_5.nonTerminals = {"S" , "A"};
//     grammar_5.CFG = {
//             {"S", {"A b S" , "e" , ""}},
//             {"A" , {"a" , "c A d"}}

//     };

//     // Display the FIRST sets for each non-terminal
//     firstSets = grammar_5.computeFirstSets();
//     for (const auto& pair : firstSets) {
//         cout << "FIRST(" << pair.first << "): { ";
//         for (const string& s : pair.second) {
//             cout << s << " ";
//         }
//         cout << "}\n";
//     }
//     cout << "\n";

//     // Calculate FOLLOW sets
//     followSets = grammar_5.computeFollowSets();

//     // Display the FOLLOW sets for each non-terminal
//     for (const auto& pair : followSets) {
//         cout << "FOllOW(" << pair.first << "): { ";
//         for (const string& s : pair.second) {
//             cout << s << " ";
//         }
//         cout << "}\n";
//     }
//     cout << "\n";

//     cout << "-----------------------------------"<<endl;
//     return 0;
// }