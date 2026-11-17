#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <semaphore.h>
#include <fcntl.h>
using namespace std;

int main() {
    string name;
    int n, k;
    cout << "File name: ";
    cin >> name;
    cout << "Records: ";
    cin >> n;
    int h[2] = {n, 0};
    char msg[20] = {};
    ofstream out(name, ios::binary);
    out.write((char*)h, sizeof h);
    for (int i = 0; i < n; i++) out.write(msg, 20);
    out.close();
    const char* names[] = {"/ready", "/full", "/empty", "/mtx"};
    for (auto s : names) sem_unlink(s);
    sem_t* ready = sem_open("/ready", O_CREAT, 0644, 0);
    sem_t* full = sem_open("/full", O_CREAT, 0644, 0);
    sem_t* empty = sem_open("/empty", O_CREAT, 0644, n);
    sem_open("/mtx", O_CREAT, 0644, 1);
    cout << "Senders: ";
    cin >> k;
    string dir = filesystem::current_path();
    for (int i = 0; i < k; i++)
        system(("osascript -e 'tell app \"Terminal\" to do script \"cd " + dir + " && ./sender " + name + "\"' > /dev/null").c_str());
    for (int i = 0; i < k; i++) sem_wait(ready);
    ifstream in(name, ios::binary);
    int head = 0;
    char c;
    while (cout << "1 - read, 0 - exit: ", cin >> c && c == '1') {
        sem_wait(full);
        in.seekg(8 + head * 20);
        in.read(msg, 20);
        head = (head + 1) % n;
        sem_post(empty);
        cout << msg << '\n';
    }
    for (auto s : names) sem_unlink(s);
}
