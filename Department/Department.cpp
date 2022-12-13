#include <iostream>

using namespace std;

class Department {
public:
    string id;
    string name;
    string manger;

    int get_size() const { return name.length() + id.length() + manger.length(); }

    friend istream &operator>>(istream &str, Department &d) {
        str >> d.name >> d.id >> d.manger;
        cin.ignore();
    }

    friend ostream &operator<<(ostream &str, Department &d) {
        str << "Name: " << d.name << endl;
        str << "ID: " << d.id << endl;
        str << "Manger: " << d.manger << endl;
        return str;
    }
};
