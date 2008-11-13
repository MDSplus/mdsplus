#ifndef MESSAGESENDER_H_
#define MESSAGESENDER_H_
class MessageSender 
{
public:
	virtual void sendMessage(char *buf, int size) = 0;
};
#endif /*MESSAGESENDER_H_*/
