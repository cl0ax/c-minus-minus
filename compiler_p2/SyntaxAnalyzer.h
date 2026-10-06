//
// The code was created by Jacob, Jafet, Abraham.
//

#ifndef SYNTAXANALYZER_H
#define SYNTAXANALYZER_H

#include <fstream>
#include <map>
#include <vector>
using namespace std;

class SyntaxAnalyzer{
private:
    vector<string> lexemes;
    vector<string> tokens;
    vector<string>::iterator lexitr;
    vector<string>::iterator tokitr;
    // map of variables and their datatype
    // i.e. sum t_integer
    map<string, string> symboltable;

    // other private methods
    bool vdecassign(const string& type); // jacob
    bool stmtlist(); // abraham
    int stmt(); // abraham
    bool ifstmt(); // jacob
    bool elsepart(); // jacob
    bool whilestmt(); // jafet
    bool assignstmt(); // jacob
    bool inputstmt(); // jafet
    bool outputstmt(); // jafet
    bool expr(); // abraham
    bool simpleexpr(); // jafet
    bool term(); // abraham
    bool logicop(); // jafet
    bool arithop(); // abraham
    bool relop(); // jacob

public:
    SyntaxAnalyzer(istream& infile);
    // pre: 1st parameter consists of an open file containing a source code's
    //	valid scanner/lexical analyzer output.  This data must be in the form: token : lexeme
    // post: the vectors have been populated

    bool parse();
    // pre: none
    // post: The lexemes/tokens have been parsed.
    // If an error occurs, a message prints indicating the token/lexeme pair
    // that caused the error.
    // If no error, vectors contain syntactically correct source code
};
#endif //SYNTAXANALYZER_H
