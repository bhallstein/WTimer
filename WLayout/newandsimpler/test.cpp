#include <cassert>
#include "WLayout.h"
#include "Diatom-Storage.h"

#define p_assert(x) do {             \
		printf("%55s", #x);    \
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
	// std::vector<std::string> errors;
	
	// Load a layout
	Diatom d = diatomFromFile("test_layout.diatom");

	p_header("Loading WLayout");
	
	WLayout layout;
	auto errors = layout.loadFromDiatom(d);
	p_assert(errors.size() == 0);
	p_assert(layout.id_for_element("A") == 2);
	p_assert(layout.id_for_element("B") == 3);
	p_assert(layout.id_for_element("E") == 4);
	p_assert(layout.id_for_element("F") == 7);
	p_assert(layout.id_for_element("G") == -1);
	
	p_header("Layout computation");
	layout.compute({ 0, 0, 100, 100 });
	auto A = layout.get(2);
	p_assert(A.x == 0   && A.y == 0);
	p_assert(A.w == 100 && A.h == 100);
	
	auto B = layout.get(3);
	p_assert(B.x == 10 && B.y == 10);
	p_assert(B.w == 40 && B.h == 20);
	
	auto E = layout.get(4);
	p_assert(E.x == A.w-50 && E.y == B.y);
	p_assert(E.w == B.w    && E.h == B.h);
	
	auto C = layout.get(5);
	p_assert(C.x == 10 && C.y == 40);
	p_assert(C.w == 80 && C.h == 50);
	
	auto D = layout.get(6);
	p_assert(D.x == 72 && D.y == C.y+3);
	p_assert(D.w == 8  && D.h == C.h-6);
	
	auto F = layout.get(7);
	p_assert(F.x == 10 && F.y == C.y+6-C.w);
	p_assert(F.w == C.w-8  && F.h == C.h);
	
	
	// Elements: check for invalid keys and non-numeric properties
	p_header("Parse errors: rule name");
	d = diatomFromFile("test_layout_invalid.diatom");
	errors = layout.loadFromDiatom(d);
	p_assert(errors.size() == 1);
	p_assert(errors[0] == "Invalid positioning rule: 'bloop'");

	return 0;
}
