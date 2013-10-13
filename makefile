all:
	g++ globals.cpp node.cpp point.cpp sokoban.cpp -o sokoban -std=c++11 -stdlib=libc++