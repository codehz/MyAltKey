main: main.cpp res.o
	g++ -municode -mwindows -o main main.cpp res.o

res.o: res.rc
	windres res.rc res.o