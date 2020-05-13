
#include "WLayout.h"

#pragma mark Helper functions

using Str = std::string;
using Term = WLayout::Term;
using Operation = WLayout::Operation;
template<class T>
using Vec = std::vector<T>;

namespace WLTHelpers {
	Vec<Str> str_split(const Str &s, char delim) {
		Vec<Str> v;
		const char *_s = s.c_str();
		for (size_t i=0; i < s.size(); ) {
			Str temp;
			for (size_t j=i; j < s.size() && _s[j] != delim; ++j)
				temp += s[j];
			if (temp.size() > 0) {
				v.push_back(temp);
				i += temp.size();
			}
			else {
				i += 1;
			}
		}
		return v;
	}
	Str str_first_before(const Str &s, char delim) {
		return str_split(s, delim)[0];
	}
	template<class T>
	typename Vec<T>::const_iterator v_find(const Vec<T> &v, const T x) {
		for (auto it = v.begin(), end = v.end(); it != end; ++it) {
			if (*it == x) return it;
		}
		return v.end();
	}
	template<class T>
	Vec<T> v_uniq(const Vec<T> &v) {
		Vec<T> u;
		for (const auto &i : v) {
			if (v_find(u, i) == u.end())
				u.push_back(i);
		}
		return u;
	}
	bool is_numeric(char c) {
		return (c == '-' || c == '.' || (c >= '0' && c <= '9'));
	}
	bool is_entirely_numeric(Str &s) {
		for (auto c : s) {
			if (!is_numeric(c))
				return false;
		}
		return true;
	}
}

Vec<Str> getElementNames(Diatom &elements);
Vec<Operation> getOperations(Diatom &rules, Vec<Str> &errors, const Vec<Str> &element_names);


#pragma mark WLayout

Vec<Str> WLayout::loadFromDiatom(Diatom &d) {
	
	// 1. Get list of elements & element names
	element_names = getElementNames(d["elements"]);
	elements.clear();
	elements.resize(element_names.size(), {1, 1, 1, 1});
	
	
	// 2. Get list of operations
	Vec<Str> errors;
	operations = getOperations(d["rules"], errors, element_names);

	return errors;
}

int WLayout::id_for_element(const std::string &elname) {
	for (int i=0, n = (int) element_names.size(); i < n; ++i) {
		if (element_names[i] == elname) return i;
	}
	return -1;
}

void WLayout::compute(Rect container) {
	elements[1] = container;
	
	auto compute_op = [](Operation &op, Vec<Rect> &elements) {
		float result = 0;
		float *f_els = (float*) &elements[0];
		for (auto &t : op.terms) {
			result += t.k * f_els[t.property_index];
		}
		return f_els[op.target_property] = result;
	};
	
	for (auto &op : operations) compute_op(op, elements);
	
//		for (auto i : element_indices_by_name) {
//			auto s = i.first;
//			auto ind = i.second;
//			auto el = elements[ind];
//			printf("%s: %.1f,%.1f %.1fx%.1f\n", s.c_str(), el.x, el.y, el.w, el.h);
//		}
}


// Helpers

Vec<Str> getElementNames(Diatom &d_elements) {
	Vec<Str> element_names;
	element_names.push_back("Default");
	element_names.push_back("Container");
	d_elements.depth_first_recursive([&](Str name, Diatom &d) {
		element_names.push_back(name);
	});
	return element_names;
}

Vec<Operation> getOperations(Diatom &d_rules, Vec<Str> &errors, const Vec<Str> &element_names) {
	// Find index for named element in elements vector
	auto index_for_elname = [&](Str elname) {
		for (int i=0; i < element_names.size(); ++i) {
			if (element_names[i] == elname)
				return i;
		}
		return -1;
	};
	
	// Find index of property into elements vector
	auto index_for_property_descr = [&](Str pdescr) {
		auto parts = WLTHelpers::str_split(pdescr, '.');
		int ind_el;
		bool OK =
			parts.size() == 2 &&
			(ind_el = index_for_elname(parts[0])) >= 0 &&
			(parts[1] == "x" || parts[1] == "y" || parts[1] == "w" || parts[1] == "h");
		if (!OK) {
			errors.push_back(Str("Invalid positioning rule: '") + pdescr + Str("'"));
			return -1;
		}
		char prop = parts[1][0];
		return ind_el * 4 + (
			prop == 'x' ? 0 :
			prop == 'y' ? 1 :
			prop == 'w' ? 2 : 3
		);
	};
	
	// Get a vector of terms from an operation string
	auto rule_string_to_term_vec = [&](Str opdescr) -> Vec<Term> {
		Vec<Term> v;
		Vec<Str> parts;
		{
			// Discard the odd bits of the string, since we just add all the terms together
			auto parts_temp = WLTHelpers::str_split(opdescr, ' ');
			bool next_negative = false;
			for (int i=0, n = (int)parts_temp.size(); i < n; ++i) {
				auto &s = parts_temp[i];
				if (i%2 == 1) {
					if (s == "-") next_negative = true;
				}
				else {
					if (!WLTHelpers::is_numeric(s[0])) s = Str("1*") + s;
					if (next_negative) {
						s = Str("-") + s;
					}
					parts.push_back(s);
					next_negative = false;
				}
			}
		}
	
		for (auto &s : parts) {
			Term t;
			// If is a constant expression, set k = expr, ind => the default property (1)
			if (WLTHelpers::is_entirely_numeric(s)) {
				t.k = std::stof(s);
				t.property_index = 0;
			}
			// Otherwise, expect format k*eldescr
			else {
				auto term_parts = WLTHelpers::str_split(s, '*');
				// printf()
				t.k = std::stof(term_parts[0]);
				t.property_index = index_for_property_descr(term_parts[1]);
			}
			v.push_back(t);
		}
		return v;
	};
	
	
	Vec<Operation> operations;
	d_rules.each_descendant([&](Str &key, Diatom &d) {
		int ind_targetProp = index_for_property_descr(key);
		if (ind_targetProp < 0) return;
		Operation op = {
			(size_t) ind_targetProp,
			rule_string_to_term_vec(d.str_value())
		};
		operations.push_back(op);
	});
	
	return operations;
}


int WLayout::id_for_property_operation(int ind_rect, int xywh) {
	int ind_op = -1;
	for (int i = 0, n = (int) operations.size(); i < n; ++i) {
//		printf("%d\n", operations[i])
		if (operations[i].target_property == ind_rect * 4 + xywh) {
			ind_op = i;
			break;
		}
	}
	return ind_op;
}

void WLayout::nudge_element_by_property(int op_id, float delta) {
	auto &op = operations[op_id];
	auto &last_term = op.terms.back();
	// If last term is a constant expression already, modify it
	if (last_term.property_index == 0) {
		last_term.k += delta;
	}
	// Otherwise, add a new constant expression on the end
	else {
		op.terms.push_back({ delta, 0 });
	}
}


