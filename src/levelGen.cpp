#include "levelGen.h"
#include <iostream>
#include "map2d.h"
#include <sstream>
#include <map>
#include "util.h"

using namespace std;

class Node {
	int area;
	int doorArea;
	map<int, Node> links;
};

class Cell {
	vector<Node> nodes;
	int roomType;
	int objects;

public:
	bool isEmpty() const {
		return nodes.empty();
	}

	void addNode() {
		nodes.push_back(Node());
	}
};

string toString(const Map2D<Cell> &map) {
	stringstream result;
	
	for (size_t y = 0; y < map.getDimMY(); ++y) {
		stringstream row[4];

		for (size_t x = 0; x < map.getDimMX(); ++x) {
			auto cell = map.get(x, y);
			if (cell.isEmpty()) {
				row[0] << "    ";
				row[1] << "    ";
				row[2] << "    ";
				row[3] << "    ";
			}
			else {
				row[0] << "    ";
				row[1] << " ...";
				row[2] << " ...";
				row[3] << " ...";
			}
		}
		result << row[0].str() << endl << row[1].str() << endl << row[2].str() << endl << row[3].str() << endl;
	}
	return result.str();
}

/**
 * Add nodes in a few places, up to a certain num
 */
void initNodes(Map2D<Cell> &lvl, int num) {

	int dx[4] = { 0, -1, 0, 1 };
	int dy[4] = { 1, 0, -1, 0 };

	int x = lvl.getDimMY() / 2;
	int y = lvl.getDimMX() / 2;

	// random walk
	for (int i = 0; i < num; ++i) {
		while (!lvl.get(x, y).isEmpty()) {
			int dir = random(4);
			x += dx[dir];
			y += dy[dir];
		}
		lvl.get(x, y).addNode();
	}
}


void createLevel() {

	auto level = Map2D<Cell>(8, 8);
	initNodes(level, 10);
	cout << endl << toString(level) << endl;
}