#include "Movement.h"

/**
 * <!--  Movement():  -->
 */
Movement::Movement() {
	count = 0;
}

/**
 * <!--  update():  -->
 */
void Movement::update() {
	count++;
}

/**
 * <!--  getParam():  -->
 */
void Movement::getParam(double& dx, double& dy) {
	dx = 0.0;

	if( ((count/2) % 2) == 0 ) {
		dy = 0.0;
	} else {
		dy = 10.0;
	}
}
