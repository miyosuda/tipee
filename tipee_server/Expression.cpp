#include "Expression.h"
#include <stdio.h>

#include "constants.h"

/**
 * <!--  set():  -->
 */
void ExpressionFrame::set(double timeSec, int brow_, int eye_, int mouth_,
						  bool cheek_, bool star_, bool heart_ ) {
	// ここでtimeをsecからcount単位に変更
	time = (int)(timeSec * 1000.0 / (double)INTERVAL_MILLIS);
	brow = brow_;
	eye = eye_;
	mouth = mouth_;
	cheek = cheek_;
	star = star_;
	heart = heart_;
}

/**
 * <!--  debugDump():  -->
 */
void ExpressionFrame::debugDump() const {
	printf( "time=%d, brow=%d, eye=%d, mouth=%d\n", 
			time, brow, eye, mouth );
}

/**
 * <!--  getTotaTime():  -->
 */
int Expression::getTotaTime() const {
	return frames[frameSize-1].time;
}

/**
 * <!--  Expression():  -->
 */
Expression::Expression(int frameSize_) {
	frameSize = frameSize_;
	frames = new ExpressionFrame[frameSize];
	count = 0;
}

/**
 * <!--  ~Expression():  -->
 */
Expression::~Expression() {
	delete [] frames;
}

/**
 * <!--  setFrame():  -->
 */
void Expression::setFrame( int frameIndex, 
						   double timeSec, 
						   int brow, int eye, int mouth,
						   bool cheek, bool star, bool heart ) {
	frames[frameIndex].set( timeSec, 
							brow, eye, mouth, 
							cheek, star, heart );
}

/**
 * <!--  reset():  -->
 */
void Expression::reset() {
	count = 0;
}

/**
 * <!--  update():  -->
 */
void Expression::update() {
	count++;
	int totalTime = getTotaTime();
	if( count > totalTime ) {
		count = 0;
	}
}

/**
 * <!--  getExpression():  -->
 */
void Expression::getExpression( int& brow, int& eye, int& mouth,
								bool& cheek, bool& star, bool& heart ) const {
	int frameIndex = 0;
	for(int i=0; i<frameSize; ++i) {
		if( count >= frames[i].time ) {
			frameIndex = i;
		}
	}
	
	ExpressionFrame& frame = frames[frameIndex];
	brow = frame.brow;
	eye = frame.eye;
	mouth = frame.mouth;
	cheek = frame.cheek;
	star = frame.star;
	heart = frame.heart;
};

/**
 * <!--  debugDump():  -->
 */
void Expression::debugDump() const {
	for(int i=0; i<frameSize; ++i) {
		frames[i].debugDump();
	}
}
