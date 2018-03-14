#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <error.h>
#include <errno.h>
#include <thread>
#include <string.h>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <fcntl.h>
#include "queue.h"
#include "message.h"


using namespace std;

const int one = 1;
int max_cli = 100;
int servSock;
vector<Queue> db; // wektor przechowujący dane - kolejki tematów
size_t found;


string get_ip(ifstream &in) {
    char line[50];
    in.getline(line, 50);
    string server = line;
    found = server.find("/");
    server = server.substr(0, found);
    return server;
}


int get_port(ifstream &in) {
    char line[50];
    in.getline(line, 50);
    int port = atoi(line);
    return port;
}


int setSock() {
    // utworzenie socketa serwera
    int servSock = socket(PF_INET, SOCK_STREAM, 0);
    if (servSock < 0){
        error(1, errno, "Socket failed!");
        exit(EXIT_FAILURE);
    }
    
    // zmiana ustawień gniazda
    setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    fcntl(servSock, F_SETFL, O_NONBLOCK);
    
    return servSock;
}


void delete_messages() {
    for (unsigned int tag = 0; tag < db.size(); tag++){
        for (unsigned int msg = 0; msg < db[tag].lista.size(); msg++){
            db[tag].lista[msg].visited--;
            if (db[tag].lista[msg].visited <= 0){
                cout << "wiadomość: " << db[tag].lista[msg].text;
                db[tag].removemessage(msg);
                cout << "została usunięta" << endl << endl;
            }
        }
    }
}


int change_size(char rozmiar[]) {
    string msg_size = rozmiar;
    string::size_type pos = msg_size.find_first_not_of('0');
    
    if (pos != string::npos) {
        msg_size.erase(0, pos);
    } else {
        msg_size = "0";
    }
    
    return stoi(msg_size);
}


void print_set(unordered_set<int> secik){
    for (auto elem : secik){
        cout << elem << " ";
    }
}


string fill_0(string text, unsigned int size){
    while (text.size() < size){
        text = "0" + text;
    }
    return text;
}


void writeData(int sock, string data){
    string msg_size = fill_0(to_string(data.size()), 5);
    if (write(sock, msg_size.c_str(), 5) == -1){
        cout << "Write went wrong!" << endl;
    }
    if (write(sock, data.c_str(), data.size()) == -1){
        cout << "Write went wrong!" << endl;
    }
}


void send_oMsg_aSub(int tag, int msg){
    string text = db[tag].tag + ": " + db[tag].lista[msg].text;
    
    for (auto sub : db[tag].subscribers){
        writeData(sub, text);
    }
}


void send_aMsg_oSub(int sub, int tag){
    string text = "";
    
    for (unsigned int msg = 0; msg < db[tag].lista.size(); msg++){
        text = text + db[tag].tag + ": " + db[tag].lista[msg].text;
    }
    writeData(sub, text);
}


int new_subscriber(int j, int fd) {
    db[j].addsubscriber(fd);
    
    cout << "tag: " << db[j].tag << endl << "subskrybenci: ";
    print_set(db[j].subscribers);
    cout << endl << endl;
    
    // wysłanie
    send_aMsg_oSub(fd, j);
    return 1;
}    


void add_new_tag(string text, int fd) {
    Queue m(text);
    m.addsubscriber(fd); // i dodajemy pierwszego subskrybenta
    db.push_back(m);
    
    cout << "Dodałem nowy tag:" << endl;
    cout << "tag: " << db.back().tag << endl << "subskrybenci: ";
    print_set(db.back().subscribers);
    cout << endl << endl;
}


void del_subscriber(int fd) {
    cout << "Usuwam subskrybenta " << fd << " z list: ";
    for (unsigned int i = 0; i < db.size(); i++){
        if (db[i].removesubscriber(fd) > 0)
            cout << db[i].tag << " ";
    }
    cout << endl << endl;
}


int find_in_db(string tag, Message msg) {
    for (unsigned int i = 0; i < db.size(); i++){ // analogicznie szukamy czy dany tag znajduje się już w naszej bazie danych
        if (db[i].tag == tag){
            db[i].addmessage(msg); // jeśli tak to dodajemy tylko nową wiadomość z danym tagiem do kolejki
            
            cout << "tag: " << db[i].tag << endl
            << "Nowy post: " << endl
            << db[i].lista.back().text << endl;
            
            // wysylanie
            send_oMsg_aSub(i, db[i].lista.size() - 1);
            return 1;
        }
    }
    return 0;
}


void new_tag_msg(string tag, Message msg) {
    Queue q(tag);
    q.addmessage(msg);
    db.push_back(q);
    
    cout << "Dodalem nowy tag: " << endl;
    cout << "tag: " << db.back().tag << endl
    << "Nowy post: " << endl
    << db.back().lista.back().text << endl;
}


void publish(string text) {
    found = text.find("/:"); // szukamy znaku specjalnego aby wydzielić część wiadomości reprezentującą tag i właściwą wiadomość
    string tag = text.substr(0, found);
    Message msg(text.substr(found + 2));
    
    if (find_in_db(tag, msg) == 0){ // jeśli nie to analogicznie
        new_tag_msg(tag, msg);
    }
}


void process_message(string text, int fd) {
    int czy_jest = 0;
    
    if (text[0] == '#'){ // rozpoznanie czy dana wiadomość jest prośbą o subskrypcję czy publikację
        text.erase(0, 1);
        for (unsigned int j = 0; j < db.size(); j++){ // szukamy czy przesłany tag znajduje się już w naszej bazie danych
            if (db[j].tag == text){ // jeśli istnieje to dodajemy kolejnego subskrybenta
                czy_jest = new_subscriber(j, fd);
                break;
            }
        }
        
        if (czy_jest == 0){ // jeśli nie było tworzymy nowy obiekt kolejki z żądanym tematem
            add_new_tag(text, fd);
        }
    } else if (text == "closeconn") {
        del_subscriber(fd);
        close(fd);
    } else if (text.size() > 0) { // obsługa prośby o publikację
        publish(text);
    }
}


int main(){
    cout << "Starting server..." << endl;
    
    // wczytanie z pliku danych potrzebnych do uruchomienia serwera
    ifstream in("ser_data.txt");
    
    if (!in){
        perror("Nie można otworzyć pliku!");
        exit(EXIT_FAILURE);
    }
    
    string server = get_ip(in); 
    int port = get_port(in);
    
    in.close();
    
    // struktura adresu lokalnego
    sockaddr_in localAddress;
    localAddress.sin_family = AF_INET;
    localAddress.sin_port = htons(port);
    localAddress.sin_addr.s_addr = inet_addr(server.c_str());
    
    int servSock = setSock();
    // powiązanie gniazda z adresem lokalnym
    if (bind(servSock, (sockaddr*)&localAddress, sizeof(localAddress))) {
        error(1, errno, "Bind failed!");
    }
    
    // nasłuchiwanie na sockecie serwera
    listen(servSock, max_cli);
    
    // deklaracja tablicy struktur, przechowującej informacje o zdarzeniach
    pollfd nacoczekac[max_cli]{};
    nacoczekac[0].fd = servSock;
    nacoczekac[0].events = POLLIN;
    
    unsigned int count = 1;
    int clSck;
    
    ssize_t roz = 5;
    char rozmiar[roz];
    string text, tag, msg;
    
    cout << "Server ready." << endl << endl;
    
    // wątek usuwający z kolejek przedawnione wiadomości (które są w kolejce dłużej niż 1 min)
    std::thread cleaner([&]{
        while (1){
            sleep(10);
            delete_messages();
        }
    });
    
    
    while (1){
        // odbieranie połączeń
        poll(nacoczekac, max_cli, -1); // oczekuje na zdarzenie
        
        if (nacoczekac[0].revents & POLLIN){ // jeśli nastąpiło zdarzenie typu IN
            clSck = accept(servSock, 0, 0); // to akceptowane jest połączenie od klienta
            
            cout << "Zaakceptowałem połączenie od nowego klienta: " << clSck << endl << endl;
            
            nacoczekac[count].fd = clSck;
            nacoczekac[count].events = POLLIN;
            count++;
        }
        
        // odbieranie wiadomości
        for (unsigned int i = 1; i < count; i++){
            if (nacoczekac[i].revents & POLLIN){
                if (recv(nacoczekac[i].fd, rozmiar, roz, MSG_WAITALL) != roz) {
                    continue;
                }
                
                int new_size = change_size(rozmiar);
                cout << "czekam na wiadomość o rozmiarze: " << new_size << endl << endl;
                
                char buffer[new_size + 1]{};
                if (recv(nacoczekac[i].fd, buffer, new_size, MSG_WAITALL) != new_size) {
                    continue;
                }
                
                found = 0;
                text.clear();
                tag.clear();
                msg.clear();
                
                text = buffer;
                process_message(text, nacoczekac[i].fd);
                
            }
        }
    }
    
    return 0;
}

