	#include "div.hpp"

/// Returns `a / d` rounded down.
int idiv(int a, int d) {
	return (int)floorf((float)a / d);
};

/// Returns `a % b` rounded down.
int imod(int a, int d) {
	int r = a % d;
	if (r < 0) r += d;
	return r;
};

/// Calculates quotient and remainder rounded down.
void idivmod(int a, int d, int& q, int& r) {
	q = idiv(a, d);
	r = imod(a, d);
};