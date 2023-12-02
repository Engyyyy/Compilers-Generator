#ifndef TOKENGENERATOR_H
#define TOKENGENERATOR_H

#include <iostream>
#include <utility>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <fstream>
#include <sstream>
#include <iterator>
#include <vector>

class tokenGenerator {
private:
    std::map<int, std::string> priority;  // priority number --> string (0 maps to keywords)
    std::map<std::pair<int, char>, int> Dfa;
    std::map<int, int> dfaFinalStates;
    std::string fileName;
    std::vector<std::string> splitTokens;
    std::set<std::string> ids;
    std::vector<std::string> tokens;
    std::vector<std::string> values;
    std::vector<std::string> errors;

    void read();

    bool match(std::string str);

    void matchAllSplitTokens();

public:
    tokenGenerator(const std::map<int, std::string>& prio,
                   const std::map<std::pair<int, char>, int>& dfa,
                   const std::map<int, int>& finalStates,
                   std::string filePath);

    std::set<std::string> getIds() const;

    // Getter for tokens
    std::vector<std::string> getTokens() const;

    // Getter for values
    std::vector<std::string> getValues() const;

    // Getter for errors
    std::vector<std::string> getErrors() const;
};

#endif  // TOKENGENERATOR_H
