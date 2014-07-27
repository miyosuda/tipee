// -*- C++ -*-
#ifndef EXPRESSIONMANAGER_HEADER
#define EXPRESSIONMANAGER_HEADER

#include <string>
#include <map>

using namespace std;

class Expression;

class ExpressionManager {
private:
	map<int, Expression*> expressionIdTable;
	map<string, Expression*> expressionNameTable;

public:
	~ExpressionManager();
	bool load();
	Expression* getExpressionByName(const char* name);
	Expression* getExpressionById(int id);
};

#endif
