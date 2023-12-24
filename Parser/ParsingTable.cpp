#include <iostream>
#include <vector>
#include <map>
#include "ParsingTable.h"

using namespace std;

using namespace Parser;

ParsingTable::ParsingTable(map<string, vector<string>> first, map<string, vector<string>> follow, map<string, vector<string>> comp)
{
    valid = true;
    bool sync ;
    for (const auto& pair : first) {
        sync=true;
        for (int i=0;i<pair.second.size();i++) {
            if (pair.second[i] != "") {
                table[make_pair(pair.first, pair.second[i])].push_back(comp[pair.first][i]) ;
            } else {
                sync= false;
                for(const auto& s : follow[pair.first]) {
                    if (!table[make_pair(pair.first, s)].empty()) {
                        valid = false; // not LL1
                        table.clear();
                        return;
                    }
                    table[make_pair(pair.first, s)].push_back(comp[pair.first][i]);
                }
            }
        }
        if(sync){
            for(const auto& s : follow[pair.first]){
                if(!table[make_pair(pair.first, s)].empty()){
                    valid= false; // not LL1
                    table.clear();
                    return;
                }
                table[make_pair(pair.first, s)].push_back("sync") ;
            }
        }
    }

}
bool ParsingTable::isValid()
{
    return valid;
}

map<pair<string, string>, vector<string>> ParsingTable::getTable()
{
    return table;
}


////print table in a proper way
//for (const auto& pair : table) {
//  cout << "table(" << pair.first.first << "," << pair.first.second << "): { ";
//for (const string& s : pair.second) {
//  cout << s << " ";
//}
//cout << "}\n";
//}