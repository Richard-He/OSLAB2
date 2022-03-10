main:
	g++ -std=c++0x -o scheduler main.cpp DES.cpp scheduler.cpp Random.cpp

clean:
	rm -rf scheduler