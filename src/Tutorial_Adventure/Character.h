#pragma once

#include "DamageTypes.h"

#include <string>
#include <array>
#include <memory>

class Character {
public:
	std::string m_name;
	std::shared_ptr<std::array<uint32_t, MAX_NUMBER_OF_DAMAGE_TYPES>> m_armorTypes;
	
	void init();
	void onSpawn();
	void onUpdate();
};