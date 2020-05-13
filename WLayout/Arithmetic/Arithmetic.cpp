#include "Arithmetic.h"

using Str = std::string;

bool is_operator_char(char c) {
	return (c == '*' || c == '/' || c == '+' || c == '-' || c == '(' || c == ')');
}

Ar::Operator operator_for_char(char c) {
	Ar::Operator op;
	if      (c == '*') op.op = Ar::Operator::Multiply;
	else if (c == '/') op.op = Ar::Operator::Divide;
	else if (c == '+') op.op = Ar::Operator::Add;
	else if (c == '-') op.op = Ar::Operator::Subtract;
	else op.op = Ar::Operator::Unknown;
	return op;
}

bool op2_is_higher_precedence(Ar::Operator o1, Ar::Operator o2) {
	if (o2.op == Ar::Operator::Multiply || o2.op == Ar::Operator::Divide)
		return (o1.op == Ar::Operator::Add || o1.op == Ar::Operator::Subtract);
	return false;
}

namespace Ar {
	struct Element {
		Str e;
		int precedence_modifier = 0;

		void print() const {
			printf(" element: %s", e.c_str());
			if (precedence_modifier < 0)      for (int i=0; i > precedence_modifier; --i) printf("v");
			else if (precedence_modifier > 0) for (int i=0; i < precedence_modifier; ++i) printf("^");
			printf("\n");
		}
	};
}

Ar::Operator sp_getOperator(Str s, int &i) {
	return operator_for_char(s[i++]);
}

Ar::Element sp_getElement(Str s, int &i) {
	Ar::Element el;
	for ( ; s[i] == '('; ++i) {
		++el.precedence_modifier;
	}
	if (s[i] == '-') {
		el.e += '-';
		++i;
	}
	while (i < s.size() && !is_operator_char(s[i]))
		el.e += s[i++];
	for ( ; s[i] == ')'; ++i) {
		--el.precedence_modifier;
	}
	return el;
}

bool get_element_lists(Str s, std::vector<Ar::Element> &els, std::vector<Ar::Operator> &ops) {
	int i=0;
	std::vector<Ar::Element> _els;
	std::vector<Ar::Operator> _ops;
	
	while (i < s.size()) {
		Ar::Element el = sp_getElement(s, i);
		if (el.e == "") return false;
		
		Ar::Operator o = sp_getOperator(s, i);
		if (o.op == Ar::Operator::Unknown && i < s.size()) return false;
		
		_els.push_back(el);
		
		if (i < s.size()) {
			_ops.push_back(o);
		}
	}
	els = _els;
	ops = _ops;
	return true;
}


Str remove_spaces(const Str &_s) {
	Str s;
	for (auto c : _s) if (c != ' ') { s += c; }
	return s;
}


Ar::ArithmeticExpression Ar::get(Str s) {
	ArithmeticExpression expr;
	s = remove_spaces(s);
	
	// Canonical elements array, including original precedence modifiers
	std::vector<Ar::Element> els;
	
	// Operators array. Modified in-flight in course of algorithm
	std::vector<Ar::Operator> inflight_ops;
	
	// Parse the string
	bool success = get_element_lists(s, els, inflight_ops);
	if (!success) {
		return expr;
	}
	
	// Create an vector of indices of elements, for the algorithm to work on,
	// and a parallel array of precedence modifiers
	std::vector<ArithmeticExpression::Input> inflight_elements;
	for (size_t i=0, n=els.size(); i < n; ++i) inflight_elements.push_back({
		ArithmeticExpression::Input::Element,
		i
	});
	
	// Create a parallel vector of precedence modifiers (brackets), which the algorithm also modifies
	std::vector<int> precedence_modifiers;
	for (const auto &el : els) precedence_modifiers.push_back(el.precedence_modifier);
	
	
	auto add_op_to_output = [&](size_t ind_op) -> void {
		size_t ind_el1_in_inflight = ind_op;
		size_t ind_el2_in_inflight = ind_op + 1;
		size_t ind_stg = expr.stages.size();

		ArithmeticExpression::Stage stg;
		stg.input1 = inflight_elements[ind_el1_in_inflight];
		stg.input2 = inflight_elements[ind_el2_in_inflight];
		stg.op = inflight_ops[ind_op];
		expr.stages.push_back(stg);
		
		int &precmod_1 = precedence_modifiers[ind_el1_in_inflight];
		int &precmod_2 = precedence_modifiers[ind_el2_in_inflight];

		int precmod1_prev = precmod_1;
		// Modify the inflight elements, ops, and precedence vectors:

		// - precedence
		int precmod_new = 0;
		if (precmod_1 > 0) precmod_new += precmod_1 - 1;
		if (precmod_2 < 0) precmod_new += precmod_2 + 1;
		precmod_1 = precmod_new;
		precedence_modifiers.erase(precedence_modifiers.begin() + ind_el2_in_inflight);
		
		
		// - remove the operation
		inflight_ops.erase(inflight_ops.begin() + ind_op);
		
		// - replace the two inflight elements with a ref to the just-added Stage
		inflight_elements.erase(inflight_elements.begin() + ind_el2_in_inflight);
		inflight_elements[ind_el1_in_inflight] = {
			ArithmeticExpression::Input::Function,
			ind_stg
		};
	};
	
	bool changed;
	do {
		changed = false;
		
		if (inflight_ops.size() == 1) {
			add_op_to_output(0);
			changed = true;
			continue;
		}
		
		for (int i=1; i < inflight_ops.size(); ++i) {
			// We're always looking at two operators, and we want
			// to determine if the previous operator can be safely pushed to output
			const Operator &op   = inflight_ops[i];
			const Operator &prev = inflight_ops[i-1];

			bool prev_expr_has_close_br = (precedence_modifiers[i] < 0);
			bool cur_expr_has_open_br   = (precedence_modifiers[i] > 0);
			bool operator_precedence_has_increased = op2_is_higher_precedence(prev, op);
			
			if (
				prev_expr_has_close_br ||
				!(cur_expr_has_open_br || operator_precedence_has_increased)
			) {
				add_op_to_output(i-1);
				changed = true;
				break;
			}
			
			// If we haven't added the previous operator, and this is the last operator,
			// add it
			if (i == inflight_ops.size() - 1) {
				add_op_to_output(i);
				changed = true;
				break;
			}
		}
	} while (changed);
	
	// Copy the unchanged element vector to the Expression
	for (auto &el : els) expr.elements.push_back(el.e);
	
	return expr;
}


float Ar::compute(const ArithmeticExpression &expr) {
	std::vector<float> function_results;
	for (auto &stg : expr.stages) {
		float i1, i2;
		if (stg.input1.type == Ar::ArithmeticExpression::Input::Element)
			i1 = std::stof(expr.elements[stg.input1.i]);
		else
			i1 = function_results[stg.input1.i];

		if (stg.input2.type == Ar::ArithmeticExpression::Input::Element)
			i2 = std::stof(expr.elements[stg.input2.i]);
		else
			i2 = function_results[stg.input2.i];
		
		printf("%.1f %.1f ", i1, i2);
		printf("%s\n",
			stg.op.op == Ar::Operator::Add ? "+" : 
			stg.op.op == Ar::Operator::Subtract ? "-" : 
			stg.op.op == Ar::Operator::Multiply ? "*" : 
			"/"
		);
		
		float r;
		if      (stg.op.op == Ar::Operator::Add)      r = i1 + i2;
		else if (stg.op.op == Ar::Operator::Subtract) r = i1 - i2;
		else if (stg.op.op == Ar::Operator::Multiply) r = i1 * i2;
		else if (stg.op.op == Ar::Operator::Divide)   r = i1 / i2;
		function_results.push_back(r);
	}
	return function_results.back();
}
float Ar::compute(const ArithmeticExpression &expr, const std::vector<float> &equivs) {
	std::vector<float> function_results;
	for (auto &stg : expr.stages) {
		float i1, i2;
		if (stg.input1.type == Ar::ArithmeticExpression::Input::Element)
			i1 = equivs[stg.input1.i];
		else
			i1 = function_results[stg.input1.i];

		if (stg.input2.type == Ar::ArithmeticExpression::Input::Element)
			i2 = equivs[stg.input2.i];
		else
			i2 = function_results[stg.input2.i];
		
		printf("%.1f %.1f ", i1, i2);
		printf("%s\n",
			stg.op.op == Ar::Operator::Add ? "+" : 
			stg.op.op == Ar::Operator::Subtract ? "-" : 
			stg.op.op == Ar::Operator::Multiply ? "*" : 
			"/"
		);
		
		float r;
		if      (stg.op.op == Ar::Operator::Add)      r = i1 + i2;
		else if (stg.op.op == Ar::Operator::Subtract) r = i1 - i2;
		else if (stg.op.op == Ar::Operator::Multiply) r = i1 * i2;
		else if (stg.op.op == Ar::Operator::Divide)   r = i1 / i2;
		function_results.push_back(r);
	}
	return function_results.back();
}
