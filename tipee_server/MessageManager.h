// -*- C++ -*-
#ifndef MESSAGEMANAGER_HEADER
#define MESSAGEMANAGER_HEADER

#include <deque>

using namespace std;

class Expression;
class Message;

class MessageManager {
private:
	deque<Message*> messages;
	
public:
	MessageManager();
	~MessageManager();
	void addMessage( const char* str, Expression* expression, int fukidashi,
					 double durationSec );
	void clearMessages();
	Message* getCurrentMessage();
	void update();
};

#endif
