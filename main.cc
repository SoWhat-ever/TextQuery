#include <iostream>
#include <fstream>
#include "Query.h"

using namespace std;

const string WORD_TO_QUERY = "git"; 

Query::QueryResult runQueries(ifstream &infile) {
    Query::TextQuery tq(infile);
    return tq.query(WORD_TO_QUERY);
};

int main(int argc, char** argv) {
    ifstream in("./test/input.txt");
    if(!in) {
        cerr << "Can't open ./test/input.txt" << endl;
    }
    Query::QueryResult qs = runQueries(in);
    in.close();
    ofstream out("./test/output.txt");
    if(!out) {
        cerr << "Can't open ./test/output.txt" << endl;
    }
    qs.print(out);
    out.close();
    return 0;
}