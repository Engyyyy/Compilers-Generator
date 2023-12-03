#include <iostream>
#include <utility>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <fstream>
#include <sstream>
#include <iterator>


#include "input handler.h"
using namespace std;




void tokenGenerator:: read(){
    string s, line;
    ifstream file(fileName);
    while ( getline (file,line) ){
        s +=' '+line;
    }
    file.close();
    istringstream b(s);
    istream_iterator<std::string> beg(b), end;
    vector<std::string> splitTok(beg, end);
    splitTokens = tokens;

}
bool tokenGenerator:: match(string str) {
    int last = -1, lastPos = 0, error_idx = -1, curr = 0;
    if (dfaFinalStates.find(curr) != dfaFinalStates.end()) {
        last = curr;
        lastPos = 0;
    }
    // get last accepted state and error is found.
    for (int i = 0; i < str.size(); i++) {
        if (Dfa.find(make_pair(curr, str[i])) == Dfa.end()) {
            error_idx = i;
            break;
        }
        curr = Dfa[make_pair(curr, str[i])];
        if (dfaFinalStates.find(curr) != dfaFinalStates.end()) {
            last = curr;
            lastPos = i;
        }
    }

    // case 1 : str is a one token
    if (curr == last) {
        string type = priority[dfaFinalStates[curr]];
        tokens.push_back(type);
        values.push_back(str);
        if (type == "id") {
            ids.insert(str);
        }
        return true;
    }

    // case 2 : str is more than one token or error
    else {
        if(lastPos!=-1){
            string type = priority[dfaFinalStates[last]];
            tokens.push_back(type);
            values.push_back(str.substr(0,lastPos+1));
            if (type == "id") {
                ids.insert(str);
            }
        }
        // case 2.1 : error
        if ((lastPos!=-1 && error_idx!=lastPos+1) || lastPos == -1 ){
            string reminder = str.substr(lastPos + 1);
            errors.push_back(reminder);
            return false;
        }
        // case 2.2 str is more than one token
        else return match(str.substr(lastPos + 1));
    }
}

void tokenGenerator:: matchAllSplitTokens(){
    for(string s : splitTokens) match(s);
}


tokenGenerator::tokenGenerator(const std::map<int, std::string>& prio,
               const std::map<std::pair<int, char>, int>& dfa,
               const std::map<int, int>& finalStates,
               string  filePath)
        : priority(prio), Dfa(dfa), dfaFinalStates(finalStates), fileName(std::move(filePath)) {
    read();
    matchAllSplitTokens();
}
set<string> tokenGenerator:: getIds() const {
    return ids;
}

// Getter for tokens
vector<string> tokenGenerator:: getTokens() const {
    return tokens;
}

// Getter for values
vector<string> tokenGenerator:: getValues() const {
    return values;
}

// Getter for errors
vector<string> tokenGenerator:: getErrors() const {
    return errors;
}

