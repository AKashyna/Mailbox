#ifndef DATABASE_H
#define DATABASE_H

#include <string>

int createAccaunt(const std::string &username, const std::string &password);
bool czyIstnieje(const std::string &username, const std::string &password);
int create();

#endif // DATABASE_H