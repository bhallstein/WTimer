#include "Diatom.h"
#include <vector>
#include <map>

#include "Arithmetic/Arithmetic.h"

#define NOT_SET -54321


struct Element {
	Element() : x(NOT_SET), y(NOT_SET), w(NOT_SET), h(NOT_SET) {  }
	float x, y, w, h;
};

struct Rule {
	struct Target {
		std::string el_name;
		enum Property { X, Y, W, H } property;
	} target;
	Ar::ArithmeticExpression expression;
};

struct Layout {
	std::map<std::string, Element> elements;
	std::vector<Rule> rules;
	
	std::map<std::string, Element> compute();
	
	Layout(Diatom &);
	// When creating a Layout:
	//  - create a set of Elements
	//  - for each rule, create an ArithExpression
	//  - convert each expressions to a memory layout consisting of:
	//    - a set of floats (both expression elements & function results)
	//    - function pointers for operations
	//    - float pointers for expression elements in a rule
	
	// This should let us compute by iterating over the functions in order, passing them
	// the expression element float pointers
	
	[ big array of element floats ]
	[ array of function elements ]
	[ array of pointers to elements in the big array ]
		
	// - each function takes two input floats and produces one output float
	// - a function element will requires pointers to all three of these floats
	// - 
		float elements[] = { a, b, c, ... };
			// some of these will be initially filled in, some NOT_SET
	
		struct func_element {
			float *input1, *input2;
			float result;
			arithmetic_fn *f;
		};
			// an array of these, as ordered by WLayout and ArithmeticExpression
		
		float* inputs[] = { }
	
	// If the memory layout is predictable/calculable, that may help us later
	// alter the layout dynamically
		
		
/*
		Alternative:
		
		- all UIElements contiguously [ A, B, C ... ]
		- a ordered set of memory mappings & operations between their properties
		- the operations are jenniferaniston-like, i.e. typed rather than expressions
		
		e.g.
			PropertySetter_OffsetConst: B.x -> A.x + 20
			PropertySetter_OffsetProportional: B.w -> Container.w * 0.5;
			PropertySetter_Offset
		
		An alternative to typing/virtual inheritance: functors or llambdas:
		
			elements:	[ A, B, C, ...]
			rules:      [ functor, functor, functor, ... ]
		
		For optimal performance, specify the rules not as data, but as code.
		This allows them to be inlined -- no function pointer jumping.
		Provide useful types of functor:
			auto propertySetter_offsetConst = [](float *x, float offset) -> float {
				return cur_target + y;
			};
			auto propertySetter_offsetConst = [](el1, &el2, offset) -> void {
				el2.x = el1.x + offset.x;
				el2.y = el1.y + offset.y;
			};
			
*/
		
	// Non-copyable
   Layout(const Layout&) = delete;
   Layout& operator=(const Layout&) = delete;
};



// float* get_el_property_ptr(ElementSet &els, std::string el_name, std::string prop_name) {
// 	auto it = els.elements.find(el_name);
// 	if (it == els.elements.end()) {
// 		printf("Element '%s' not found!\n", el_name.c_str());
// 		return NULL;
// 	}
//
// 	if (prop_name == "x") return &(it->second).x;
// 	if (prop_name == "y") return &(it->second).y;
// 	if (prop_name == "w") return &(it->second).w;
// 	if (prop_name == "h") return &(it->second).h;
//
// 	return NULL;
// }
// float* get_el_property_ptr_for_string(ElementSet &els, std::string rule_name) {
// 	std::vector<std::string> el_name_components;
// 	str_split(rule_name, el_name_components, '.');
// 	if (el_name_components.size() != 2) {
// 		return NULL;
// 	}
// 	return get_el_property_ptr(els, el_name_components[0], el_name_components[1]);
// }

