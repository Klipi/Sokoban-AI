all:
	g++ globals.cpp node.cpp point.cpp sokoban.cpp -ggdb -o sokoban -std=c++11 -stdlib=libc++