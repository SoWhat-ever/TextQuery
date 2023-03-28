#include <iostream>
#include <fstream>
#include "Query.h"

using namespace std;


Query::QueryResult runQueries(ifstream &infile) {
    Query::TextQuery tq(infile);
    // 不存在"you"，且存在"You"或者"git"
    Query::Query q = ~Query::Query("you") & (Query::Query("git") | Query::Query("You"));
    return q.eval(tq);
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