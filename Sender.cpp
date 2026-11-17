#include <iostream>
#include <fstream>
#include <semaphore.h>
using namespace std;

int main(int argc, char* argv[]) {
    fstream f(argv[1], ios::in | ios::out | ios::binary);
    sem_t* ready = sem_open("/ready", 0);
    sem_t* full = sem_open("/full", 0);
    sem_t* empty = sem_open("/empty", 0);
    sem_t* mtx = sem_open("/mtx", 0);
    sem_post(ready);
    char c, msg[20];
    int h[2];
    while (cout << "1 - send, 0 - stop: ", cin >> c && c == '1') {
        cout << "Message: ";
        cin.ignore();
        cin.getline(msg, 20);
        sem_wait(empty);
        sem_wait(mtx);
        f.seekg(0);
        f.read((char*)h, sizeof h);
        f.seekp(8 + h[1] * 20);
        f.write(msg, 20);
        h[1] = (h[1] + 1) % h[0];
        f.seekp(0);
        f.write((char*)h, sizeof h);
        f.flush();
        sem_post(mtx);
        sem_post(full);
    }
}
