/*
 * WCallback.h - a simple templated C++ method callback
 *
 * The point here is to implement callbacks to methods with the same function signature
 * but belonging to different classes.
 *
 * Why might this be useful?
 * - Because certain types of library might like to have multiple classes providing the same
 *   'type' of callback
 *
 * Wouldn't llambdas and std::function be better?
 * - Probably! There'd be no need to encode the class type!
 *
 * Example usage:
 *   struct X {  int f(float x) { return 1 * x; }  };  // NB method signatures are the same,
 *   struct Y {  int g(float x) { return 2 * x; }  };  // but two different classes
 *   
 *   X x;
 *   Y y;
 *   
 *   WCallback<int, float> *callbacks[] = {    // We can store callbacks from multiple classes
 *      make_wcallback(&X::f, &x),             // in a container! Hooray!
 *      make_wcallback(&Y::g, &y)
 *   };
 *   
 *   int x_result = callbacks[0]->call(3.0);   // x_result -> 3
 *   int y_result = callbacks[1]->call(3.0);   // y_result -> 6
 *
 *
 * Could probably be 'improved' with some clever type erasure, but would add complexity without
 * simplifying usage.
 *
 * Extracted from legacy W library, modified & made standalone 2016.4.23
 * by Ben Hallstein
 *
 */

#ifndef __WCallback
#define __WCallback

template<class TRet, class TParam>
class WCallback {
public:
	virtual ~WCallback() { }
	virtual TRet call(TParam) = 0;
	virtual void* resp() = 0;
};

template<class TCla, class TRet, class TParam>
using wcb_member_fn_pointer = TRet (TCla::*)(TParam);

template <class TCla, class TRet, class TParam>
class WCallback_Derived : public WCallback<TRet, TParam> {
typedef wcb_member_fn_pointer<TCla, TRet, TParam> mftype;
public:
	WCallback_Derived(mftype _f, TCla *_o) : f(_f), o(_o) { }
	~WCallback_Derived() { }
	TRet call(TParam p) {
		return (o->*f)(p);
	}
	void* resp() { return o; }

private:
	mftype f;
	TCla *o;
};

template <class TCla, class TRet, class TParam>
WCallback<TRet, TParam>* make_wcallback(wcb_member_fn_pointer<TCla, TRet, TParam> f, TCla *o) {
	return new WCallback_Derived<TCla, TRet, TParam>(f, o);
}

#endif
