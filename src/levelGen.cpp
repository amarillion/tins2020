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
	{ TELEPORT, { 0, 0, 'T', TELEPORT }}
};

int dx[4] = { 0, 1, 0, -1 };
int dy[4] = { -1, 0, 1, 0 };


/** short cut to check if a key exists in a given map */
//TODO: move to utility class
template<typename T, typename U>
bool hasKey(const T &aMap, const U &aKey) {
	return aMap.find(aKey) != aMap.end();
}

template<typename T>
T choice(const std::vector<T> &aContainer) {
	return aContainer[random(aContainer.size())];
}

// essential missing functionality
// see: https://stackoverflow.com/questions/6942273/how-to-get-a-random-element-from-a-c-container
template<typename T>
T pop(std::vector<T> &aContainer) {
	T response = aContainer[aContainer.size()-1];
	aContainer.pop_back();
	return response;
}

struct Node {
	int area;
	int doorArea;
	int x, y;
	map<Dir, Node*> links;
	map<Dir, bool> locks;

	char letter; // for teleporter edges...
	bool hasLink(Dir dir) {
		return hasKey(links, dir);
	}
	bool hasLock(Dir dir) {
		return hasKey(locks, dir);
	}
	int pStart = 0;
	bool hasBanana = false;
	bool hasKeycard = false;
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
				row[0] << "▒▒▒▒";
				row[1] << "▒▒▒▒";
				row[2] << "▒▒▒▒";
				row[3] << "▒▒▒▒";
			}
			else {
				auto &node = cell.nodes[0];
				
				char pstartChar = node.pStart == 0 ? ' ' : (node.pStart == 1 ? '1' : '2');
				const char *northDoor = (node.hasLink(N) ? (node.hasLock(N) ? "X" : " ") : "▒");
				const char *westDoor = (node.hasLink(W) ? (node.hasLock(W) ? "X" : " ") : "▒");
				

				row[0] << string_format("▒▒%s▒", northDoor);
				row[1] << string_format ("▒%02i ", node.doorArea);
				// row[1] << "▒   ";
				row[2] << string_format("%s %c ", westDoor, pstartChar);
				row[3] << string_format("▒%c%c%c", 
					node.hasBanana ? 'b' : ' ', 
					node.hasKeycard ? 'k' : ' ', 
					node.hasLink(TELEPORT) ? node.letter : ' '
				);
			}
		}
		result << row[0].str() << "▒\n" << row[1].str() << "▒\n" << row[2].str() << "▒\n" << row[3].str() << "▒\n";
	}

	for (size_t x = 0; x < map.getDimMX(); ++x) {
		result << "▒▒▒▒";
	}
	result << "▒\n";

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
vector<Edge> listEdges(Map2D<Cell> &lvl, Node *node) {
	vector<Edge> result;
	int x = node->x;
	int y = node->y;
	vector<Dir> dirs = { N, E, S, W };
	for (auto dir : dirs) {
		int nx = x + dx[dir];
		int ny = y + dy[dir];
		if (lvl.inBounds(nx, ny)) {
			auto &other = lvl.get(nx, ny);
			if (other.isEmpty()) continue;
			Node &otherNode = other.nodes[0];
			
			Edge e = { node, &otherNode, dir, '1' };
			result.push_back(e);
		}
	}

	return result;
}


vector<Node*> getAllNodes(Map2D<Cell> &lvl) {
	vector<Node*> result;
	for (size_t y = 0; y < lvl.getDimMY(); ++y) {
		for (size_t x = 0; x < lvl.getDimMY(); ++x) {
			if (lvl.get(x, y).isEmpty()) continue;
			result.push_back(&lvl.get(x, y).nodes[0]);
		}
	}
	return result;
}

vector<Node*> getDoorAreaNodes(Map2D<Cell> &lvl, int area) {
	vector<Node*> result;
	for (size_t y = 0; y < lvl.getDimMY(); ++y) {
		for (size_t x = 0; x < lvl.getDimMY(); ++x) {
			if (lvl.get(x, y).isEmpty()) continue;
			Node *n = &lvl.get(x, y).nodes[0];
			if (n->doorArea == area) {
				result.push_back(n);
			}
		}
	}
	return result;
}

const int NUM_BANANAS = 3;

void placeObjects(Map2D<Cell> &lvl, int area1, int area2) {
	auto allNodes = getAllNodes(lvl);

	auto area1Nodes = getDoorAreaNodes(lvl, area1);
	auto area2Nodes = getDoorAreaNodes(lvl, area2);

	if (area1Nodes.size() < area2Nodes.size()) {
		auto temp = area1Nodes;
		area1Nodes = area2Nodes;
		area2Nodes = temp;
	}

	random_shuffle(area1Nodes.begin(), area1Nodes.end());
	random_shuffle(area2Nodes.begin(), area2Nodes.end());
	random_shuffle(allNodes.begin(), allNodes.end());
	
	// area1 is the largest area.
	// put the players in there.
	auto n1 = pop(area1Nodes);
	n1->pStart = 1;
	auto n2 = pop(area1Nodes);
	n2->pStart = 2;

	// put the key in area1
	auto n3 = pop(area1Nodes);
	n3->hasKeycard = true;
	
	int bananaRemain = NUM_BANANAS;
	// put at least one banana behind the door
	auto n4 = pop(area2Nodes);
	n4->hasBanana = true;
	bananaRemain--;

	while (bananaRemain > 0) {
		auto b = pop(allNodes);
		if (!b->hasBanana) {
			b->hasBanana = true;
			bananaRemain--;
		}
	}

	// start over
	allNodes = getAllNodes(lvl);
	
}

void linkNodes(Node *src, Node *dest, Dir dir, bool reverse = true, bool doorLink = false) {
	assert(!hasKey(src->links, dir));
	src->links[dir] = dest;

	if (doorLink) {
		src->locks[dir] = true;
	}

	if (reverse) {
		linkNodes(dest, src, DIR[dir].reverse, false, doorLink);
	}
}

void convertArea(Map2D<Cell> &lvl, int srcArea, int destArea, bool convertDoors=true) {
	for (size_t y = 0; y < lvl.getDimMY(); ++y) {
		for (size_t x = 0; x < lvl.getDimMY(); ++x) {
			if (lvl.get(x, y).isEmpty()) continue;
			auto &node = lvl.get(x, y).nodes[0];
			if (node.area == srcArea) {
				node.area = destArea;
				if (convertDoors) {
					node.doorArea = destArea;
				}
			}
		}
	}
};

void kruskal(Map2D<Cell> &lvl) {
	vector<Edge> allEdges;
	
	int areaCounter = 0;

	// list all possible edges
	for (auto n : getAllNodes(lvl)) {
		n->area = areaCounter++;
		n->doorArea = n->area;
		for (auto &edge : listEdges(lvl, n)) {
			allEdges.push_back(edge);
		}
	}

	vector<Node*> teleporterNodes = getAllNodes(lvl);
	random_shuffle(teleporterNodes.begin(), teleporterNodes.end());
	char letter = 'A';
	while (teleporterNodes.size() >= 2) {
		Node *from = pop(teleporterNodes);
		Node *to = pop(teleporterNodes);
		allEdges.push_back(Edge{ from, to, TELEPORT, letter++});
	}

	// Kruskal's normally picks the lowest weights in a priority queue,
	// but for us the order really doesn't matter!
	random_shuffle(allEdges.begin(), allEdges.end());

	int countAreas = areaCounter;
	while (allEdges.size() > 0 && areaCounter > 1) {
		// pick a random edge
		// does it link two areas?
		Edge e = pop(allEdges);

		// if (countAreas == 2 && e.dir == TELEPORT) {
		// 	continue;
		// 	//TODO! there is a risk that there is nothing left...
		// }

		if (e.src->area == e.dest->area) {
			// this is now a candidate for creating a loop
		}
		else {
			cout << "Adding link: " << e.toString() << endl;
			// create an edge, update areas
			// until there is only one area
			
			if (areaCounter == 2 && e.dir != TELEPORT) {
				linkNodes(e.src, e.dest, e.dir, true, true);
				convertArea(lvl, e.dest->area, e.src->area, false);

				placeObjects(lvl, e.src->doorArea, e.dest->doorArea);
			}
			else {
				linkNodes(e.src, e.dest, e.dir);
				convertArea(lvl, e.dest->area, e.src->area);
			}
			if (e.dir == TELEPORT) {
				e.src->letter = e.letter;
				e.dest->letter = e.letter;
			}
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