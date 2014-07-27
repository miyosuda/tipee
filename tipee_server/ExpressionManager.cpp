#include "ExpressionManager.h"
#include "picojson.h"
#include "Expression.h"
#include <iostream>
#include <fstream>

using namespace picojson;

/**
 * <!--  ~ExpressionManager():  -->
 */
ExpressionManager::~ExpressionManager() {
	map<string, Expression*>::iterator it;
	for(it=expressionNameTable.begin(); 
		it!=expressionNameTable.end(); 
		++it) {
		Expression* expression = it->second;
		delete expression;
	}
	expressionNameTable.clear();
	expressionIdTable.clear();
}

/**
 * <!--  load():  -->
 *
 * jsonから表情パターンデータをロード.
 */
bool ExpressionManager::load() {
	ifstream stream("expressions.json");
	if ( !stream.is_open() ) {
		return false;
	}
	
	value root;
	stream >> root;
	array expressions = root.get<array>();
	int expressionSize = expressions.size();
	
	cout << "exp size=" << expressionSize << endl;

	for(int i=0; i<expressions.size(); ++i ) {
		object exp = expressions[i].get<object>();
		string name = exp["name"].get<string>();
		int id = exp["id"].get<double>();
		
		array frames = exp["frames"].get<array>();
		int frameSize = frames.size();
		
		cout << "frame size=" << frameSize << endl;
		
		Expression* expression = new Expression(frameSize);
		
		for(int j=0; j<frameSize; ++j ) {
			object frame = frames[j].get<object>();
			double timeSec = frame["time"].get<double>();
			int brow = frame["brow"].get<double>();
			int eye = frame["eye"].get<double>();
			int mouth = frame["mouth"].get<double>();

			bool cheek = false;
			bool star = false;
			bool heart = false;
			value cheekVal = frame["cheek"];
			if( cheekVal.is<bool>() ) {
				cheek = cheekVal.get<bool>();
			}
			value starVal = frame["star"];
			if( starVal.is<bool>() ) {
				star = starVal.get<bool>();
			}
			value heartVal = frame["heart"];
			if( heartVal.is<bool>() ) {
				heart = heartVal.get<bool>();
			}
			
			expression->setFrame( j, timeSec, brow, eye, mouth,
								  cheek, star, heart );
		}
		
		expressionNameTable[name] = expression;
		expressionIdTable[id] = expression;
	}
	
	return true;
}

/**
 * <!--  getExpression():  -->
 */
Expression* ExpressionManager::getExpressionByName(const char* name) {
	return expressionNameTable[name];
}

/**
 * <!--  getExpression():  -->
 */
Expression* ExpressionManager::getExpressionById(int id) {
	return expressionIdTable[id];
}
