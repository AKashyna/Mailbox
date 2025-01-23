#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <functional>

using namespace std;

bool connect();
bool log(const string &username, const string &password, const string &option);
bool send(const string &reciver, const string &topic, const string &message);
vector<tuple<string, string>> getMessages();
string getMessage(const string &reciver, const string &topic);

#endif // CLIENT_H