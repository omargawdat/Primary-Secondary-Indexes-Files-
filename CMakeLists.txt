cmake_minimum_required(VERSION 3.23)
project(untitled7)

set(CMAKE_CXX_STANDARD 23)

add_executable(untitled7 main.cpp Employee/Employee.cpp Department/Department.cpp Employee/EmployeeDataHandler.cpp Department/DepartmentDataHandler.cpp Other/CONST.cpp Other/Indxes.cpp Other/query.cpp)
