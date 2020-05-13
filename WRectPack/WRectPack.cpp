/*
 * W - a tiny 2D game development library
 *
 * ====================
 *  TextureAtlas.cpp
 * ====================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "WRectPack.h"
#include <cfloat>


WRectPack::WRectPack(float w, float h) : nodes({{0.0, 0.0, w, h}})
{
	// hello
}

int first_node_in_vector_able_to_fit_rect(std::vector<WRectPack::Node> &v, float w, float h) {
	int i = 0;
	for (auto &n : v) {
		if (n.w >= w && n.h >= h)
			return i;
		++i;
	}
	return -1;
}

bool WRectPack::addRect(float w, float h, float &x, float &y) {
	int i = first_node_in_vector_able_to_fit_rect(nodes, w, h);
	if (i == -1)
		return false;
	
	auto &n = nodes[i];
	
	if (n.w == w && n.h == h) {
		printf("node fit perfectly, erasing\n");
		x = n.x, y = n.y;
		nodes.erase(nodes.begin() + i);
		return true;
	}
	
	x = n.x, y = n.y;
	
	if (n.h == h) {
		// Split n into chunks with widths w & (n.w - w)
		n.x += w;
		n.w -= w;
		return true;
	}
	
	if (n.w == w) {
		// Split n into chunks with heights h & (n.h - h)
		n.y += h;
		n.h -= h;
		return true;
	}
	
	// Split both horizontally and vertically
	bool split_horizontally = (w - n.w >= h - n.h);
		// Split horizontally if the horizontal excess in the node is larger than the
		// vertical excess (to give the largest possible new empty node sizes)
	
	if (split_horizontally) {
		n.x += w;
		n.w -= w;
		nodes.insert(nodes.begin() + i, { x, y + h, w, n.h - h });
	}
	else {
		n.y += h;
		n.h -= h;
		nodes.insert(nodes.begin() + i, { x + w, y, n.w - w, h });
	}
	return true;
}
