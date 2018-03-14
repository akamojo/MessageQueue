#include "queue.h"
#include "message.h"

Queue::Queue(string t) {
	tag = t;
}

Queue::~Queue() { }

void Queue::addmessage(Message m) {
	lista.push_back(m);
}

void Queue::removemessage(int idx) {
	lista.erase(lista.begin() + idx);
}

/*vector<string> Queue::readmessages() {
	return lista;
}*/

string Queue::gettag() {
	return tag;
}

void Queue::addsubscriber(int fd) {
	subscribers.insert(fd);
}

int Queue::removesubscriber(int fd) {
	return subscribers.erase(fd);
}

unordered_set<int> Queue::getsubscribers() {
	return subscribers;
}
