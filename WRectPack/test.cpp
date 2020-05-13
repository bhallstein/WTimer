#include <cstdio>
#include <cstdlib>
#include "WRectPack.h"
#include "EasyBMP.h"


void test(WRectPack &p, float w, float h) {
	float x, y;
	bool res = p.addRect(w, h, x, y);
	
	printf("%s\n", res ? "yes" : "no");
	if (res)
		printf("x:%.1f y:%.1f\n", x, y);
}

struct R {
	float x, y, w, h;
	int r, g, b;
};

bool rects_overlap(R r, R s) {
	// Two rects do not overlap if one is to be found entirely to the left, right, top, or bottom of the other
	
	bool s_right_of_r = (s.x >= r.x + r.w);
	bool s_below_r    = (s.y >= r.y + r.h);
	
	bool s_left_of_r = (s.x + s.w <= r.x);
	bool s_above_r   = (s.y + s.h <= r.y);

	
	return !(s_right_of_r || s_left_of_r || s_above_r || s_below_r);
}

void printRect(R r) {
	printf("  x:%.1f - %.1f  y:%.1f - %.1f\n", r.x, r.x + r.w, r.y, r.y + r.h);
}

void genImage(std::vector<R> &v, char *filename) {
	BMP img;
	img.SetSize(1000,1000);
	img.SetBitDepth(32);
	for (int i=0; i < 1000; ++i)
		for (int j=0; j < 1000; ++j) {
			auto px = img(i,j);
			px->Red = px->Green = px->Blue = px->Alpha = 255;
		}

	for (auto r : v) {
		for (int i=r.x; i < r.x + r.w; ++i) {
			for (int j=r.y; j < r.y + r.h; ++j) {
				img(i,j)->Red = r.r;
				img(i,j)->Green = r.g;
				img(i,j)->Blue = r.b;
				img(i,j)->Alpha = 255;
			}
		}
	}
	
	img.WriteToFile(filename);
}

int main() {
	
	WRectPack p(1000, 1000);
	
	std::vector<R> v;
	
	float max_rect_size = 40.0;
	
	for (int i=0; i < 10000; ++i) {
		R r = {
			0.0,
			0.0,
			rand() / float(RAND_MAX) * max_rect_size/2 + max_rect_size/2,
			rand() / float(RAND_MAX) * max_rect_size/2 + max_rect_size/2,
			
			(int) (rand() / float(RAND_MAX) * 255),
			(int) (rand() / float(RAND_MAX) * 255),
			(int) (rand() / float(RAND_MAX) * 255)
		};
		
		bool result = p.addRect(r.w, r.h, r.x, r.y);
		if (!result) continue;
		
		v.push_back(r);
		
		// char fn[200];
		// sprintf(fn, "i/i%d.bmp", i);
		// genImage(v, fn);
	}
	
	char fn[] = "_o.bmp";
	genImage(v, fn);
	
	printf("inserted successfully: %lu\n", v.size());

	return 0;
}