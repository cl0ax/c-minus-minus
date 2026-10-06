//
// The code was created by Jacob, Jafet, Abraham.
//

#include <iostream>
#include "SyntaxAnalyzer.h"

int main() {
    ifstream test("test.txt");
    SyntaxAnalyzer syntaxAnalyzer(test);
    cout << syntaxAnalyzer.parse() << endl;
    return 0;
}
