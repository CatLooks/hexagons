#include "ui/units.hpp"
#include <cassert>

int main() {
	using ui::Dim;
	using ui::DimVector;
	using ui::DimRect;

	// Dim get: px + ps*parent + ts*element
	Dim d = Dim::from_px(10.f) + Dim::from_ps(0.5f) + Dim::from_ts(2.f);
	assert(d.get(100, 20) == 10 + int(0.5f * 100) + int(2.f * 20)); // 10 + 50 + 40 = 100

	// Dim operatory
	Dim a = 10px;
	Dim b = 1ps;
	Dim c = a + b;
	assert(c.get(20, 0) == 30); // 10 + 1*20

	// DimVector get
	DimVector dv(1ps, 0.5ps);
	auto v = dv.get({200, 100}, {0, 0});
	assert(v.x == 200 && v.y == 50);

	// DimRect get: pozycja + rozmiar
	DimRect r({0px, 0px}, {0.5ps, 0.5ps});
	auto rect = r.get({{10, 20}, {200, 100}});
	assert(rect.position.x == 10 && rect.position.y == 20);
	assert(rect.size.x == 100 && rect.size.y == 50);

	return 0;
}