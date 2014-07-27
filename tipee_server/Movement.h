// -*- C++ -*-
#ifndef MOVEMENT_HEADER
#define MOVEMENT_HEADER

/**
 * 吹き出しの動き
 */
class Movement {
private:
	int count;
	
public:
	Movement();
	void update();
	void getParam(double& dx, double& dy);
};

#endif
