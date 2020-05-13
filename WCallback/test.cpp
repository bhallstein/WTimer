#include "WCallback.h"
#include <cstdio>
#include "../_test.h"

class X {
public:
	
	int f(float x) {
		return 7 * x;
	}
		
};


int main() {
	X x;
	
	p_header("WCallback: construct & call");
	WCallback<int, float> *cb = make_wcallback(&X::f, &x);
	int result = cb->call(3.0);
	p_assert(result == 21);
	
	delete cb;
	return 0;
}