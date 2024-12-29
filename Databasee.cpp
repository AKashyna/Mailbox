#include <iostream>
#include <sqlite3.h>
#include "Database.h"

int createAccaunt(const std::string &username, const std::string &password) {
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

bool czyIstnieje(const std::string &username, const std::string &password) {
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

int create(){
    sqlite3* db;

    if (sqlite3_open("MailBase.db", &db)) {
        return -1;
    }

    std::string createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS Users (
            ID INTEGER PRIMARY KEY AUTOINCREMENT,
            UserName TEXT NOT NULL,
            Password TEXT NOT NULL
        );
    )";

    char* errorMessage = nullptr;
    if (sqlite3_exec(db, createTableSQL.c_str(), nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        sqlite3_free(errorMessage);
        return -1;
    }

    sqlite3_close(db);
    return 0;
}