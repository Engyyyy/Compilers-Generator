#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include "NFA.h"
#include "DFA.h"
#include "input handler.h"
#include "input_parser.h"


using namespace std;

int main() {
//    map<int, string> priorityStrings ;
//
//    NFA nfa1 , nfa2 ,nfa3, nfa4 ,nfa5, nfa6 , nfa7, nfa8 , nfa9 , nfa10 , nfa11 , nfa12 , nfa13 ,nfa14, nfa15 ,nfa16, nfa17 , nfa18, nfa19 , nfa20 , nfa21 , nfa22 , nfa23 ,nfa24, nfa25 ,nfa26 , nfa30 , nfa31 ,nfa32, nfa33,nfa34 ,nfa35, nfa36 , nfa37, nfa38 , nfa39 , nfa40 ;
//    nfa1.addSympol('q');
//    nfa2.addSympol('w');
//    nfa3.addSympol('e');
//    nfa4.addSympol('r');
//    nfa5.addSympol('t');
//    nfa6.addSympol('y');
//    nfa7.addSympol('u');
//    nfa8.addSympol('i');
//    nfa9.addSympol('o');
//    nfa10.addSympol('p');
//    nfa11.addSympol('a');
//    nfa12.addSympol('s');
//    nfa13.addSympol('d');
//    nfa14.addSympol('f');
//    nfa15.addSympol('g');
//    nfa16.addSympol('h');
//    nfa17.addSympol('j');
//    nfa18.addSympol('k');
//    nfa19.addSympol('l');
//    nfa20.addSympol('z');
//    nfa21.addSympol('x');
//    nfa22.addSympol('c');
//    nfa23.addSympol('v');
//    nfa24.addSympol('b');
//    nfa25.addSympol('n');
//    nfa26.addSympol('m');
//
//    nfa30.addSympol('1');
//    nfa31.addSympol('0');
//
//    nfa32.addSympol('+');
//    nfa33.addSympol('-');
//    nfa34.addSympol('*');
//    nfa35.addSympol('/');
//
//
//    nfa36.addSympol('=');
//    nfa37.addSympol('!');
//
//    NFA letter = NFA::Or(nfa1,nfa2);
//    letter = NFA::Or(letter,nfa3);
//    letter = NFA::Or(letter,nfa4);
//    letter = NFA::Or(letter,nfa5);
//    letter = NFA::Or(letter,nfa6);
//    letter = NFA::Or(letter,nfa7);
//    letter = NFA::Or(letter,nfa8);
//    letter = NFA::Or(letter,nfa9);
//    letter = NFA::Or(letter,nfa10);
//    letter = NFA::Or(letter,nfa11);
//    letter = NFA::Or(letter,nfa12);
//    letter = NFA::Or(letter,nfa13);
//    letter = NFA::Or(letter,nfa14);
//    letter = NFA::Or(letter,nfa15);
//    letter = NFA::Or(letter,nfa16);
//    letter = NFA::Or(letter,nfa17);
//    letter = NFA::Or(letter,nfa18);
//    letter = NFA::Or(letter,nfa19);
//    letter = NFA::Or(letter,nfa20);
//    letter = NFA::Or(letter,nfa21);
//    letter = NFA::Or(letter,nfa22);
//    letter = NFA::Or(letter,nfa23);
//    letter = NFA::Or(letter,nfa24);
//    letter = NFA::Or(letter,nfa25);
//    letter = NFA::Or(letter,nfa26);
//
//    NFA dig = NFA::Or(nfa30,nfa31);
//
//    NFA digs = NFA::kleeneClosure(dig);
//
//    NFA oo = NFA::Or(letter,dig);
//    NFA pc = NFA::positiveClosure(oo);
//
//    NFA id = NFA::concatenate(letter,pc);
//    id.setPriority(3);
//    priorityStrings[3]="id";
//    NFA num = NFA::positiveClosure(dig);
//    num.setPriority(4);
//    priorityStrings[4]="num";
//    NFA assign ;
//    assign.addSympol('=');
//    assign.setPriority(5);
//    priorityStrings[5]="assign";
//    NFA addop = NFA::Or(nfa32,nfa33);
//    addop.setPriority(6);
//    priorityStrings[6]="addop";
//    NFA mulop = NFA::Or(nfa34,nfa35);
//    mulop.setPriority(7);
//    priorityStrings[7]="mulop";
//    NFA relop = NFA::concatenate(nfa37,nfa36);
//    relop.setPriority(8);
//    priorityStrings[8]="relop";
//    NFA wh = NFA::concatenate(nfa2,nfa16);
//    wh = NFA::concatenate(wh,nfa8);
//    wh = NFA::concatenate(wh,nfa19);
//    wh = NFA::concatenate(wh,nfa3);
//    wh.setPriority(1);
//    priorityStrings[1]="while";
//    NFA in = NFA::concatenate(nfa8,nfa25);
//    in = NFA::concatenate(in,nfa5);
//    in.setPriority(2);
//    priorityStrings[2]="int";
//
//
//    NFA p1 , p2 , p3 , p4 , p5 , p6 ;
//    p1.addSympol(';');
//    p1.setPriority(9);
//    priorityStrings[9]=";";
//    p2.addSympol(',');
//    p2.setPriority(10);
//    priorityStrings[10]=",";
//    p3.addSympol('{');
//    p3.setPriority(11);
//    priorityStrings[11]="{";
//    p4.addSympol('}');
//    p4.setPriority(12);
//    priorityStrings[12]="}";
//    p5.addSympol('(');
//    p5.setPriority(13);
//    priorityStrings[13]="(";
//    p6.addSympol(')');
//    p6.setPriority(14);
//    priorityStrings[14]=")";
//    vector<NFA> v = {id , num , assign , addop , mulop , relop , wh , in , p1 , p2, p3 , p4, p5 , p6};
//
//
//    NFA nfa = NFA::combine(v);
    //------------------------------------- 1. input parsing -------------------------------------//

    InputParser parser("C:\\Users\\Lenovo\\Desktop\\f2.txt");
    parser.parse();

    //------------------------------------------ 2. NFA ------------------------------------------//

    NFA nfa = parser.getCombinedNFA();
    map<int, string> priorityStrings = parser.getPriorityStrings();
    map<pair<int, char>, set<int>> nfaTransitions = nfa.getNfaTransitions();
    set<char> alphabet = nfa.getAlphabet();
    map<int, int> nfaFinalStates = nfa.getStatePriorities();

    //------------------------------------------ 3. DFA ------------------------------------------//

    DFA dfa = DFA(nfaTransitions,alphabet,nfaFinalStates);
    map<pair<int, char>, int> dfaTransitions = dfa.getDFA();
    map<int, int> dfaFinalStates = dfa.getDFAFinalStates();


    //------------------------------------ 4. Generate tokens ------------------------------------//

    string path = "C:\\Users\\Lenovo\\Desktop\\file1.txt";
    tokenGenerator t = tokenGenerator(priorityStrings,dfaTransitions,dfaFinalStates,path);
    set<string> ids = t.getIds() ;
    vector<string> tokens = t.getTokens();
    vector<string> values = t.getValues();
    vector<string> errors = t.getErrors();

    //----------------------------------------- 5. Output -----------------------------------------//
    //5.1 DFA table :
    vector<std::pair<std::pair<int, char>, int>> vec(dfaTransitions.begin(), dfaTransitions.end());
    auto customComparator = [](const auto& lhs, const auto& rhs) {
        return (lhs.first.first < rhs.first.first) ||
               (!(rhs.first.first < lhs.first.first) && lhs.first.second < rhs.first.second);
    };
    sort(vec.begin(), vec.end(), customComparator);
    int curr = 0;
    ofstream outputFile1("DFA.txt");
    for (const auto& ele : vec) {
        if(ele.first.first!=curr){
            outputFile1 <<"-------------------------------------------------------------------------------------------------"<< endl;
            curr= ele.first.first;
        }
        outputFile1 << "state  " << ele.first.first << " --(" << ele.first.second << ")-->: state " << ele.second << endl;
    }
    outputFile1 <<"\n\n\n\n\n\n"<< endl;
    outputFile1 << "Number of states : "<< dfa.getDFAStates().size()<< " state" << endl;

    outputFile1.close();

    //5.2 Tokens :
    ofstream outputFile2("Tokens.txt");
    for (auto s: tokens) {
        outputFile2 << s << endl;
    }
    outputFile2.close();

    //5.3 Values :
    ofstream outputFile3("Values.txt");
    for (auto s: values) {
        outputFile3 << s << endl;
    }
    outputFile3.close();

    //5.4 errors :
    ofstream outputFile4("Errors.txt");
    for (auto s: errors) {
        outputFile4 << s << endl;
    }
    outputFile4.close();


}