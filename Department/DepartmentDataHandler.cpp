#pragma once

#include <fstream>
#include <vector>
#include "iostream"
#include "../Other/CONST.cpp"
#include "..\Other\Indxes.cpp"
#include "Department.cpp"

using namespace std;


class DepartmentDataHandler {
    fstream wrData;
    fstream wrPrimary;
    fstream wrSecondary;
    fstream wrLinkedList;

public:
    DepartmentDataHandler() {
        wrData.open(R"(..\Data\Department\Data.txt)", ios::in | ios::out);
        wrData.seekg(0, ios::end);
        if (wrData.tellg() == 0) {
            int availHeader = -1;
            wrData.write((char *) &availHeader, HEADER_SIZE);
        }
        wrData.close();
    }

    void addDepartment(const Department &depart) {
        int offset = get_new_record_offset(depart);
        cout << offset << endl;
        writeDataFile(depart, offset);
        addPrimaryIndex(depart.id, offset);
        addSecondaryLinkedIndex(depart.id, depart.name);
    }


    void printRecordById(string id) {
        int offset = getRecordOffset(id);
        if (offset == -1) {
            cout << "Department Not Found.." << endl;
            return;
        }

        Department depart = getDepartmentByOffset(offset);

        cout << "Department [" << id << "] information: " << endl;
        cout << "Name: " << depart.name << endl;
        cout << "Mange: " << depart.manger << endl;

        wrData.close();
    }

    void printRecordByName(string id) {
        vector<SDepartmentIndex> v = readSecondaryFile();
        int offset = searchSecondaryBS(id, v);
        if (offset == -1) {
            cout << "Department Not Found.." << endl;
            return;
        }
        wrLinkedList.open(R"(..\Data\Department\LinkedList.txt)", ios::in);

        int nextPrimary = offset;
        LLIndex temp;
        Department dep;
        vector<SDepartmentIndex> sv = readSecondaryFile();
        while (nextPrimary != -1) {
            wrLinkedList.seekg(nextPrimary, ios::beg);
            wrLinkedList.read((char *) &temp, sizeof(SDepartmentIndex));

            int dataOffset = searchSecondaryBS(temp.id, sv);
            dep = getDepartmentByOffset(dataOffset);

            cout << "Department [" << dep.id << "] information: " << endl;
            cout << "Name: " << dep.name << endl;
            cout << "Manger: " << dep.manger << endl;

            nextPrimary = temp.nextPrimaryOffset;
            break;
        }
        wrLinkedList.close();
    }

    void deleteDepartment(string id) {
        vector<PIndex> v = readPrimaryFile();
        int offset = searchPrimaryBS(id, v);
        if (offset == -1) {
            cout << "Department Not Found.." << endl;
            return;
        }

        wrData.open(R"(..\Data\Department\Data.txt)", ios::in | ios::out);
        wrData.seekg(offset);
        wrData.put(DELETE_FLAG);

        // update primary
        for (int i = 0; i < v.size(); ++i) {
            if (v[i].id == id) {
                v.erase(v.begin() + i, v.begin() + i + 1);
                break;
            }
        }

        rewritePrimary(v);

        // update secondary
        Department dep = getDepartmentByOffset(offset);

        vector<SDepartmentIndex> sv = readSecondaryFile();
        for (int i = 0; i < sv.size(); ++i) {
            if (sv[i].Name == dep.name) {
                sv.erase(sv.begin() + i, sv.begin() + i);
                break;
            }
        }
        rewriteSecondary(sv);

        cout << "Department has been deleted Successfully ✅" << endl;
        wrData.close();
        // Avail
    }


private:
    Department getDepartmentByOffset(int offset) {
        wrData.open(R"(..\Data\Department\Data.txt)", ios::in);
        Department dep;

        wrData.seekg(offset + FLAG_SIZE + OFFSET_SIZE + sizeof(int), ios::beg);
        getline(wrData, dep.name, '|');
        getline(wrData, dep.id, '|');
        getline(wrData, dep.manger, '|');
        wrData.close();

        return dep;
    }

    int get_new_record_offset(Department depart) {
        wrData.open(R"(..\Data\Department\Data.txt)", ios::in | ios::out);

        int availHead, rec_off, record_size;
        wrData.read((char *) &availHead, HEADER_SIZE);
        int req_size = depart.get_size() + DEPARTMENT_DELIM_COUNT;

        rec_off = availHead;
        while (rec_off != -1) {
            wrData.seekg(rec_off + FLAG_SIZE + OFFSET_SIZE, ios::beg);
            wrData.read((char *) &record_size, LENGTH_INDICATOR_SIZE);
            if (record_size >= req_size) {
                wrData.close();
                return rec_off;
            }
            wrData.seekg(rec_off + FLAG_SIZE, ios::beg);
            wrData.read((char *) &rec_off, OFFSET_SIZE);
        }
        wrData.seekg(0, ios::end);
        int offset = wrData.tellg();
        wrData.close();
        return offset;
    }

    void writeDataFile(const Department &department, int offset) {
        wrData.open(R"(..\Data\Department\Data.txt)", ios::in | ios::out);

        wrData.seekp(offset, ios::beg);
        wrData.write((char *) &ACTIVE_FLAG, FLAG_SIZE);
        wrData.write((char *) &PREDELETED_INDEX, OFFSET_SIZE);
        int record_size = department.get_size() + DEPARTMENT_DELIM_COUNT;
        wrData.write((char *) &record_size, sizeof(int));
        wrData << department.name << DELIMITER << department.id << DELIMITER << department.manger << DELIMITER;

        wrData.close();
    }

    void rewritePrimary(vector<PIndex> v) {
        wrPrimary.open(R"(..\Data\Department\Primary.txt)", ios::out);
        wrPrimary.close();
        wrPrimary.open(R"(..\Data\Department\Primary.txt)", ios::app);
        for (auto record: v)
            wrPrimary.write((char *) &record, sizeof(record));

        wrPrimary.close();
    }

    void addPrimaryIndex(string id, int offset) {
        vector<PIndex> records = readPrimaryFile();

        PIndex newIndex{};
        strcpy(newIndex.id, id.c_str());
        newIndex.offset = offset;
        records.push_back(newIndex);
        sort(records.begin(), records.end());

        rewritePrimary(records);
    }

    vector<PIndex> readPrimaryFile() {
        wrPrimary.open(R"(..\Data\Department\Primary.txt)", ios::in);

        vector<PIndex> v;
        PIndex temp;
        wrPrimary.seekg(0, ios::end);
        int records_count = wrPrimary.tellg() / sizeof(PIndex);
        wrPrimary.seekg(0, ios::beg);
        for (int i = 0; i < records_count; i++) {
            wrPrimary.read((char *) &temp, sizeof(PIndex));
            v.push_back(temp);
        }

        wrPrimary.close();
        return v;
    }

    int getRecordOffset(string ID) {
        vector<PIndex> v = readPrimaryFile();
        int low = 0, mid, high = v.size();
        while (low <= high) {
            mid = (low + high) / 2;
            if (ID < v[mid].id)
                high = mid - 1;
            else if (ID > v[mid].id)
                low = mid + 1;
            else {
                return v[mid].offset;
            }
        }
        return -1;
    }


    vector<SDepartmentIndex> readSecondaryFile() {
        wrSecondary.open(R"(..\Data\Department\Secondary.txt)", ios::in);

        vector<SDepartmentIndex> v;
        SDepartmentIndex temp;
        wrSecondary.seekg(0, ios::end);
        int records_count = wrSecondary.tellg() / sizeof(SDepartmentIndex);
        wrSecondary.seekg(0, ios::beg);
        for (int i = 0; i < records_count; i++) {
            wrSecondary.read((char *) &temp, sizeof(SDepartmentIndex));
            v.push_back(temp);
        }

        wrSecondary.close();
        return v;
    }

    void addSecondaryLinkedIndex(string depID, string deptName) {

        wrLinkedList.open(R"(..\Data\Department\LinkedList.txt)", ios::out | ios::in);

        vector<SDepartmentIndex> records = readSecondaryFile();

        // write into linked list
        wrLinkedList.seekg(0, ios::end);
        int LLnewOffset = wrLinkedList.tellg();

        LLIndex l;
        strcpy(l.id, depID.c_str());
        l.nextPrimaryOffset = -1;

        wrLinkedList.write((char *) &l, sizeof(SDepartmentIndex));

        // Adjust files
        int primaryOffset = searchSecondaryBS(deptName, records);
        if (primaryOffset == -1) {
//                  point from secondary into record
            SDepartmentIndex newIndex{};
            strcpy(newIndex.Name, deptName.c_str());
            newIndex.primaryOffset = LLnewOffset;

            // add record and sort file
            records.push_back(newIndex);
            sort(records.begin(), records.end());
            rewriteSecondary(records);
        } else {
            // reach the tail of the primaries
            int nextPrimary = primaryOffset;

            LLIndex temp;
            int reqOffset;
            while (nextPrimary != -1) {
                wrLinkedList.seekg(nextPrimary, ios::beg);
                reqOffset = wrLinkedList.tellg();
                wrLinkedList.read((char *) &temp, sizeof(SDepartmentIndex));

                nextPrimary = temp.nextPrimaryOffset;
            }

            wrLinkedList.seekg(reqOffset, ios::beg);

            // edit the tail of the record
            LLIndex newTemp;
            strcpy(l.id, depID.c_str());
            l.nextPrimaryOffset = LLnewOffset;
            wrLinkedList.write((char *) &newTemp, sizeof(SDepartmentIndex));
        }

        if (wrLinkedList.is_open())
            wrLinkedList.close();
    }

    void rewriteSecondary(vector<SDepartmentIndex> v) {
        // delete data
        wrSecondary.open(R"(..\Data\Department\Secondary.txt)", ios::out);
        wrSecondary.close();

        wrSecondary.open(R"(..\Data\Department\Secondary.txt)", ios::app);
        for (auto record: v)
            wrSecondary.write((char *) &record, sizeof(record));

        wrSecondary.close();
    }

    int searchPrimaryBS(string ID, vector<PIndex> v) {
        if (v.size() == 0)
            return -1;

        int low = 0, mid, high = v.size();
        while (low <= high) {
            mid = (low + high) / 2;
            if (ID < v[mid].id)
                high = mid - 1;
            else if (ID > v[mid].id)
                low = mid + 1;
            else {
                return v[mid].offset;
            }
        }
        return -1;
    }

    int searchSecondaryBS(string name, vector<SDepartmentIndex> v) {
        if (v.size() == 0)
            return -1;

        int low = 0, mid, high = v.size();
        while (low <= high) {
            mid = (low + high) / 2;
            if (name < v[mid].Name)
                high = mid - 1;
            else if (name > v[mid].Name)
                low = mid + 1;
            else {
                return v[mid].primaryOffset;
            }
        }
        return -1;
    }
};