// -*- C++ -*-
#ifndef MESSAGE_HEADER
#define MESSAGE_HEADER

#include "String.h"

class Expression;

/**
 * セリフ, 吹き出しパターン, 表情パターンをまとめて管理したオブジェクト.
 */
class Message {
private:
	int fukidashi;
	Expression* expression;
	String str;
	int durationCount;
	
public:
	Message( int fukidashi_,
			 Expression* expression_,
			 const char* str_,
			 double durationSec );
	
	const String& getString() const {
		return str;
	}
	
	Expression* getExpression() {
		return expression;
	}

	int getFukidashi() const {
		return fukidashi;
	}

	bool update();
};

#endif
