#ifndef MIGRATION_H
#define MIGRATION_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>

using namespace std;

struct Migrant {
    string region;
    string reg_date;
    string last_name;
    string initials;
    string nationality;
    int year_of_birth;
    int passp_series;
    int passp_num;
    string purp_of_arr;
    string de_reg_date;
};

class Migration
{
protected:
    vector<Migrant> base;
    vector<string> regions;
    vector<string> nationalities;
public:
    Migration();
    Migration(const Migration& src);
    ~Migration();

    Migration& operator = (const Migration&);
    friend ostream& operator << (ostream&, const Migration&);
};

#endif // MIGRATION_H
