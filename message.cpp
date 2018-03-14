#include <time.h>
#include "message.h"
#include <unistd.h>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

int TTL = 6;		//czas przebywania wiadomości w kolejce: 6*10s = 1min

Message::Message(string t) {
	text = t;
	visited = TTL;
}

Message::~Message() {}


