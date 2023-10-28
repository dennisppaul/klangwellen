#! /bin/zsh

PROJECT=klangwellen

clang++ -std=c++11 $PROJECT.cpp ../src/*.cpp -I../include/ -o $PROJECT
./$PROJECT
