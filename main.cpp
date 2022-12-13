#include <iostream>
#include "Employee/EmployeeDataHandler.cpp"
#include "Department/DepartmentDataHandler.cpp"

using namespace std;


int main() {
    DepartmentDataHandler departmentDataHandler;
    EmployeeDataHandler employeeDataHandler;


    char c;
    do {
        cout << "MENU: Pick number (1->9)" << endl;
        cout << "1. Add New Employee" << endl;
        cout << "2. Add New Department" << endl;
        cout << "3. Delete Employee (ID)" << endl;
        cout << "4. Delete Department(ID)" << endl;
        cout << "5. Print Employee (ID)" << endl;
        cout << "6. Print Employee (DepartID)" << endl;
        cout << "7. Print Department (ID)" << endl;
        cout << "8. Print Department (DepartID)" << endl;
        cout << "9. Write Query" << endl;
        cout << "press 'E' for Exit.." << endl;

        cout << "choice:";
        cin >> c;
        switch (c) {
            case '1': {
                Employee emp;
                cout << "Enter name id position deptID:" << endl;
                cin >> emp;
                employeeDataHandler.addEmployee(emp);
                break;
            }
            case '2': {
                Department department;
                cout << "Enter Department: name id manger" << endl;
                cin >> department;
                departmentDataHandler.addDepartment(department);
                break;
            }
            case '3': {
                string id;
                cout << "Employee ID:" << endl;
                cin >> id;
                employeeDataHandler.deleteEmployee(id);
                break;
            }
            case '4': {
                string id;
                cout << "Department ID:" << endl;
                cin >> id;
                departmentDataHandler.deleteDepartment(id);
                break;

            }
            case '5': {
                string id;
                cout << "Employee ID:" << endl;
                cin >> id;
                employeeDataHandler.printRecordById(id);
                break;
            }
            case '6': {
                string deptId;
                cout << "Department ID:" << endl;
                cin >> deptId;
                employeeDataHandler.printRecordByDept(deptId);
                break;

            }
            case '7': {
                string id;
                cout << "Department ID:" << endl;
                cin >> id;
                departmentDataHandler.printRecordById(id);
                break;
            }
            case '8': {
                string name;
                cout << "Department ID:" << endl;
                cin >> name;
                departmentDataHandler.printRecordByName(name);
                break;
            }
            case '9': {
                string arr[8];
                cout << "Enter Query:" << endl;
                for (int i = 0; i < 8; i++) {
                    cin >> arr[i];
                }

                if (arr[0] == "select" && arr[1] == "all" && arr[3] == "Employee" && arr[5] == "Dept_ID") {
                    employeeDataHandler.printRecordByDept(arr[7]);
                } else if (arr[0] == "select" && arr[1] == "all" && arr[3] == "Department" && arr[5] == "Dept_ID") {
                    departmentDataHandler.printRecordById(arr[7]);
                } else if (arr[0] == "select" && arr[1] == "Employee_Name" && arr[3] == "Employee" &&
                arr[5] == "Employee_ID") {
                    employeeDataHandler.printNameByID(arr[7]);
                }
            }
            default:
                break;
        }
        cout << "\n";

    } while (toupper(c) != 'E');
}
