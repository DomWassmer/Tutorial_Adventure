#pragma once

#include <array>

// Hit and damage calcs and armors inspired by AOE2; 

#define MAX_NUMBER_OF_DAMAGE_TYPES 18

// Option between array and enums or map and enum class, I think this may be better and produces less searching
// As a result numbers should not be changed in later patches and additional damage types may only be added to the end of the list

// inline variables require C++17 or higher
inline std::array<uint32_t, MAX_NUMBER_OF_DAMAGE_TYPES> g_baseDamageAmount{
	//Non magical basic damage types
	0, //Bludgeoning
	0, //Slashing
	0, //Piercing
	0, //Bleeding

	//Magical basic damage types
	0, //MagicalBludgeoning
	0, //MagicalSlashing
	0, //MagicalPiercing

	//Elemental damage types
	0, //Fire
	0, //Frost
	0, //Lightning
	0, //Nature

	0, //Necrotic
	0, //Radiant
	0, //Eldritch

	0, //Poisonous
	0, //Acid
	0, //Psychic
	0 //Thunder
};
inline std::array<uint32_t, MAX_NUMBER_OF_DAMAGE_TYPES> g_baseArmorAmount{
	//Non magical basic damage types
	1000, //Bludgeoning
	1000, //Slashing
	1000, //Piercing
	1000, //Bleeding

	//Magical basic damage types
	1000, //MagicalBludgeoning
	1000, //MagicalSlashing
	1000, //MagicalPiercing

	//Elemental damage types
	1000, //Fire
	1000, //Frost
	1000, //Lightning
	1000, //Nature

	1000, //Necrotic
	1000, //Radiant
	1000, //Eldritch

	1000, //Poisonous
	1000, //Acid
	1000, //Psychic
	1000 //Thunder
};

enum DamageTypes {
	// Non magical basic damage types
	Bludgeoning = 0,
	Slashing = 1,
	Piercing = 2,
	Bleeding = 3,

	// Magical basic damage types
	MagicalBludgeoning = 4,
	MagicalSlashing = 5,
	MagicalPiercing = 6,

	// Elemental damage types
	Fire = 7,
	Frost = 8,
	Lightning = 9,
	Nature = 10,

	Necrotic = 11,
	Radiant = 12,
	Eldritch = 13,

	Poisonous = 14,
	Acid = 15,
	Psychic = 16,
	Thunder = 17
};
