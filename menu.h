#ifndef MENU_H
#define MENU_H
#include "migration.h"

class menu : public Migration
{
    bool isValidDate(const string&);
    bool isChronological(const string&, const string&);
    bool isDigit(const string&, size_t);
    bool isBornBeforeReg(int, const string&);
    bool isValidInitials(string&);
    void capitalFirstLetter(string &);
    int dateToInt(const string&);
    void readString(ifstream&, string&);
    void writeString(ofstream&, const string&);

    char* makePass();
    void encryptFile();
    void decryptFile();

    void loadFromFile(const string&);
    void showBase();
    void addNewMigrant();
    void deleteEntries();
    void editEntries();
    void listOfRegistered();
    void searchMenu();
    void leavingMigrants();
    void saveToFile(const string&);
public:
    int run(const char*);
    friend ostream& operator << (ostream&, const Migrant&);
};

#endif // MENU_H
