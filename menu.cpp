#include "menu.h"
#include "migration.h"
#include <algorithm>
#include <regex>

ostream& operator << (ostream& out, const Migrant& migrant) {
    out << migrant.region << " " << migrant.reg_date << " " << migrant.last_name << " " << migrant.initials << " " << migrant.nationality << " " << migrant.year_of_birth << " " << migrant.passp_series << " " << migrant.passp_num << " " << migrant.purp_of_arr << " " << migrant.de_reg_date << "\n";
    return out;
}
bool menu::isDigit(const string &str, size_t length) {
    return str.length() == length && all_of(str.begin(), str.end(), ::isdigit);
}
bool menu::isValidDate(const string &date) {
    if (!regex_match(date, regex(R"(^\d{2}\.\d{2}\.\d{4}$)"))) return false;
    int day = stoi(date.substr(0,2));
    int month = stoi(date.substr(3,2));
    int year = stoi(date.substr(6,4));
    if (month < 1 || month > 12 || year < 1920 || year > 2025) return false;
    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))) {
        days_in_month[1] = 29;
    }
    return ((day >= 1) && (day <= days_in_month[month - 1]));
}
bool menu::isChronological(const string &reg_date, const string &de_reg_date) {
    if (de_reg_date == "r") return true;
    int date1 = stoi(reg_date.substr(6,4))*10000 + stoi(reg_date.substr(3,2))*100 + stoi(reg_date.substr(0,2));
    int date2 = stoi(de_reg_date.substr(6,4))*10000 + stoi(de_reg_date.substr(3,2))*100 + stoi(de_reg_date.substr(0,2));
    return date1 <= date2;
}
bool menu::isBornBeforeReg(int birth, const string &reg_date) {
    int reg_year = stoi(reg_date.substr(6,4));
    return birth <= reg_year;
}
bool menu::isValidInitials(string &initials) {
    if ((initials.length() != 4) || (initials[1] != '.') || (initials[3] != '.')) return false;
    if ((!isalpha(initials[0])) || (!isalpha(initials[2]))) return false;

    initials[0] = toupper(initials[0]);
    initials[2] = toupper(initials[2]);
    return true;
}
void menu::capitalFirstLetter(string &str) {
    if (!str.empty()) {
        str[0] = toupper(str[0]);
        for (size_t i = 1; i < str.size(); ++i) {
            str[i] = tolower(str[i]);
        }
    }
}
void menu::readString(ifstream &in_str, string &str) {
    size_t len;
    in_str.read(reinterpret_cast<char*>(&len), sizeof(len));
    if (in_str.eof()) return;

    vector<char> buf(len);
    in_str.read(buf.data(), len);
    str.assign(buf.begin(), buf.end());
}
void menu::writeString(ofstream &out_str, const string &str) {
    size_t len = str.length();
    out_str.write(reinterpret_cast<const char*>(&len), sizeof(len));
    out_str.write(str.c_str(), len);
}
int menu::dateToInt(const string &date) {
    int d, m, y;
    sscanf(date.c_str(), "%d.%d.%d", &d, &m, &y);
    return ((y*10000) + (m*100) + d);
}

char* menu::makePass() {
    srand(time(NULL));
    char* pass = new char[65];
    for (int i{}; i < 64; ++i) {
        switch (rand() % 3) {
        case 0: {
            pass[i] = rand() % 10 + '0';
            break;
        }
        case 1: {
            pass[i] = rand() % 26 + 'A';
            break;
        }
        case 2: {
            pass[i] = rand() % 26 + 'a';
        }
        }
    }
    pass[64] = '\0';
    return pass;
}
void menu::encryptFile() {
    char* password = makePass();
    string command = "c:\\OpenSSL-Win64\\bin\\openssl.exe enc -aes-256-cbc ";
    command += "-e -in data.bin -out data.bin.enc -iter 200000 -k ";
    command += password;
    system(command.c_str());
    if (remove("data.bin") != 0) {
        cout << "[ERROR] - deleting file" << endl;
    }
    ofstream file;
    file.open("key.txt", ios::binary);
    file.write(password, 64);
    file.close();

    command = "c:\\OpenSSL-Win64\\bin\\openssl.exe pkeyutl -encrypt -inkey ";
    command += "rsa.public -pubin -in key.txt -out key.txt.enc";
    system(command.c_str());
    if (remove("key.txt") != 0) {
        cout << "[ERROR] - deleting file" << endl;
    }
}
void menu::decryptFile() {
    string command = "c:\\OpenSSL-Win64\\bin\\openssl.exe pkeyutl -decrypt ";
    command += "-inkey rsa.private -in key.txt.enc -out key.txt";
    system(command.c_str());
    if (remove("key.txt.enc") != 0) {
        cout << "[ERROR] - deleting file" << endl;
    }
    char* password = new char[65];
    ifstream file;
    file.open("key.txt", ios::binary);
    file.read(password, 64);
    file.close();
    password[64] = '\0';
    if (remove("key.txt") != 0) {
        cout << "[ERROR] - deleting file" << endl;
    }
    command = "c:\\OpenSSL-Win64\\bin\\openssl enc -aes-256-cbc -d ";
    command += "-in data.bin.enc -out data.bin -iter 200000 -k ";
    command += password;
    system(command.c_str());
    if (remove("data.bin.enc") != 0) {
        cout << "[ERROR] - deleting file" << endl;
    }
}

void menu::loadFromFile(const string &filename) {
    system("cls");
    decryptFile();
    base.clear();
    regions.clear();
    nationalities.clear();
    ifstream file_in_bin(filename, ios::binary);
    if (!file_in_bin) {
        cout << "Error: can't open the file! Please, check the correctness of input file name!";
        system("pause");
        return;
    }
    while (file_in_bin.peek() != EOF) {
        Migrant migrant;
        readString(file_in_bin, migrant.region);
        readString(file_in_bin, migrant.reg_date);
        readString(file_in_bin, migrant.last_name);
        readString(file_in_bin, migrant.initials);
        readString(file_in_bin, migrant.nationality);
        file_in_bin.read(reinterpret_cast<char *>(&migrant.year_of_birth), sizeof(int));
        file_in_bin.read(reinterpret_cast<char *>(&migrant.passp_series), sizeof(int));
        file_in_bin.read(reinterpret_cast<char *>(&migrant.passp_num), sizeof(int));
        readString(file_in_bin, migrant.purp_of_arr);
        readString(file_in_bin, migrant.de_reg_date);
        if (!file_in_bin) break;
        base.push_back(migrant);
    }

    for (const auto& migrant : base) {
        if (find(regions.begin(), regions.end(), migrant.region) == regions.end()) {
            regions.push_back(migrant.region);
        }
        if (find(nationalities.begin(), nationalities.end(), migrant.nationality) == nationalities.end()) {
            nationalities.push_back(migrant.nationality);
        }
    }
    cout << "Data uploaded succesfully!\n";
    fflush(stdin);
    system("pause");
}
void menu::showBase() {
    system("cls");
    if (base.empty()) {
        cout << "Database is empty. Please, upload the data first (option 1 in menu)!\n";
    } else {
        cout << "==================== MIGRATION INFO ====================\n";
        for (int i{}; i < base.size(); ++i) {
            cout << (i+1) << ". " << base[i];
        }
    }
    system("pause");
    fflush(stdin);
}
void menu::addNewMigrant() {
    system("cls");
    fflush(stdin);
    if (base.empty()) {
        cout << "Database is empty. Please, upload the data first (option 1 in menu)!\n";
        system("pause");
        return;
    }
    int n;
    while (true) {
        system("cls");
        cout << "How many records do you want to add (enter \"0\" to exit)? ";
        if (!(cin >> n)) {
            cout << "Wrong input! Please enter a valid number.\n";
            cin.clear();
            fflush(stdin);
            system("pause");
            continue;
        }
        if (n == 0) {
            break;
        }
        if (n < 1) {
            cout << "Invalid entry number! Try again.\n";
            system("pause");
            continue;
        }
        break;
    }
    system("cls");
    cout << "Enter new entries:\n";
    for (int i{}; i < n; ) {
        cout << (i+1) << ". ";
        string region, reg_date, last_name, initials, nationality, purp_of_arr, de_reg_date, input_year, input_passp_series, input_passp_num;
        int year_of_birth, passp_series, passp_num;
        while (true) {
            cout << "Enter region: ";
            cin >> region;
            capitalFirstLetter(region);
            if (!region.empty()) break;
            cout << "\tRegion can't be empty! Try again: ";
        }
        while (true) {
            cout << "\tEnter registration date (dd.mm.yyyy): ";
            cin >> reg_date;
            if (isValidDate(reg_date)) break;
            cout << "\tInvalid date format. Try again! ";
        }
        while (true) {
            cout << "\tEnter last name: ";
            cin >> last_name;
            capitalFirstLetter(last_name);
            if (!last_name.empty()) break;
            cout << "\tLast name can't be empty! Try again: ";
        }
        while (true) {
            cout << "\tEnter initials (format X.X.): ";
            cin >> initials;
            if (isValidInitials(initials)) break;
            cout << "\tInvalid initials format! Use X.X., please! Try again: ";
        }
        while (true) {
            cout << "\tEnter nationality: ";
            cin >> nationality;
            transform(nationality.begin(), nationality.end(), nationality.begin(), [](unsigned char c) {return tolower(c);});
            if (!nationality.empty()) break;
            cout << "\tNationality cannot be empty. Try again.\n";
        }
        while (true) {
            cout << "\tEnter year of birth (format: yyyy): ";
            cin >> input_year;
            if (isDigit(input_year, 4)) {
                year_of_birth = stoi(input_year);
                if ((year_of_birth >= 1925) && (year_of_birth <= 2025)) break;
            }
            cout << "\tInvalid year! Enter a value between 1925 and 2025: ";
        }
        while (true) {
            cout << "\tEnter passport series (4 digits): ";
            cin >> input_passp_series;
            if (isDigit(input_passp_series, 4)) {
                passp_series = stoi(input_passp_series);
                break;
            }
            cout << "\tInvalid series! Try again: ";
        }
        while (true) {
            cout << "\tEnter passport number (6 digits): ";
            cin >> input_passp_num;
            if (isDigit(input_passp_num, 6)) {
                passp_num = stoi(input_passp_num);
                break;
            }
            cout << "\tInvalid number! Try again: ";
        }
        cout << "\tEnter purpose of arrival: ";
        cin >> purp_of_arr;
        while (true) {
            cout << "\tEnter deregistration date (dd.mm.yyyy or 'r'): ";
            cin >> de_reg_date;
            if ((de_reg_date == "r") || (isValidDate(de_reg_date))) break;
            cout << "\tInvalid date! Try again: ";
        }
        if (!isChronological(reg_date, de_reg_date)) {
            cout << "\tError! Deregistration date is earlier, than registration! Try again!\n";
            system("pause");
            continue;
        }
        if (!isBornBeforeReg(year_of_birth, reg_date)) {
            cout << "\tError! Registration date is earlier than year of birth! Try again!\n";
            system("pause");
            continue;
        }

        base.push_back({region, reg_date, last_name, initials, nationality, year_of_birth, passp_series, passp_num, purp_of_arr, de_reg_date});

        if (find(regions.begin(), regions.end(), region) == regions.end()) {
            regions.push_back(region);
        }
        if (find(nationalities.begin(), nationalities.end(), nationality) == nationalities.end()) {
            nationalities.push_back(nationality);
        }

        ++i;
    }
    system("cls");
    cout << "New entries were succesfully added!\n";
    system("pause");
    fflush(stdin);
}
void menu::deleteEntries() {
    system("cls");
    fflush(stdin);
    int migrant_num;
    if (base.empty()) {
        cout << "Database is empty. Please, upload the data first (option 1 in menu)!\n";
        system("pause");
    } else {
        while (true) {
            while (true) {
                system("cls");
                cout << "The entry under which number do you want to delete? (Enter 0 to exit the deleting mode): ";
                if (!(cin >> migrant_num)) {
                    cout << "Wrong input! Please enter a valid number.\n";
                    cin.clear();
                    fflush(stdin);
                    system("pause");
                    continue;
                }
                if (migrant_num == 0) {
                    break;
                }
                if (migrant_num < 1 || migrant_num > base.size()) {
                    cout << "Invalid entry number! Try again.\n";
                    system("pause");
                    continue;
                }
                break;
            }
            if (migrant_num == 0) {
                break;
            }
            vector<Migrant>::iterator it = base.begin();
            for (int i = 1; i <= base.size(); ++i) {
                if (i == migrant_num) {
                    base.erase(it);
                    cout << "The entry was succesfully deleted.\n";
                    system("pause");
                    break;
                }
                it++;
            }
            if (base.empty()) {
                system("cls");
                cout << "Database is empty! Please, load data!" << endl;
                system("pause");
                break;
            }
        }
        system("cls");
        if (base.empty()) {
            cout << "Base is empty now.\n";
        } else if (migrant_num == 0) {
            cout << "Confirm exit.\n";
        } else {
            cout << "All necessary entries was successfully deleted.\n";
        }
        system("pause");
    }
    regions.clear();
    nationalities.clear();
    for (int i{}; i < base.size(); ++i) {
        if (find(regions.begin(), regions.end(), base[i].region) == regions.end()) {
            regions.push_back(base[i].region);
        }
        if (find(nationalities.begin(), nationalities.end(), base[i].nationality) == nationalities.end()) {
            nationalities.push_back(base[i].nationality);
        }
    }
    fflush(stdin);
}
void menu::editEntries() {
    system("cls");
    fflush(stdin);
    if (base.empty()) {
        cout << "Database is empty. Please, upload the data first (option 1 in menu)!\n";
        system("pause");
    } else {
        fflush(stdin);
        int number;
        char edit;
        while (true) {
            system("cls");
            cout << "==================== EDIT MENU ====================\n";
            cout << "Which entry would you like to change? (Enter 0 to exit from editing mode)\n";
            while (true) {
                cout << "Enter the number of the required entry: ";
                if (!(cin >> number)) {
                    cout << "Wrong input! Please enter a valid number.\n";
                    cin.clear();
                    fflush(stdin);
                    system("pause");
                    continue;
                }
                if (number == 0) {
                    system("pause");
                    return;
                }
                if (number < 1 || number > base.size()) {
                    cout << "Invalid entry number! Try again.\n";
                    system("pause");
                    continue;
                }
                break;
            }
            do {
                system("cls");
                cout << "Your entry:" << endl;
                cout << base[number-1] << "\n\n";
                cout << "Which data do you want to edit?\na - region, b - registration date, c - last name,"
                        "\nd - initials, e - nationality, f - year of birth,"
                        "\ng - passport series, h - passport number, i - purpose of arrival,"
                        "\nj - deregistration date (enter \"r\" if migrant is registered now), x - exit to entry selection\n";
                cout << "\nSelect an action: ";
                cin >> edit;
                fflush(stdin);
                switch (edit) {
                case 'x': {
                    system("pause");
                    break;
                }
                case 'a': {
                    system("cls");
                    cout << "Enter new region: ";
                    string input;
                    cin >> input;
                    capitalFirstLetter(input);
                    base[number-1].region = input;
                    break;
                }
                case 'b':{
                    system("cls");
                    cout << "Enter new registration date (format dd.mm.yyyy): ";
                    string regdate;
                    while (true) {
                        cin >> regdate;
                        if ((isValidDate(regdate)) && (isChronological(regdate, base[number-1].de_reg_date)) && (isBornBeforeReg(base[number-1].year_of_birth, regdate))) {
                            base[number-1].reg_date = regdate;
                            break;
                        }
                        cout << "Invalid date! Try again: ";
                    }
                    break;
                }
                case 'c': {
                    system("cls");
                    cout << "Enter new last name: ";
                    string input;
                    cin >> input;
                    capitalFirstLetter(input);
                    base[number-1].last_name = input;
                    break;
                }
                case 'd': {
                    system("cls");
                    cout <<"Enter new initials (format X.X.): ";
                    string input;
                    while (true) {
                        cin >> input;
                        if (isValidInitials(input)) {
                            base[number-1].initials = input;
                            break;
                        } else {
                            cout << "Invalid initials! Format must be X.X.! Try again: ";
                        }
                    }
                    break;
                }
                case 'e': {
                    system("cls");
                    cout << "Enter new nationality: ";
                    cin >> base[number-1].nationality;
                    transform(base[number-1].nationality.begin(), base[number-1].nationality.end(), base[number-1].nationality.begin(), ::tolower);
                    break;
                }
                case 'f': {
                    system("cls");
                    cout << "Enter new year of birth (yyyy): ";
                    string input;
                    while (true) {
                        cin >> input;
                        if (isDigit(input, 4)) {
                            int year = stoi(input);
                            if ((year >= 1925) && (year <= 2025) && isBornBeforeReg(year, base[number-1].reg_date)) {
                                base[number-1].year_of_birth = year;
                                break;
                            } else {
                                system("cls");
                                cout << "Invalid input. Please, enter the correct year: ";
                            }
                        } else {
                            system("cls");
                            cout << "Invalid input. Please, enter the correct year: ";
                        }
                    }
                    break;
                }
                case 'g': {
                    system("cls");
                    cout << "Enter new passport series (4 digits): ";
                    string input;
                    while (true) {
                        cin >> input;
                        if (isDigit(input, 4)) {
                            base[number-1].passp_series = stoi(input);
                            break;
                        } else {
                            cout << "Invalid input. Please, enter the correct series: ";
                        }
                    }
                    break;
                }
                case 'h': {
                    system("cls");
                    cout << "Enter new passport number (6 digits): ";
                    string input;
                    while (true) {
                        cin >> input;
                        if (isDigit(input, 6)) {
                            base[number-1].passp_num = stoi(input);
                            break;
                        } else {
                            cout << "Invalid input. Please, enter the correct number: ";
                        }
                    }
                    break;
                }
                case 'i': {
                    system("cls");
                    cout << "Enter new purpose of arrival: ";
                    cin >> base[number-1].purp_of_arr;
                    break;
                }
                case 'j': {
                    system("cls");
                    string deregdate;
                    cout << "Enter new deregistration date (format dd.mm.yyyy or 'r'): ";
                    while (true) {
                        cin >> deregdate;
                        if ((deregdate == "r") || ((isValidDate(deregdate)) && isChronological(base[number-1].reg_date, deregdate))) {
                            base[number-1].de_reg_date = deregdate;
                            break;
                        }
                        cout << "Invalid input. Please, enter the correct date: ";
                    }
                    break;
                }
                default: {
                    system("cls");
                    cout << "Unknown command!\n";
                    system("pause");
                    fflush(stdin);
                    break;
                }
                }
            } while (edit != 'x');
        }
        regions.clear();
        nationalities.clear();
        for (int i{}; i < base.size(); ++i) {
            transform(base[i].nationality.begin(), base[i].nationality.end(), base[i].nationality.begin(), [](unsigned char c) {return tolower(c);});
            if (find(regions.begin(), regions.end(), base[i].region) == regions.end()) {
                regions.push_back(base[i].region);
            }
            if (find(nationalities.begin(), nationalities.end(), base[i].nationality) == nationalities.end()) {
                nationalities.push_back(base[i].nationality);
            }
        }
    }
}
void menu::listOfRegistered() {
    system("cls");
    fflush(stdin);
    int number = 1;
    if (base.empty()) {
        cout << "Database is empty. Please, upload the data first (option 1 in menu)!\n";
        system("pause");
    } else {
        cout << "List of registered migrants:\n\n";
        for (const auto& migrant : base) {
            if (migrant.de_reg_date == "r") {
                cout << number++ << ". " << migrant;
            }
        }
    }
    system("pause");
    fflush(stdin);
}
void menu::searchMenu() {
    system("cls");
    fflush(stdin);
    if (base.empty()) {
        cout << "Database is empty. Please, upload the data first (option 1 in menu)!\n";
        system("pause");
        return;
    }
    auto toLower = [](string s) {
        transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {return tolower(c);});
        return s;
    };

    string inp_nationality;
    while (true) {
        system("cls");
        cout << "==================== SEARCH MENU ====================\n";
        cout << "List of nationalities: ";
        for (const auto& nationality : nationalities) {
            cout << nationality << "; ";
        }
        cout << "\n\nWhat nationality of migrants do you want to count (if you want to exit - enter \"0\")? Enter the nationality: ";
        cin >> inp_nationality;
        inp_nationality = toLower(inp_nationality);
        if (inp_nationality == "0") break;
        while (find(nationalities.begin(), nationalities.end(), inp_nationality) == nationalities.end()) {
            cout << "There're no migrants with this nationality! Repeat input, please: ";
            cin.clear();
            fflush(stdin);
            cin >> inp_nationality;
            if (inp_nationality == "0") {
                system("pause");
                return;
            }
            inp_nationality = toLower(inp_nationality);
        }
        vector<pair<string, int>> spisok;
        for (const auto& region : regions) {
            int count = 0;
            for (const auto& migrant : base) {
                if ((migrant.region == region) && (toLower(migrant.nationality) == inp_nationality) && (migrant.de_reg_date == "r")) {
                    count++;
                }
            }
            spisok.push_back({region, count});
        }
        sort(spisok.begin(), spisok.end(), [](const pair<string, int>& a, const pair<string, int>& b) {return a.second > b.second;});
        cout << "\nRegistered migrants of nationality [" << inp_nationality << "]:\n";
        for (int i{}; i < spisok.size(); ++i) {
            cout << (i+1) << ". " << spisok[i].first << ": " << spisok[i].second << "\n";
        }
        system("pause");
        fflush(stdin);
    }
    system("pause");
    fflush(stdin);
}
void menu::leavingMigrants() {
    system("cls");
    fflush(stdin);
    if (base.empty()) {
        cout << "Database is empty. Please, upload the data first (option 1 in menu)!\n";
        system("pause");
        return;
    }
    vector<string> lowerRegions;
    for (const auto &r : regions) {
        string region = r;
        transform(region.begin(), region.end(), region.begin(), [](unsigned char c) {return tolower(c);});
        lowerRegions.push_back(region);
    }
    string region_enter;
    while (true) {
        system("cls");
        cout << "List of regions:\n";
        for (size_t i{}; i < regions.size(); ++i) {
            cout << (i + 1) << ". " << regions[i] << "\n";
        }
        cout << "\nEnter region (enter '0' to exit): ";
        cin >> region_enter;
        transform(region_enter.begin(), region_enter.end(), region_enter.begin(), [](unsigned char c) {return tolower(c);});
        if (region_enter == "0") return;
        if (find(lowerRegions.begin(), lowerRegions.end(), region_enter) == lowerRegions.end()) {
            cout << "Region not found! Try again!\n";
            system("pause");
            continue;
        }
        string start_date, end_date;
        int start_int, end_int;
        while (true) {
            system("cls");
            cout << "Enter the first date (start of the period) in the format dd.mm.yyyy: ";
            cin >> start_date;
            cout << "Enter the second date (end of the period) in the format dd.mm.yyyy: ";
            cin >> end_date;
            if ((!isValidDate(start_date)) || (!isValidDate(end_date))) {
                cout << "Invalid date format! Try again, please.\n";
                system("pause");
                continue;
            }
            start_int = dateToInt(start_date);
            end_int = dateToInt(end_date);
            if (start_int > end_int) {
                cout << "Start date is later than end date! Try again, please.\n";
                system("pause");
                continue;
            }
            break;
        }

        int count = 0;
        for (const auto &migrant : base) {
            string region = migrant.region;
            transform(region.begin(), region.end(), region.begin(), [](unsigned char c) {return tolower(c);});
            if ((region != region_enter) || (migrant.de_reg_date == "r")) continue;
            if (!isValidDate(migrant.de_reg_date)) continue;
            int dereg_int = dateToInt(migrant.de_reg_date);
            if ((dereg_int >= start_int) && (dereg_int <= end_int)) {
                count++;
            }
        }
        system("cls");
        cout << "Number of migrants who left region " << region_enter << "\n";
        cout << "From " << start_date << " to " << end_date << ": " << count << "\n";
        system("pause");
    }
    system("pause");
}
void menu::saveToFile(const string &filename) {
    if (base.empty()) {
        cout << "Database is empty. You can't save nothing!\n";
        system("pause");
        return;
    }

    ofstream out(filename, ios::binary | ios::trunc);
    if (!out) {
        cout << "Failed to open file for saving!\n";
        system("pause");
        return;
    }

    for (const auto& migrant : base) {
        writeString(out, migrant.region);
        writeString(out, migrant.reg_date);
        writeString(out, migrant.last_name);
        writeString(out, migrant.initials);
        writeString(out, migrant.nationality);
        out.write(reinterpret_cast<const char*>(&migrant.year_of_birth), sizeof(int));
        out.write(reinterpret_cast<const char*>(&migrant.passp_series), sizeof(int));
        out.write(reinterpret_cast<const char*>(&migrant.passp_num), sizeof(int));
        writeString(out, migrant.purp_of_arr);
        writeString(out, migrant.de_reg_date);
    }
    out.close();
    encryptFile();
    cout << "Data saved succesfully!\n";
    system("pause");
}
int menu::run(const char *file_name)
{
    char choice;
    do {
        system("cls");
        fflush(stdin);
        cout << "==================== MENU ===================="
                "\n1 - Load data from file\n2 - Show database"
                "\n3 - Add entry to the list\n4 - Delete entry from the list"
                "\n5 - Edit entry\n6 - List of registered migrants"
                "\n7 - Search menu\n8 - Counter of deregistered migrants"
                "\n9 - Save data to file\n0 - Exit\n\nSelect an action: ";
        cin >> choice;
        switch (choice) {
        case '0': {
            system("cls");
            saveToFile(file_name);
            cout << "Confirm Exit (press Enter): " << endl;
            return 0;
        }
        case '1': {
            loadFromFile(file_name);
            break;
        }
        case '2': {
            showBase();
            break;
        }
        case '3': {
            addNewMigrant();
            break;
        }
        case '4': {
            deleteEntries();
            break;
        }
        case '5': {
            editEntries();
            break;
        }
        case '6': {
            listOfRegistered();
            break;
        }
        case '7': {
            searchMenu();
            break;
        }
        case '8': {
            leavingMigrants();
            break;
        }
        case '9': {
            system("cls");
            saveToFile(file_name);
            system("pause");
            break;
        }
        default: {
            system("cls");
            fflush(stdin);
            cout << "Unknown command! Try again!\n";
            system("pause");
            break;
        }
        }
    } while (choice != '0');
    return 0;
}
