#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

#include <vector>
#include <iostream>
#include <string>
#include <unordered_set>
#include "message.h"

using namespace std;

//klasa reprezentująca kolejkę
class Queue{

public:

	string tag;								//tag (temat) - nazwa kolejki
	vector<Message> lista;					//lista wiadomości opublikowanych z danym tagiem
	unordered_set<int> subscribers;			//set klientów subskrybujących dany tag
	
	Queue(string);
	~Queue();
	void addmessage(Message);
	void removemessage(int);
	//vector<string> readmessages();
	string gettag();
	void addsubscriber(int);
	int removesubscriber(int);
	unordered_set<int> getsubscribers();
};


#endif
