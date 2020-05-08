#ifndef __TWIST5_TILEMAP_H__
#define __TWIST5_TILEMAP_H__

#ifdef USE_TEGEL

#include "json.h"
#include <string>

struct TEG_MAP;
struct TEG_TILELIST;

class Tilemap {
public:
	JsonNode rawData;
	TEG_MAP *map;

	~Tilemap();
};

Tilemap *loadTilemap (const std::string &filename, TEG_TILELIST *tiles);

#endif

#endif
