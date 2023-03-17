#pragma once
#include <string>

namespace Tmpl9
{
	class Procedural
	{
	public:
		Procedural();
		[[nodiscard]] std::string NameGenerator(unsigned seed) const;
		~Procedural();

	private:
		std::string vowels[6] = {"A", "E", "I", "O", "U", "Y"};
		std::string consonants[21] = {"B", "C", "D", "F", "G", "J", "K", "L", "M", "N", "P", "Q", "S", "T", "V", "X", "Z", "H", "R", "W", "Y"};
	};
}
