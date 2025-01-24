#include <iostream>
#include <sqlite3.h>
#include <vector>
#include <tuple>
#include <string>
#include <cstring>
#include "Database.h"

using namespace std;

//funkcje do User
int createAccount(const string &username, const string &password) {
    sqlite3 *db;
    if (sqlite3_open("MailBase.db", &db)) {
        return 0;
    }

    const char *insertSQL = "INSERT INTO Users (UserName, Password) VALUES (?, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return 0;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return result == SQLITE_DONE;
}

bool czyIstnieje(const string &username, const string &password) {
    sqlite3 *db;
    if (sqlite3_open("MailBase.db", &db)) {
        return false;
    }

    const char *selectSQL = "SELECT COUNT(*) FROM Users WHERE UserName = ? AND Password = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(stmt, 0) > 0;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return exists;
}

bool czyIstniejeUzytkownik(const string &username) {
    sqlite3 *db;
    if (sqlite3_open("MailBase.db", &db)) {
        return false;
    }

    const char *selectSQL = "SELECT COUNT(*) FROM Users WHERE UserName = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(stmt, 0) > 0;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return exists;
}

//funkcje do Message
bool addMessage(const string &sender, const string &receiver, const string &subject, const string &content){
    sqlite3 *db;
    if (sqlite3_open("MailBase.db", &db)) {
        return 0;
    }

    const char *insertSQL = "INSERT INTO Messages (Sender, Receiver, Topic, Message) VALUES (?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return 0;
    }

    sqlite3_bind_text(stmt, 1, sender.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, receiver.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, subject.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, content.c_str(), -1, SQLITE_STATIC);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return success;
}

vector<tuple<string, string>> getTopics(const string &receiver) {
    sqlite3 *db;
    if (sqlite3_open("MailBase.db", &db)) {
        return {};
    }

    const char *selectSQL = "SELECT Sender, Receiver, Topic, Message FROM Messages WHERE Receiver = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return {};
    }

    sqlite3_bind_text(stmt, 1, receiver.c_str(), -1, SQLITE_STATIC);

    vector<tuple<string, string>> messages;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string sender = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
        string subject = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        messages.emplace_back(sender, subject);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return messages;
}

string getMessage(const string &sender, const string &receiver, const string &topic) {
    sqlite3 *db;
    if (sqlite3_open("MailBase.db", &db)) {
        return {};
    }

    const char *selectSQL = "SELECT Message FROM Messages WHERE Sender = ? and Receiver = ? and Topic = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return {};
    }

    sqlite3_bind_text(stmt, 1, sender.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, receiver.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, topic.c_str(), -1, SQLITE_STATIC);

    string message;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        message = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return message;
}

int create(){
    sqlite3* db;

    if (sqlite3_open("MailBase.db", &db)) {
        return -1;
    }

    string createUsersTableSQL = R"(
        CREATE TABLE IF NOT EXISTS Users (
            ID INTEGER PRIMARY KEY AUTOINCREMENT,
            UserName TEXT NOT NULL,
            Password TEXT NOT NULL
        );
    )";

    string createMessagesTableSQL = R"(
    CREATE TABLE IF NOT EXISTS Messages (
        ID INTEGER PRIMARY KEY AUTOINCREMENT,
        Sender TEXT NOT NULL,
        Receiver TEXT NOT NULL,
        Topic TEXT NOT NULL,
        Message TEXT NOT NULL,
        FOREIGN KEY(Sender) REFERENCES Users(UserName),
        FOREIGN KEY(Receiver) REFERENCES Users(UserName)
        );
    )";

    char* errorMessage = nullptr;
    if (sqlite3_exec(db, createUsersTableSQL.c_str(), nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        sqlite3_free(errorMessage);
        return -1;
    }
    if (sqlite3_exec(db, createMessagesTableSQL.c_str(), nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        sqlite3_free(errorMessage);
        return -1;
    }

    sqlite3_close(db);
    return 0;
}
