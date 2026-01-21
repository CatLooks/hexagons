#include "localization/token.hpp"
#include <cassert>
#include <string_view>
#include <utility>

int main() {
	using namespace localization;

	Section root;
	// hello -> "hi"
	root.items.push_back({ "hello", Text{ "hi", {} } });

	// ns.world -> "earth"
	Section ns;
	ns.items.push_back({ "world", Text{ "earth", {} } });
	root.items.push_back({ "ns", std::move(ns) });

	// prosty klucz
	auto t1 = root.req("hello");
	assert(t1.get({}) == "hi");

	// zagnie¿d¿ony klucz
	auto t2 = root.req("ns.world");
	assert(t2.get({}) == "earth");

	// brakuj¹cy klucz daje tekst zaczynaj¹cy siê od "!("
	auto t3 = root.req("ns.missing");
	auto s3 = t3.get({});
	assert(s3.rfind("!(", 0) == 0); // starts_with
	return 0;
}