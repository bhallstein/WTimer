#include "Arithmetic.h"

#define OUT(s) printf(s)

void print(Ar::Operator op) {
	printf("%s", 
		op.op == Ar::Operator::Add ? "+" :
		op.op == Ar::Operator::Subtract ? "-" :
		op.op == Ar::Operator::Multiply ? "*" :
		op.op == Ar::Operator::Divide ? "/" : "Unknown"
	);
}

void print_stage(Ar::ArithmeticExpression::Stage stg) {
	printf("Stage: "); print(stg.op); //printf("\n");
	printf(
		"  %s %lu,",
		stg.input1.type == Ar::ArithmeticExpression::Input::Element ? "element" : "function",
		stg.input1.i
	);
	printf(
		"  %s %lu\n",
		stg.input2.type == Ar::ArithmeticExpression::Input::Element ? "element" : "function",
		stg.input2.i
	);
}


int main(int argc, const char **argv) {
	if (argc != 2) {
		printf("u wot mate\n");
		return 0;
	}
	
	Ar::ArithmeticExpression expr = Ar::get(argv[1]);
	printf("n_elements: %lu\n", expr.elements.size());
	printf("n_stages:   %lu\n", expr.stages.size());
	
	for (auto el : expr.elements)
		printf("%s ", el.c_str());
	for (auto stg : expr.stages)
		print_stage(stg);
	
	printf(" -> %.2f\n", Ar::compute(expr));
	
	return 0;
}

