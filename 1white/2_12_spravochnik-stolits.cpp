#include <iostream>
#include <map>

using namespace std;

using sprav = map<string, string>;


void change_capital(sprav& sprav) {
    string country, new_capital;
    cin >> country >> new_capital;

    if (sprav.count(country) == 0) {
        cout << "Introduce new country " << country << " with capital " << new_capital << endl;
    } else if (sprav[country] == new_capital) {
        cout << "Country " << country << " hasn't changed its capital" << endl;
    } else {
        cout << "Country " << country << " has changed its capital from " << sprav[country] << " to " << new_capital << endl;
    }
    sprav[country] = new_capital;
}

void rename(sprav& sprav) {
    string old_country_name, new_country_name;
    cin >> old_country_name >> new_country_name;

    if (old_country_name == new_country_name
        || sprav.count(old_country_name) == 0
        || sprav.count(new_country_name) == 1)
    {
        cout << "Incorrect rename, skip" << endl;
    } else {
        sprav[new_country_name] = sprav[old_country_name];
        sprav.erase(old_country_name);
        cout << "Country " << old_country_name << " with capital " << sprav[new_country_name] << " has been renamed to " << new_country_name << endl;
    }
}

void about(const sprav& sprav) {
    string requested_country;
    cin >> requested_country;
    
    if (sprav.count(requested_country) > 0) {
        cout << "Country " << requested_country << " has capital " << sprav.at(requested_country) << endl;
    } else {
        cout << "Country " << requested_country << " doesn't exist" << endl;
    }
}

void dump(const sprav& sprav) {
    if (sprav.size() == 0) {
        cout << "There are no countries in the world" << endl;
    } else {
        for (const auto& [country, capital] : sprav) {
            cout << country << "/" << capital << " ";
        }
        cout << endl;
    }
}

int main()
{
    sprav sprav;
    size_t q;    
    cin >> q;

    while (q > 0) {
        q--;
        string command;
        cin >> command;
        if (command == "CHANGE_CAPITAL") {
            change_capital(sprav);
        } else if (command == "RENAME") {
            rename(sprav);
        } else if (command == "ABOUT") {
            about(sprav);
        } else if (command == "DUMP") {
            dump(sprav);
        }
    }
    return 0;
}