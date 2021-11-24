g++ -Wall -g -I../src -c /DISKS/sda2/code/git/clap-test/src/main.cpp -o obj/debug/src/main.o

g++  -o ../bin/clap-test-debug obj/debug/src/main.o   -lsndfile -lm -ldl

