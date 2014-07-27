// -*- C++ -*-
#ifndef EXPRESSION_HEADER
#define EXPRESSION_HEADER

/**
 * 表情パターンの1フレーム
 */
class ExpressionFrame {
public:
	int time;
	int brow;
	int eye;
	int mouth;
	bool cheek;
	bool star;
	bool heart;
	
	void set( double timeSec, int brow_, int eye_, int mouth_,
			  bool cheek_, bool star_, bool heart_ );
	void debugDump() const;
};

/**
 * 表情パターン
 */
class Expression {
private:
	int frameSize;
	ExpressionFrame* frames;
	int count;

	int getTotaTime() const;
	
public:
	Expression(int frameSize_);
	~Expression();
	void setFrame( int frameIndex, double timeSec, 
				   int brow, int eye, int mouth,
				   bool cheek, bool star, bool heart );
	void reset();
	void update();
	void getExpression( int& brow, int& eye, int& mouth,
						bool& cheek, bool& star, bool& heart ) const;
	void debugDump() const;
};

#endif
