/*
 * WLayout.h
 *
 *
 */

#ifndef __WLayout_h
#define __WLayout_h

#include "Diatom-Storage.h"
#include <vector>
#include <map>

class WLayout {
public:
	std::vector<std::string> loadFromDiatom(Diatom &d);	// returns errors, if any
	
	struct Rect {
		float x, y, w, h;
		bool operator==(const Rect &e) {
			return x == e.x && y == e.y && w == e.w && h == e.h;
		}
		bool point_within(float px, float py) {
			return (px >= x && py >= y && px < x + w && py < y + h);
		}
	};
	
	int id_for_element(const std::string &elname);
	void compute(Rect container);
	
	Rect& get(int id) { return elements[id]; }
	unsigned long n() { return elements.size() - 2; }

	template<class T>
	void each(T f) {
		for (size_t i=2, _n = elements.size(); i < _n; ++i)
			f(elements[i]);
	}
	template<class T>
	void each_by_name(T f) {
		for (size_t i=2, n = element_names.size(); i < n; ++i)
			f(element_names[i], elements[i]);
	}
	
	int id_for_property_operation(int elm_id, int property);
	void nudge_element_by_property(int op_id, float delta);
	
	struct Term {
		float k;
		size_t property_index;   // NB offset is into elements *as floats*
	};
	struct Operation {
		size_t target_property;  // NB ~
		std::vector<Term> terms;
	};
	
private:
	std::vector<Rect>         elements;
	std::vector<std::string>  element_names;
	std::vector<Operation>    operations;
	
};

#endif
