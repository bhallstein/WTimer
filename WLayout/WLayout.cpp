#include "WLayout.h"

float add(float a, float b)      {  return a + b;  }
float subtract(float a, float b) {  return a - b;  }
float mult(float a, float b)     {  return a * b;  }
float div(float a, float b)      {  return a / b;  }

typedef float (arithmetic_fn)(float, float);


std::vector<std::string> str_split(const std::string &s, char delim) {
	std::vector<std::string> v;
	const char *_s = s.c_str();
	for (size_t i=0; i < s.size(); ++i) {
		std::string temp;
		for (size_t j=i; j < s.size() && _s[j] != '.'; ++j)
			temp += s[j];
		if (temp.size() > 0)
			v.push_back(temp);
	}
	return v;
}
std::string str_first_before(const std::string &s, char delim) {
	std::vector<std::string> v = str_split(s, delim);
	return v[0];
}

bool target_for_str(const std::string &s, Rule::Target &t, const std::map<std::string, Element> &els) {
	auto components = str_split(s, '.');
	if (components.size() != 2) return false;
	
	Rule::Target _t;
	if      (components[1] == "x") { _t.property = Rule::Target::X; }
	else if (components[1] == "y") { _t.property = Rule::Target::Y; }
	else if (components[1] == "w") { _t.property = Rule::Target::W; }
	else if (components[1] == "h") { _t.property = Rule::Target::H; }
	else return false;
	
	const auto it = els.find(components[0]);
	if (it == els.end()) return false;
	
	_t.el_name = components[0];
	t = _t;
	return true;
}

void set_target_property(Rule::Target &t, std::map<std::string, Element> &els, float x) {
	auto &el = els[t.el_name];
	if      (t.property == Rule::Target::X) { el.x = x; }
	else if (t.property == Rule::Target::Y) { el.y = x; }
	else if (t.property == Rule::Target::W) { el.w = x; }
	else if (t.property == Rule::Target::H) { el.h = x; }
}
float get_target_proprety(const Rule::Target &t, const std::map<std::string, Element> &els) {
	const auto &el = els.find(t.el_name)->second;
	if      (t.property == Rule::Target::X) { return el.x; }
	else if (t.property == Rule::Target::Y) { return el.y; }
	else if (t.property == Rule::Target::W) { return el.w; }
	else if (t.property == Rule::Target::H) { return el.h; }
	return NOT_SET;
}


Layout::Layout(Diatom &d_layout) {
	Layout l;
	
	// Get elements
	for (auto &i : d_layout["intrinsics"].descendants()) {
		std::string el_name = i.first;
		Diatom &d_el = i.second;
		Element e;
		if (d_el["x"].isNumber()) e.x = d_el["x"].number_value();
		if (d_el["y"].isNumber()) e.y = d_el["y"].number_value();
		if (d_el["w"].isNumber()) e.w = d_el["w"].number_value();
		if (d_el["h"].isNumber()) e.h = d_el["h"].number_value();
		l.elements[el_name] = e;
	}
	for (auto &i : d_layout["rules"].descendants()) {
		std::string el_name = str_first_before(i.first, '.');
		if (l.elements.find(el_name) == l.elements.end()) {
			l.elements[el_name] = Element();
		}
	}
	
	// Get rules
	for (auto &i : d_layout["rules"].descendants()) {
		Rule r;
		bool success = target_for_str(i.first, r.target, l.elements);
		if (!success) {
			printf("Parse error for target '%s'\n", i.first.c_str());
			continue;
		}

		if (!i.second.isString()) {
			printf("Parse error for rule '%s': property is not a string?\n", i.first.c_str());
			continue;
		}
		std::string expr_str = i.second.str_value();
		printf("layout rule: '%s'\n", expr_str.c_str());
		r.expression = Ar::get(expr_str);

		l.rules.push_back(r);
	}
	
	return l;
}


bool get_property_for_str(const std::string &s, const std::map<std::string, Element> &els, float &f) {
	Rule::Target t;
	bool success = target_for_str(s, t, els);
	if (!success) return false;
	
	const auto it = els.find(t.el_name);
	const auto &el = it->second;
	f = (
		t.property == Rule::Target::X ? el.x :
		t.property == Rule::Target::Y ? el.y :
		t.property == Rule::Target::W ? el.w : el.h
	);
	
	return true;
}


std::map<std::string, Element> Layout::compute() {
	std::map<std::string, Element> results = elements;
	
	for (auto &r : rules) {
		Rule::Target &target = r.target;
		std::vector<float> equivs;
		
		for (auto &s : r.expression.elements) {
			float f;
			printf("%s ", s.c_str());
			bool found = get_property_for_str(s, elements, f);
			if (found) {
				equivs.push_back(f);
			}
			else {
				equivs.push_back(std::stof(s));
			}
		}
		printf("\n");
		for (auto f : equivs) printf("%.2f  ", f);
		printf("\n");

		float result = Ar::compute(r.expression, equivs);
		set_target_property(target, results, result);
		
		printf(" -> %.2f\n", result);
	}
	return results;
}
