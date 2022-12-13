#pragma once

#include <fstream>
#include <vector>
#include "iostream"
#include "../Other/CONST.cpp"
#include "..\Other\Indxes.cpp"
#include "Employee.cpp"

using namespace std;


class EmployeeDataHandler {
    fstream wrData;
    fstream wrPrimary;
    fstream wrSecondary;
    fstream wrLinkedList;

public:
    EmployeeDataHandler() {
        wrData.open(R"(..\Data\Employee\Data.txt)", ios::in | ios::out);
        wrData.seekg(0, ios::end);
        if (wrData.tellg() == 0) {
            int availHeader = -1;
            wrData.write((char *) &availHeader, HEADER_SIZE);
        }
        wrData.close();
    }

    void addEmployee(const Employee &emp) {
        int offset = get_new_record_offset(emp);
        writeDataFile(emp, offset);
        addPrimaryIndex(emp.id, offset);
        addSecondaryLinkedIndex(emp.id, emp.deptID);
    }

    void deleteEmployee(string id) {
        vector<PIndex> v = readPrimaryFile(R"(..\Data\Employee\Primary.txt)", sizeof(PIndex));
        int offset = searchPrimaryBS(id, v);
        if (offset == -1) {
            cout << "Employee Not Found.." << endl;
            return;
        }

        wrData.open(R"(..\Data\Employee\Data.txt)", ios::in | ios::out);
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
        Employee emp = getEmployeeByOffset(offset);

        vector<SIndex> sv = readSecondaryFile();
        for (int i = 0; i < sv.size(); ++i) {
            if (sv[i].deptID == emp.deptID) {
                sv.erase(sv.begin() + i, sv.begin() + i);
                break;
            }
        }
        rewriteSecondary(sv);

        cout << "Employee has been deleted Successfully" << endl;
        wrData.close();
        // Avail
    }


    void printRecordById(string id) {
        vector<PIndex> v = readPrimaryFile(R"(..\Data\Employee\Primary.txt)", sizeof(PIndex));
        int offset = searchPrimaryBS(id, v);
        if (offset == -1) {
            cout << "Employee Not Found.." << endl;
            return;
        }

        Employee emp = getEmployeeByOffset(offset);
        cout << "Employee [" << id << "] information: " << endl;
        cout << "Name: " << emp.name << endl;
        cout << "Position: " << emp.position << endl;
        cout << "Department ID: " << emp.deptID << endl;

    }

    void printNameByID(string id) {
        vector<PIndex> v = readPrimaryFile(R"(..\Data\Employee\Primary.txt)", sizeof(PIndex));
        int offset = searchPrimaryBS(id, v);
        if (offset == -1) {
            cout << "Employee Not Found.." << endl;
            return;
        }

        Employee emp = getEmployeeByOffset(offset);
        cout << "Name: " << emp.name << endl;
    }

    void printRecordByDept(string id) {
        vector<SIndex> v = readSecondaryFile();
        int offset = searchSecondaryBS(id, v);
        if (offset == -1) {
            cout << "Employee Not Found.." << endl;
            return;
        }
        wrLinkedList.open(R"(..\Data\Employee\LinkedList.txt)", ios::in);

        int nextPrimary = offset;
        LLIndex temp;
        Employee emp;
        vector<SIndex> sv = readSecondaryFile();
        while (nextPrimary != -1) {
            wrLinkedList.seekg(nextPrimary, ios::beg);
            wrLinkedList.read((char *) &temp, sizeof(SIndex));

            int dataOffset = searchSecondaryBS(temp.id, sv);
            emp = getEmployeeByOffset(dataOffset);

            cout << "Employee [" << id << "] information: " << endl;
            cout << "Name: " << emp.name << endl;
            cout << "Position: " << emp.position << endl;
            cout << "Department ID: " << emp.deptID << endl;

            nextPrimary = temp.nextPrimaryOffset;
            break;
        }
        wrLinkedList.close();
    }


private:
    Employee getEmployeeByOffset(int offset) {
        wrData.open(R"(..\Data\Employee\Data.txt)", ios::in);
        Employee emp;

        wrData.seekg(offset + FLAG_SIZE + OFFSET_SIZE + sizeof(int), ios::beg);
        getline(wrData, emp.name, '|');
        getline(wrData, emp.id, '|');
        getline(wrData, emp.position, '|');
        getline(wrData, emp.deptID, '|');
        wrData.close();

        return emp;
    }

    int get_new_record_offset(Employee emp) {
        wrData.open(R"(..\Data\Employee\Data.txt)", ios::in | ios::out);

        int availHead, rec_off, record_size;
        wrData.read((char *) &availHead, HEADER_SIZE);
        int req_size = emp.get_size() + EMPLOYEE_DELIM_COUNT;

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

    void writeDataFile(const Employee &emp, int offset) {
        wrData.open(R"(..\Data\Employee\Data.txt)", ios::in | ios::out);

        wrData.seekp(offset, ios::beg);
        wrData.write((char *) &ACTIVE_FLAG, FLAG_SIZE);
        wrData.write((char *) &PREDELETED_INDEX, OFFSET_SIZE);
        int record_size = emp.get_size() + EMPLOYEE_DELIM_COUNT;
        wrData.write((char *) &record_size, sizeof(int));
        wrData << emp.name << DELIMITER << emp.id << DELIMITER << emp.position << DELIMITER << emp.deptID << DELIMITER;

        wrData.close();
    }

    void rewritePrimary(vector<PIndex> v) {
        // delete data
        wrPrimary.open(R"(..\Data\Employee\Primary.txt)", ios::out);
        wrPrimary.close();

        wrPrimary.open(R"(..\Data\Employee\Primary.txt)", ios::app);
        for (auto record: v)
            wrPrimary.write((char *) &record, sizeof(record));

        wrPrimary.close();
    }

    void rewriteSecondary(vector<SIndex> v) {
        // delete data
        wrSecondary.open(R"(..\Data\Employee\Secondary.txt)", ios::out);
        wrSecondary.close();

        wrSecondary.open(R"(..\Data\Employee\Secondary.txt)", ios::app);
        for (auto record: v)
            wrSecondary.write((char *) &record, sizeof(record));

        wrSecondary.close();
    }

    void addPrimaryIndex(string id, int offset) {
        vector<PIndex> records = readPrimaryFile(R"(..\Data\Employee\Primary.txt)", sizeof(PIndex));

        PIndex newIndex{};
        strcpy(newIndex.id, id.c_str());
        newIndex.offset = offset;

        records.push_back(newIndex);

        sort(records.begin(), records.end());

        rewritePrimary(records);
    }


    void addSecondaryLinkedIndex(string empID, string deptID) {

        wrLinkedList.open(R"(..\Data\Employee\LinkedList.txt)", ios::out | ios::in);

        vector<SIndex> records = readSecondaryFile();

        // write into linked list
        wrLinkedList.seekg(0, ios::end);
        int LLnewOffset = wrLinkedList.tellg();

        LLIndex l;
        strcpy(l.id, empID.c_str());
        l.nextPrimaryOffset = -1;

        wrLinkedList.write((char *) &l, sizeof(SIndex));

        // Adjust files
        int primaryOffset = searchSecondaryBS(deptID, records);
        if (primaryOffset == -1) {
//                  point from secondary into record
            SIndex newIndex{};
            strcpy(newIndex.deptID, deptID.c_str());
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
                wrLinkedList.read((char *) &temp, sizeof(SIndex));

                nextPrimary = temp.nextPrimaryOffset;
            }

            wrLinkedList.seekg(reqOffset, ios::beg);

            // edit the tail of the record
            LLIndex newTemp;
            strcpy(l.id, empID.c_str());
            l.nextPrimaryOffset = LLnewOffset;
            wrLinkedList.write((char *) &newTemp, sizeof(SIndex));
        }

        if (wrLinkedList.is_open())
            wrLinkedList.close();
    }


    vector<PIndex> readPrimaryFile(const string &address, int SIZE) {
        wrPrimary.open(address, ios::in);

        vector<PIndex> v;
        PIndex temp;
        wrPrimary.seekg(0, ios::end);
        int records_count = wrPrimary.tellg() / SIZE;
        wrPrimary.seekg(0, ios::beg);
        for (int i = 0; i < records_count; i++) {
            wrPrimary.read((char *) &temp, SIZE);
            v.push_back(temp);
        }

        wrPrimary.close();
        return v;
    }

    vector<SIndex> readSecondaryFile() {
        wrSecondary.open(R"(..\Data\Employee\Secondary.txt)", ios::in);

        vector<SIndex> v;
        SIndex temp;
        wrSecondary.seekg(0, ios::end);
        int records_count = wrSecondary.tellg() / sizeof(SIndex);
        wrSecondary.seekg(0, ios::beg);
        for (int i = 0; i < records_count; i++) {
            wrSecondary.read((char *) &temp, sizeof(SIndex));
            v.push_back(temp);
        }

        wrSecondary.close();
        return v;
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

    int searchSecondaryBS(string ID, vector<SIndex> v) {
        if (v.size() == 0)
            return -1;

        int low = 0, mid, high = v.size();
        while (low <= high) {
            mid = (low + high) / 2;
            if (ID < v[mid].deptID)
                high = mid - 1;
            else if (ID > v[mid].deptID)
                low = mid + 1;
            else {
                return v[mid].primaryOffset;
            }
        }
        return -1;
    }
};