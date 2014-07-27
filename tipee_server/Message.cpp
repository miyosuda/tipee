#include "Message.h"
#include "Expression.h"
#include "constants.h"

/**
 * <!--  Message():  -->
 */
Message::Message( int fukidashi_,
				  Expression* expression_,
				  const char* str_,
				  double durationSec )
	:
	fukidashi(fukidashi_),
	expression(expression_),
	str(str_) {
	
	if( durationSec <= 0.0 ) {
		durationCount = -1;
	} else {
		durationCount = (int)(durationSec * (1000.0 / (double)INTERVAL_MILLIS));
	}
}

/**
 * <!--  update():  -->
 *
 * @return 終了する時true
 */
bool Message::update() {
	if( durationCount >= 0 ) {
		durationCount--;
		if( durationCount <= 0 ) {
			return true;
		} else {
			return false;
		}
	} else {
		// 無期限の場合
		return false;
	}
}
