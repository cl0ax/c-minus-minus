//
// The code was created by Jacob, Jafet, Abraham.
//
// The code looks through a file of valid lexemes that are
// syntactically correct and generates a instruction table
// and executes the instruction table as code.
//
// Functions made by Jacob: WhileStmt, StrOutStmt, ConstExpr,
// buildWhile, buildExpr, populateTokenLexemes, and run.
//
// Functions made by Abraham: AssignStmt, ExprOutStmt, PostFixExpr,
// buildAssign, buildOutput, buildStmt, and compile.
//
// Functions made by Jafet: IfStmt, InputStmt, GoToStmt, IdExpr,
// buildIf, buildInput, and populateSymbolTable.

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <stack>
#include <string>
using namespace std;

class Expr;
class Stmt;

int pc;
vector<string> lexemes;
vector<string> tokens;
vector<string>::iterator lexitr;
vector<string>::iterator tokitr;
map<string, string> vartable;
vector<Stmt *> insttable;
map<string, string> symboltable;

void dump();

class Expr{
public:
	virtual string toString() = 0;
	virtual ~Expr(){}
};
class IntExpr : public Expr {
public:
	virtual int eval() = 0;
};

class StringExpr : public Expr {
public:
	virtual string* eval() = 0;
};

// Made by Jacob
class IntConstExpr : public IntExpr{
private:
	int value;
public:
	IntConstExpr(int val) {
		value = val;
	}
	int eval() {
		return value;
	};
	string toString() {
		return std::to_string(value);
	}
};
// Made by Jacob
class StringConstExpr : public StringExpr{
private:
	string value;
public:
	StringConstExpr(string val) {
		value = val;
	}
	string* eval() {
		return &value;
	};
	string toString() {
		return value;
	}
};

// Made by Jafet
class IntIdExpr : public IntExpr {
private:
	string id;
public:
	IntIdExpr(string intId) {
		id = intId;
	};

	int eval() {
		return stoi(vartable[id]);
	};

	string toString() {
		return "id: " + id + "eval: " + vartable[id];
	};
};
// Made by Jafet
class StringIdExpr : public StringExpr {
private:
	string id;
public:
	StringIdExpr(string stringId) {
		id = stringId;
	};

	string* eval() {
		return &vartable[id];
	};

	string toString() {
		return "id: " + id + "eval: " + vartable[id];
	};
};

// Made by Abraham
class IntPostFixExpr : public IntExpr{
private:
	vector<string> postfixExprs;
	vector<string> tokens;
public:
	IntPostFixExpr(vector<string>& exprTokens, vector<string>& opTokens) {
		postfixExprs = exprTokens;
		tokens = opTokens;
	}
	~IntPostFixExpr() {}

	int eval() {
		stack<int> evalStack;
		for (string& token : postfixExprs) {
			if (isdigit(token[0])) {
				evalStack.push(stoi(token));
			}
			else if (vartable.contains(token)) {
				evalStack.push(stoi(vartable[token]));
			}
			else {
				int right = evalStack.top(); evalStack.pop();
				int left = evalStack.top(); evalStack.pop();
				if (token == "+") {
					evalStack.push(left + right);
				}
				else if (token == "-") {
					evalStack.push(left - right);
				}
				else if (token == "*") {
					evalStack.push(left * right);
				}
				else if (token == "/") {
					evalStack.push(left / right);
				}
				else if (token == "%") {
					evalStack.push(left % right);
				}
				else if (token == "and") {
					evalStack.push(left && right);
				} else if (token == "or") {
					evalStack.push(left || right);
				} else if (token == "==") {
					evalStack.push(left == right);
				} else if (token == "!=") {
					evalStack.push(left != right);
				} else if (token == ">=") {
					evalStack.push(left >= right);
				} else if (token == "<=") {
					evalStack.push(left <= right);
				} else if (token == ">") {
					evalStack.push(left > right);
				} else if (token == "<") {
					evalStack.push(left < right);
				} else {
					return 0;
				}
			}
		}
		if (evalStack.empty()) {
			return 0;
		}
		return evalStack.top();
	}
	string toString() {
		string result;
		for (const string& token : postfixExprs) {
			result += token + " ";
		}
		return result;
	}
};
// Made by Abraham
class StringPostFixExpr : public StringExpr {
private:
	vector<string> postfixExprs;
	vector<string> tokens;
	string result;
public:
	StringPostFixExpr(vector<string>& exprTokens, vector<string>& opTokens) {
		postfixExprs = exprTokens;
		tokens = opTokens;
	}
	~StringPostFixExpr(){}

	string* eval() {
		stack<string*> evalStack;
		stack<string*> typeStack;
		string basicType = "t_text";

		for (int i = 0; i < postfixExprs.size(); ++i) {
			if (isalnum(postfixExprs.at(i)[0])) {
				evalStack.push(&postfixExprs.at(i));
				typeStack.push(&tokens.at(i));
			}
			else {
				string* right;
				string* left;

				if (*typeStack.top() == "t_string") {
					right = &vartable[*evalStack.top()];
				}
				else {
					right = evalStack.top();
				}
				evalStack.pop(); typeStack.pop();
				if (*typeStack.top() == "t_string") {
					left = &vartable[*evalStack.top()];
				}
				else {
					left = evalStack.top();
				}
				evalStack.pop(); typeStack.pop();

				if (postfixExprs.at(i) == "+") {
					string combined = *left + *right;
					evalStack.push(&combined);
				}
				else if (postfixExprs.at(i) == "and") {
					if (left != nullptr && right != nullptr) {
						evalStack.push(left);
					}
					else {
						evalStack.push(nullptr);
					}
				}
				else if (postfixExprs.at(i) == "or") {
					if (left != nullptr || right != nullptr) {
						evalStack.push(left);
					}
					else {
						evalStack.push(nullptr);
					}
				}
				else if (postfixExprs.at(i) == "==") {
					if (left->compare(*right) == 0) {
						evalStack.push(left);
					}
					else {
						evalStack.push(nullptr);
					}
				}
				else if (postfixExprs.at(i) == "!=") {
					if (left->compare(*right) != 0) {
						evalStack.push(left);
					}
					else {
						evalStack.push(nullptr);
					}
				}
				else if (postfixExprs.at(i) == ">=") {
					if (left->compare(*right) >= 0) {
						evalStack.push(left);
					}
					else {
						evalStack.push(nullptr);
					}
				}
				else if (postfixExprs.at(i) == "<=") {
					if (left->compare(*right) <= 0) {
						evalStack.push(left);
					}
					else {
						evalStack.push(nullptr);
					}
				}
				else if (postfixExprs.at(i) == ">") {
					if (left->compare(*right) > 0) {
						evalStack.push(left);
					}
					else {
						evalStack.push(nullptr);
					}
				}
				else if (postfixExprs.at(i) == "<") {
					if (left->compare(*right) < 0) {
						evalStack.push(left);
					}
					else {
						evalStack.push(nullptr);
					}
				}
				typeStack.push(&basicType);
			}
		}
		if (evalStack.top() != nullptr) {
			result = *evalStack.top();
			return &result;
		}
		return nullptr;
	}
	string toString() {
		string result;
		for (const string& token : postfixExprs) {
			result += token + " ";
		}
		result += "\n";
		for (const string& token : tokens) {
			result += token + " ";
		}
		return result;
	}
};

class Stmt{
private:
	string name;
public:
	Stmt(){}
	virtual ~Stmt(){};
	virtual string toString() = 0;
	virtual void execute() = 0;
};
// Made by Abraham
class AssignStmt : public Stmt{
private:
	string var;
	Expr* p_expr;
public:
	AssignStmt(string varName, Expr* expr) {
		var = varName;
		p_expr = expr;
	}
	~AssignStmt() {
		delete p_expr;
	}
	string toString() {
		return "Assign " + var + " = " + p_expr->toString();
	}
	void execute() {
		IntExpr* intExpr = dynamic_cast<IntExpr*>(p_expr);
		if (intExpr) {
			int result = intExpr->eval();
			vartable[var] = to_string(result);
		}
		else {
			StringExpr* stringExpr = dynamic_cast<StringExpr*>(p_expr);
			if (stringExpr) {
				string* result = stringExpr->eval();
				vartable[var] = *result;
			}
		}
		++pc;
	}
};

// Made by Jafet
class InputStmt : public Stmt {
private:
	string var;
public:
	InputStmt(string s) {
		var = s;
	};
	~InputStmt() {}

	string toString() {
		return "var: " + var;
	};

	void execute() {
		cin >> vartable[var];
		++pc;
	};
};

// Made by Jacob
class StrOutStmt : public Stmt{
private:
	string value;
public:
	StrOutStmt(string val) {
		value = val;
	}
	~StrOutStmt() {}

	string toString() {
		return value;
	}
	void execute() {
		cout << value;
		++pc;
	}
};

// Made by Abraham
class ExprOutStmt : public Stmt {
private:
	Expr* p_expr;
public:
	ExprOutStmt(Expr* expr) {
		p_expr = expr;
	}
	~ExprOutStmt() {
		delete p_expr;
	}
	string toString() {
		return "(" + p_expr->toString() + ")";
	}
	void execute() {
		IntExpr* intExpr = dynamic_cast<IntExpr*>(p_expr);
		if (intExpr) {
			cout << intExpr->eval() << endl;
		}
		else {
			StringExpr* stringExpr = dynamic_cast<StringExpr*>(p_expr);
			if (stringExpr) {
				cout << (*stringExpr->eval()) << endl;
			}
		}
		++pc;
	}
};

// Made by Jafet
class IfStmt : public Stmt {
private:
	Expr *p_expr;
	int elsetarget;

public:
	IfStmt(Expr *e) {
		p_expr = e;
	};

	~IfStmt() {
		delete p_expr;
	};

	string toString() {
		return "p_expr: " + p_expr->toString() + "elsetarget: " + to_string(elsetarget) + "pc: " + to_string(pc);
	};

	void setTarg(int target) {
		elsetarget = target;
	}

	void execute() {
		if (p_expr != nullptr) {
			++pc;
			IntExpr *expr = dynamic_cast<IntExpr *>(p_expr);
			if (expr != nullptr) {
				if (expr->eval() == 0) {
					pc = elsetarget;
				}
			}
			StringExpr *expr2 = dynamic_cast<StringExpr *>(p_expr);
			if (expr2 != nullptr) {
				if (expr2->eval() == nullptr) {
					pc = elsetarget;
				}
			}
		}
	}
};

// Made by Jacob
class WhileStmt : public Stmt{
private:
	Expr* p_expr;
	int elsetarget;
public:
	WhileStmt() = default;
	~WhileStmt() {
		delete p_expr;
	}
	string toString() {
		string s = p_expr->toString();
		s.append(" : ");
		s.append(to_string(elsetarget));
		return s;
	};
	void execute() {
		IntExpr* expr = dynamic_cast<IntExpr*>(p_expr);
		StringExpr* expr2 = dynamic_cast<StringExpr*>(p_expr);
		if (expr != nullptr && expr->eval() != 0) {
			++pc;
		}
		else if (expr2 != nullptr && expr2->eval() != nullptr) {
			++pc;
		}
		else {
			pc = elsetarget;
		}
	}
	void setExpr(Expr* expr) {
		p_expr = expr;
	}
	void setTar(int target) {
		elsetarget = target;
	}
};

// Made by Jafet
class GoToStmt : public Stmt {
private:
	int target;
public:
	GoToStmt() {};

	~GoToStmt() {};

	string toString() {
		return "target: " + to_string(target) + " pc: " + to_string(pc);
	};

	void setTarg(int targ) {
		target = targ;
	}

	void execute() {
		pc = target;
	};
};

// Made by Abraham
class Compiler{
private:
	map<string, int> precMap;
	// Made by Jafet
	void buildIf() {
		tokitr += 2;
		lexitr += 2;
		IfStmt *ifStmt = new IfStmt(buildExpr());
		insttable.push_back(ifStmt);
		++tokitr; ++lexitr;
		while (*tokitr != "s_rbrace") {
			buildStmt();
		}
		ifStmt->setTarg(insttable.size() + 1);
		++tokitr; ++lexitr;
		if (*tokitr == "t_else") {
			GoToStmt *goToStmt = new GoToStmt();
			insttable.push_back(goToStmt);
			tokitr += 2;
			lexitr += 2;
			while (*tokitr != "s_rbrace") {
				buildStmt();
			}
			++tokitr; ++lexitr;
			goToStmt->setTarg(insttable.size());
		}
	};
	// Made by Jacob
	void buildWhile() {
		tokitr += 2;
		lexitr += 2;
		WhileStmt* wStmt = new WhileStmt();
		wStmt->setExpr(buildExpr());
		insttable.push_back(wStmt);
		int index = insttable.size();
		++tokitr;
		++lexitr;

		while (*tokitr != "s_rbrace") {
			buildStmt();
		}
		++tokitr;
		++lexitr;
		wStmt->setTar(insttable.size() + 1);
		GoToStmt* gtStmt = new GoToStmt();
		gtStmt->setTarg(index-1);
		insttable.push_back(gtStmt);
	}
	// Made by Abraham
	void buildStmt() {
		if (*tokitr == "t_if") {
			buildIf();
		}
		else if (*tokitr == "t_while") {
			buildWhile();
		}
		else if (*tokitr == "t_id") {
			buildAssign();
		}
		else if (*tokitr == "t_input") {
			buildInput();
		}
		else if (*tokitr == "t_output") {
			buildOutput();
		}
	}
	// Made by Abraham
	void buildAssign() {
		string var = *lexitr;
		++tokitr; ++lexitr;
		++tokitr; ++lexitr;
		Expr* expr = buildExpr();
		AssignStmt* stmt = new AssignStmt(var, expr);
		insttable.push_back(stmt);
	}
	// Made by Jafet
	void buildInput() {
		tokitr += 2;
		lexitr += 2;
		InputStmt* input = new InputStmt(*lexitr);
		insttable.push_back(input);
		tokitr += 2;
		lexitr += 2;
	};
	// Made by Abraham
	void buildOutput() {
		++tokitr; ++lexitr;
		++tokitr; ++lexitr;
		Stmt* stmt = nullptr;

		if (*tokitr == "t_text") {
			string text = *lexitr;
			++tokitr; ++lexitr;
			if (*tokitr == "s_rparen") {
				stmt = new StrOutStmt(text);
			}
			else {
				--tokitr; --lexitr;
			}
		}
		if (stmt == nullptr) {
			Expr* expr = buildExpr();
			stmt = new ExprOutStmt(expr);
			--tokitr; --lexitr;
		}
		++tokitr; ++lexitr;
		insttable.push_back(stmt);
	}

	// Made by Jacob
	bool isOperator(string terms) {
		if (terms == "s_plus" || terms == "s_minus" || terms == "s_mult" || terms == "s_div" || terms == "s_mod"
			|| terms == "t_or" || terms == "t_and" || terms == "s_eq" || terms == "s_ne"
			|| terms == "s_ge" || terms == "s_le" || terms == "s_lt" || terms == "s_gt" ) {
			return true;
			}
		return false;
	}
	// Made by Jacob
	Expr* buildExpr() {
		bool isInt = false;
		bool isString = false;
		bool crash = false;
		int paren = 1;
		vector<string> result;
		vector<string> tokens;
		stack<string> s;

		while (paren != 0 && !crash) {
			if (*tokitr == "t_id") {
				if (symboltable[*lexitr] == "t_integer" && !isString) {
					isInt = true;
					result.push_back(*lexitr);
					tokens.push_back("t_integer");
				}
				else if (symboltable.find(*lexitr)->second == "t_string" && !isInt) {
					isString = true;
					result.push_back(*lexitr);
					tokens.push_back("t_string");
				}
				else {
					crash = true;
				}
			}
			else if (*tokitr == "t_number" && !isString) {
				isInt = true;
				result.push_back(*lexitr);
				tokens.push_back("t_number");
			}
			else if (*tokitr == "t_text" && !isInt) {
				isString = true;
				result.push_back(*lexitr);
				tokens.push_back("t_text");
			}
			else if (*tokitr == "s_lparen") {
				s.push(*tokitr);
				++paren;
			}
			else if (*tokitr == "s_rparen") {
				--paren;
				while (!s.empty() && s.top() != "s_lparen") {
					result.push_back(s.top());
					tokens.push_back("s_op");
					s.pop();
				}
				if (!s.empty()) {
					s.pop();
				}
			}
			else if (*tokitr == "s_semi") {
				--paren;
			}
			else if (isOperator(*tokitr)) {
				while (!s.empty() && s.top() != "s_lparen" && precMap.find(*lexitr)->second >= precMap[s.top()]) {
					result.push_back(s.top());
					tokens.push_back("s_op");
					s.pop();
				}
				s.push(*lexitr);
			}
			else {
				crash = true;
			}
			++tokitr; ++lexitr;
		}
		if (!s.empty()) {
			result.push_back(s.top());
			tokens.push_back("s_op");
		}
		if (crash == true) {
			for (auto stmts: insttable) {
				delete stmts;
			}
			exit(-2);
		}
		Expr* exprs = nullptr;

		if (isInt) {
			if (result.size() == 1) {
				if (tokens[0] == "t_integer") {
					exprs = new IntIdExpr(result[0]);
				}
				else{
					exprs = new IntConstExpr(stoi(result[0]));
				}
			}
			else {
				exprs = new IntPostFixExpr(result, tokens);
			}
		}
		else {
			if (result.size() == 1) {
				if (tokens[0] == "t_string") {
					exprs = new StringIdExpr(result[0]);
				}
				else{
					exprs = new StringConstExpr(result[0]);
				}
			}
			else {
				exprs = new StringPostFixExpr(result, tokens);
			}
		}
		return exprs;
	}
	// Made by Jacob
	void populateTokenLexemes(istream& infile) {
		string wordPair;
		while (getline(infile, wordPair)) {
			unsigned int pos = wordPair.find(" ");
			string token = wordPair.substr(0, pos);
			string lex = wordPair.substr(pos + 1, wordPair.size());
			lexemes.push_back(lex);
			tokens.push_back(token);
		}
		tokitr = tokens.begin();
		lexitr = lexemes.begin();
	}
	// Made by Jafet
	void populateSymbolTable(istream& infile) {
		string wordPair;
		while (getline(infile, wordPair)) {
			unsigned int pos = wordPair.find(' ');
			string var = wordPair.substr(0, pos);
			string type = wordPair.substr(pos + 1, wordPair.size());
			symboltable[var] = type;
		}
	}
public:
	Compiler() {}
	Compiler(istream& source, istream& symbols){
		precMap["or"] = 5; precMap["and"] = 4;
		precMap["=="] = 3; precMap["!="] = 3; precMap[">="] = 3;
		precMap["<="] = 3; precMap[">"] = 3; precMap["<"] = 3;
		precMap["+"] = 2; precMap["-"] = 2; precMap["*"] = 1;
		precMap["/"] = 1; precMap["%"] = 1;

		populateTokenLexemes(source);
		populateSymbolTable(symbols);
	}
	// Made by Abraham
	bool compile() {
		while (*tokitr != "t_main") {
			++tokitr; ++lexitr;
		}
		++tokitr; ++lexitr;
		++tokitr; ++lexitr;
		while (tokitr != tokens.end() && *tokitr != "s_rbrace") {
			buildStmt();
		}
		return true;
	}
	// Made by Jacob
	void run() {
		while (pc < insttable.size()) {
			(insttable.at(pc))->execute();
		}
		for (auto stmts: insttable) {
			delete stmts;
		}
	}
};
// Made by Group
void dump() {
	for (auto var: vartable) {
		cout << var.first << " " << var.second << endl;
	}
	cout << endl;
	for (auto inst: insttable) {
		cout << inst->toString() << endl;
	}
	cout << endl;
	for (auto symbol: symboltable) {
		cout << symbol.first << " " << symbol.second << endl;
	}
	cout << endl;
}
int main(){
	ifstream source("data.txt");
	ifstream symbols("vars.txt");
	if (!source || !symbols) exit(-1);
	Compiler c(source, symbols);
	c.compile();
	//dump();
	c.run();
	return 0;
}
