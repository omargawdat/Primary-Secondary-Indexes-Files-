#include <iostream>

using namespace std;

class Employee {
public:
    string name;
    string id;
    string position;
    string deptID;

    int get_size() const { return name.length() + id.length() + position.length() + deptID.length(); }

    friend istream &operator>>(istream &str, Employee &e) {
        str >> e.name >> e.id >> e.position >> e.deptID;
        cin.ignore();
        return str;
    }

    friend ostream &operator<<(ostream &str, Employee &e) {
        str << "Name: " << e.name << endl;
        str << "ID: " << e.id << endl;
        str << "Position: " << e.position << endl;
        str << "Department ID: " << e.deptID << endl;
        return str;
    }
};