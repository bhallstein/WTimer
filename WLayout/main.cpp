#include "WLayout.h"
#include "Diatom-Storage.h"
#include "WTimer.h"

#include <cassert>

#define p_assert(x) do {             \
		printf("TEST: %35s", #x);    \
		assert(x);                   \
		printf(" - PASS :)\n");      \
	} while (false)
#define p_header(s) do {                                  \
		for (int i=0; s[i] != '\0'; ++i) printf("*");     \
		printf("********\n");                             \
		printf("**  %s  **\n", s);                        \
		for (int i=0; s[i] != '\0'; ++i) printf("*");     \
		printf("********\n");                             \
	} while (false)


int main() {
	WTimer timer;
	
	// Load a layout
	Diatom d = diatomFromString(
		"layout: \n"
		"  intrinsics: \n"
		"    A:\n"
		"      w: 20\n"
		"      h: 20\n"
		"      x: 0.1\n"
		"      y: 0.75\n"
		"  rules:\n"
		"    B.x: \"A.x + 20\"\n"
		"    B.y: \"A.y + 40\"\n"
		"    B.w: \"A.w / 2 + A.x * 100\"\n"
		"    B.h: \"A.h * 2\""
	);
	p_assert(d.isTable());
	p_assert(d["layout"]["intrinsics"].isTable());
	p_assert(d["layout"]["rules"].isTable());
	
	// First job: deserialize the layout into:
	// - list of elements
	// - list of intrinsics
	Layout layout = Layout::from_diatom(d["layout"]);
	
	// for (auto &e : layout.elements) {
	// 	printf("%s:\n", e.first.c_str());
	// 	printf("  %.2f\n  %.2f\n  %.2f\n  %.2f\n", e.second.x, e.second.y, e.second.w, e.second.h);
	// }
	
	timer.reset();
	auto results = layout.compute();
	auto micros = timer.getMicroseconds();
	
	printf("Layout results:\n");
	for (auto i : results) {
		auto el_name = i.first;
		auto el = i.second;
		
		printf("%s: ", el_name.c_str());
		printf("%.2f %.2f %.2f %.2f\n", el.x, el.y, el.w, el.h);
	}
	
	printf("Took %lu µs\n", micros);
	
	return 0;
}