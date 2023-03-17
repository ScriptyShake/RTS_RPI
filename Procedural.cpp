#include "Procedural.h"
#include <cmath>
#include <iostream>
#include <random>

Tmpl9::Procedural::Procedural()
{
	srand(time(0));
}

std::string Tmpl9::Procedural::NameGenerator(unsigned seed) const
{
	std::mt19937 generator(seed);
	std::uniform_real_distribution<float> v_distribution(0.0, 5.0);
	std::uniform_real_distribution<float> c_distribution(0.0, 20.0);
	
	int wordlength_index;
	wordlength_index = rand() % 3 + 2;
	std::string name = "";

	for (int i = 0; i < wordlength_index; ++i)
	{
		int vowel_index = v_distribution(generator);
		int consonant_index = c_distribution(generator);
		name = name + consonants[consonant_index];
		name = name + vowels[vowel_index];
	}
	return name;
}


Tmpl9::Procedural::~Procedural()
= default;
