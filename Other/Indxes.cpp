#pragma once

#include <cstring>
#include <iostream>

using namespace std;

struct PIndex {
    char id[13]{};
    int offset{};

    bool operator<(const PIndex &r) const {
        return stoi(id) < stoi(r.id);
    }

    friend ostream &operator<<(ostream &str, PIndex &PI) {
        str << "Offset: " << PI.offset << endl;
        str << "ID: " << PI.id << endl;
        return str;
    }
};


struct SIndex {
    char deptID[13]{};
    int primaryOffset{};

    bool operator<(const SIndex &r) const {
        return stoi(deptID) < stoi(r.deptID);
    }
};

struct LLIndex {
    char id[13]{};
    int nextPrimaryOffset{};

//    LLIndex(string id, int primaryOffset) {
//        strcpy(this->id, id.c_str());
//        this->nextPrimaryOffset = primaryOffset;
//    }
};

struct SDepartmentIndex {
    char Name[13]{};
    int primaryOffset{};

    bool operator<(const SDepartmentIndex &r) const {
        return strcmp(Name, r.Name) < 0;
    }
};