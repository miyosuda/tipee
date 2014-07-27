#include "MessageManager.h"
#include "Message.h"
#include "Expression.h"

#include <stdlib.h> // for NULL

/**
 * <!--  MessageManager():  -->
 */
MessageManager::MessageManager() {
}

/**
 * <!--  ~MessageManager():  -->
 */
MessageManager::~MessageManager() {
	clearMessages();
}

void MessageManager::addMessage( const char* str, Expression* expression, int fukidashi,
								 double durationSec ) {
	Message* message = new Message( fukidashi, expression, str, durationSec );
	messages.push_back(message);
}

void MessageManager::clearMessages() {
	for(int i=0; i<messages.size(); ++i) {
		delete messages[i];
	}
	messages.clear();
}

Message* MessageManager::getCurrentMessage() {
	if( !messages.empty() ) {
		return messages[0];
	} else {
		return NULL;
	}
}

void MessageManager::update() {
	Message* message = getCurrentMessage();
	if( message != NULL ) {
		bool finished = message->update();
		if( finished ) {
			messages.pop_front();
			delete message;
		}
	}
}
