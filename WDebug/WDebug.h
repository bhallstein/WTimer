/*
 * WDebug.h - simple debug macro
 *
 * - Enable with #define WDEBUG
 * - Calls will be removed completely by compiler if WDEBUG is not defined
 *
 * Usage is as cout:
 *   wdebug << "My hingdom for a korse\n";
 *
 * Extracted from legacy W library, modified & made standalone on 2016.4.23
 * by Ben Hallstein
 * 
 */

struct _wdebug_t {
	const _wdebug_t& operator<<(const char *s) const {
		printf("%s", s);
		return *this;
	}
	explicit operator bool() const { return true; }
	
	static const _wdebug_t& get() {
		static _wdebug_t w;
		return w;
	}
};

#ifdef WDEBUG
	#undef wdebug
	#define wdebug (_wdebug_t::get())
#else
	#undef wdebug
	#define wdebug 0 && (_wdebug_t::get())
#endif
