# this make file works only for linux you can look in the raylib github to see what is needed for the windows version 
all:
	g++ -std=c++20 -Wall -Wextra main.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

debug:
	g++  -std=c++20 -Wall -Wextra -g main.cpp -o db.out -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

run:
	./a.out
