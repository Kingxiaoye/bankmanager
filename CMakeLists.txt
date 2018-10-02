# cmake_minimum_required(VERSION <specify CMake version here>)
project(bankmanager C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(bankmanager
        BankManagement.c
        user.c
        user.h)
