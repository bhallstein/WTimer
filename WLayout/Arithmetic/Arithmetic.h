#ifndef __Arithmetic
#define __Arithmetic

#include <string>
#include <vector>

using Str = std::string;

namespace Ar {

	struct Operator {
		enum Type { Add, Subtract, Multiply, Divide, Unknown } op;
	};
	
	struct ArithmeticExpression {
		struct Input {
			enum Type { Element, Function } type;
			size_t i;
		};
		
		struct Stage {
			Input input1, input2;
			Operator op;
		};
		
		std::vector<Str> elements;
		std::vector<Stage> stages;
	};

	ArithmeticExpression get(Str s);

	float compute(const ArithmeticExpression &);
		// Compute directly
		// Will fail if any expression is not stof-able

	float compute(const ArithmeticExpression &, const std::vector<float> &element_equivs);
		// Compute, substituting the elements vector
		// for a vector of user-supplied floats
}

#endif
