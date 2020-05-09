#include "levelGen.h"
#include <iostream>
#include "map2d.h"
#include <sstream>
#include <map>
#include "util.h"
#include <algorithm>    // std::random_shuffle
#include "strutil.h"

using namespace std;

enum Dir { N, E, S, W, TELEPORT };
// map<Dir, int> DX = { { N, 0 }, { E, 1 }, { S, 0 },  { W, -1 } };

struct DirInfo {
	int dx;
	int dy;
	char shortName;
	Dir reverse;
};

map<Dir, DirInfo> DIR = {
	{ N, { 0, -1, 'N', S } },
	{ E, { 1,  0, 'E', W } },
	{ S, { 0,  1, 'S', N } },
	{ W, { -1, 0, 'W', E } },
};

int dx[4] = { 0, 1, 0, -1 };
int dy[4] = { -1, 0, 1, 0 };


/** short cut to check if a key exists in a given map */
//TODO: move to utility class
template<typename T, typename U>
bool hasKey(const T &aMap, const U &aKey) {
	return aMap.find(aKey) != aMap.end();
}

struct Node {
	int area;
	int x, y;
	map<Dir, Node*> links;
	bool hasLink(Dir dir) {
		return hasKey(links, dir);
	}
};

class Edge {
public:
	Node *src;
	Node *dest;
	Dir dir;
	char letter; // for teleporter edges...

	string toString() {
		return string_format("Edge %c from (%i, %i) to (%i, %i)", 
			DIR[dir].shortName, 
			src->x, src->y,
			dest->x, dest->y);
	}
};

class Cell {
public:
	int x;
	int y;
	vector<Node> nodes;
	int roomType;
	int objects;

	bool isEmpty() const {
		return nodes.empty();
	}

	void addNode() {
		Node n;
		n.x = x;
		n.y = y;
		nodes.push_back(n);
	}

	void getEdges() {
		vector<Edge> result;
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
				auto &node = cell.nodes[0];
				row[0] << (node.hasLink(N) ? "  | ": "    ");
				row[1] << string_format (" %02i.", node.area);
				row[2] << (node.hasLink(W) ? "-..." : " ...");
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

// list all potential edges from a given cell
vector<Edge> listEdges(Map2D<Cell> &lvl, int x, int y) {
	vector<Edge> result;
	auto &cell = lvl.get(x, y);
	if (cell.isEmpty()) return result;
	
	// only first node is connected normally. Other nodes are tunnels etc...
	Node &node = cell.nodes[0];

	vector<Dir> dirs = { N, E, S, W };
	for (auto dir : dirs) {
		int nx = x + dx[dir];
		int ny = y + dy[dir];
		if (lvl.inBounds(nx, ny)) {
			auto &other = lvl.get(nx, ny);
			if (other.isEmpty()) continue;
			Node &otherNode = other.nodes[0];
			
			Edge e = { &node, &otherNode, dir, '1' };
			result.push_back(e);
		}
	}

	return result;
}


void linkNodes(Node *src, Node *dest, Dir dir, bool reverse = true) {
	assert(!hasKey(src->links, dir));
	src->links[dir] = dest;

	if (reverse) {
		linkNodes(dest, src, DIR[dir].reverse, false);
	}
}

void convertArea(Map2D<Cell> &lvl, int srcArea, int destArea) {
	for (size_t y = 0; y < lvl.getDimMY(); ++y) {
		for (size_t x = 0; x < lvl.getDimMY(); ++x) {
			if (lvl.get(x, y).isEmpty()) continue;
			auto &node = lvl.get(x, y).nodes[0];
			if (node.area == srcArea) {
				node.area = destArea;
			}
		}
	}
};

void kruskal(Map2D<Cell> &lvl) {
	vector<Edge> allEdges;
	
	int areaCounter = 0;

	// list all possible edges
	// TODO: Add optional teleportation edges here
	for (size_t y = 0; y < lvl.getDimMY(); ++y) {
		for (size_t x = 0; x < lvl.getDimMY(); ++x) {
			if (lvl.get(x, y).isEmpty()) continue;

			lvl.get(x,y).nodes[0].area = areaCounter++;
			for (auto &edge : listEdges(lvl, x, y)) {
				allEdges.push_back(edge);
			}
		}
	}

	
	random_shuffle(allEdges.begin(), allEdges.end());

	int countAreas = areaCounter;
	while (allEdges.size() > 0 && countAreas > 1) {
		// pick a random edge
		// does it link two areas?
		Edge e = allEdges[allEdges.size()-1];
		allEdges.pop_back(); // remove last element

		if (e.src->area == e.dest->area) {
			// this is now a candidate for creating a loop
		}
		else {
			cout << "Adding link: " << e.toString() << endl;
			// create an edge, update areas
			// until there is only one area
			linkNodes(e.src, e.dest, e.dir);
			convertArea(lvl, e.dest->area, e.src->area);
			areaCounter--;

			// print intermediate step for debugging
			cout << endl << toString(lvl) << endl;
		}
	}
}

void initCells(Map2D<Cell> &lvl) {
	for (size_t y = 0; y < lvl.getDimMY(); ++y) {
		for (size_t x = 0; x < lvl.getDimMY(); ++x) {
			auto &cell = lvl.get(x, y);
			cell.x = x;
			cell.y = y;
		}
	}
}

void createLevel() {

	auto level = Map2D<Cell>(8, 8);
	initCells(level);
	
	// initialize a bunch of adjacent nodes
	initNodes(level, 10);

	kruskal(level);

	cout << endl << toString(level) << endl;
}