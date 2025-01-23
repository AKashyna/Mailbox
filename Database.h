#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>

using namespace std;

//user
int createAccount(const string &username, const string &password);
bool czyIstnieje(const string &username, const string &password);
bool czyIstniejeUzytkownik(const string &username);
//message
bool addMessage(const string &sender, const string &receiver, const string &subject, const string &content);
vector<tuple<string, string>> getTopics(const string &receiver);
string getMessage(const string &sender, const string &receiver, const string &topic);

int create();

#endif // DATABASE_H