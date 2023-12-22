#include <utility>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <stack>
#include "../Lexical Analyzer/input handler.h"
#include "ParserOutput.h"

using namespace std;
using namespace Parser;

ParserOutput::ParserOutput(vector<string> term, vector<string> nonterm, string sta, LexicalAnalyzer::tokenGenerator to, const map<pair<string, string>, vector<string>> m)
    : terminals(term), nonTerminals(nonterm), start(sta), t(to), mp(m)
{
    st.push("$");
    st.push(start);
    auto it = find(terminals.begin(), terminals.end(), "$");
    if (!(it != terminals.end()))
    {
        terminals.push_back("$");
    }
}
void ParserOutput::getNextInput()
{
    inputs.push(t.getNext());
}
void ParserOutput::run()
{
    string st_f = st.top();
    string in_f = inputs.top();
    auto it = find(terminals.begin(), terminals.end(), st_f);

    if (it != terminals.end())
    {
        if (st_f == in_f && in_f == "$")
            return;
        else if (st_f == in_f)
        {
            // match
            st.pop();
            inputs.pop();
            getNextInput();
        }
        else
        {
            // report error
            inputs.push(st_f);
        }
    }
    else
    {
        std::pair<std::string, std::string> searchKey(st_f, in_f);
        if (mp.find(searchKey) != mp.end())
        {
            vector<string> v = mp[searchKey];
            if (v[0] == "sync")
            {
                // report error
                st.pop();
            }
            else if (v[0] == "")
            {
                st.pop();
            }
            else
            {
                st.pop();
                for (int j = v.size() - 1; j >= 0; j--)
                    st.push(v[j]);
            }
        }
        else
        {
            // report error
            inputs.pop();
            getNextInput();
        }
    }
    run();
}
