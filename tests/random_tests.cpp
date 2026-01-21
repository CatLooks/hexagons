#include "random.hpp"
#include <cassert>

int main() {
	// deterministycznoœæ seeda
	Random::seed(123u);
	uint32_t a1 = Random::u32();
	uint32_t a2 = Random::u32();

	Random::seed(123u);
	assert(Random::u32() == a1);
	assert(Random::u32() == a2);

	// uniform w [0,1)
	float u = Random::uniform();
	assert(u >= 0.0f && u < 1.0f);

	// pity: brak sukcesu przy value=0, max=1 -> pierwszy strza³ fa³szywy, licznik roœnie
	uint8_t fail = 0;
	bool r1 = Random::pity(0.0f, fail, 1);
	assert(r1 == false);
	assert(fail == 1);

	// pity: drugi strza³ przy value=0, max=1 -> szansa 1.0, sukces i reset licznika
	bool r2 = Random::pity(0.0f, fail, 1);
	assert(r2 == true);
	assert(fail == 0);

	// pity: value=1.0 gwarantuje sukces i reset licznika
	fail = 3;
	bool r3 = Random::pity(1.0f, fail, 5);
	assert(r3 == true);
	assert(fail == 0);

	return 0;
}