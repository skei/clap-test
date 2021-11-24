.h files are things that should probably be replaced with 'proper' implementations..

clap_*.hpp are clap-test specific


command line
(from last compilation, linux, codeblocks)

g++ -Wall -g -I../src -c /home/skei/Desktop/u-he/clap-test/src/main.cpp -o obj/debug/src/main.o

g++ -o ../bin/clap-test-debug obj/debug/src/main.o -lsndfile -lm -ldl

