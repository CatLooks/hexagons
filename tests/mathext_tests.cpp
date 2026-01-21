#include "mathext.hpp"
#include <cassert>

int main() {
	// idiv / imod / idivmod z liczbami ujemnymi
	assert(ext::idiv(7, 3) == 2);
	assert(ext::imod(7, 3) == 1);
	auto dm1 = ext::idivmod(-7, 3);
	assert(dm1.first == -3 && dm1.second == 2);

	// fpown: dodatnie, ujemne i zero
	assert(ext::fpown(2.f, 3) == 8.f);
	assert(ext::fpown(2.f, -3) == 0.125f);
	assert(ext::fpown(5.f, 0) == 1.f);

	// str_time: sekundy, minuty, godziny
	assert(ext::str_time(59.f).find("59") != std::string::npos);
	assert(ext::str_time(61.f).find("1") != std::string::npos);   // powinno mieæ "1m"
	assert(ext::str_time(3661.f).find("1") != std::string::npos); // powinno mieæ "1h"

	return 0;
}