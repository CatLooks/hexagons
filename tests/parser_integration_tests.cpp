#include "localization/parser.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
	// mini TLML
	const char* src =
		"menu {\n"
		"  title: \"Hello\"\n"
		"  nested {\n"
		"    key: \"Val\"\n"
		"  }\n"
		"}\n";

	FILE* f = tmpfile();
	assert(f);
	fwrite(src, 1, strlen(src), f);
	rewind(f);

	localization::State st(f);
	auto root = localization::load(st);
	fclose(f);

	// brak b³êdów parsowania
	assert(st.list.empty());

	// proste klucze
	assert(root.req("menu.title").get({}) == "Hello");
	assert(root.req("menu.nested.key").get({}) == "Val");

	// brakuj¹cy klucz -> tekst zaczynaj¹cy siê od "!("
	auto missing = root.req("menu.unknown").get({});
	assert(missing.rfind("!(", 0) == 0);
	return 0;
}