#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include <iostream>
#include <string>
#include <thread>

using namespace std;

//klasa reprezentująca wiadomość
class Message {

public:
	string text;
	int visited;
	
	Message(string);
	~Message();
	
};


#endif
