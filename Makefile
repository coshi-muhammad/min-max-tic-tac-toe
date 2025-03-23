# this make file works only for linux you can look in the raylib github to see what is needed for the windows version 
all:
	g++ main.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

debug:
	g++  -g main.cpp -o db.out -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

run:
	./a.out
