/*
 * W - a tiny 2D game development library
 *
 * ==================
 *  TextureAtlas.h
 * ==================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __WRectPack
#define __WRectPack

#include <vector>

class WRectPack {
public:
	WRectPack(float w, float h);
	bool addRect(float w, float h, float &x, float &y);
	
	struct Node {
		float x, y, w, h;
	};
	
private:
	std::vector<Node> nodes;

};

#endif