#pragma once

#include "DamageTypes.h"
#include "Character.h"
#include "Object.h"

#include <string>
#include <vector>
#include <array>
#include <memory>

class Weapon {
public:
	struct HitPayload {
		std::vector<Character> targetsHit;
		std::vector<Object> objectsHit;
		int closestCharacterHit;
	};

public:
	std::string m_name;
	std::shared_ptr<std::array<uint32_t, MAX_NUMBER_OF_DAMAGE_TYPES>> m_damageAmounts;
	int m_hitBoxIndex;
	bool m_canMultiHit;

	virtual void attack();
	[[nodiscard]] virtual HitPayload hitDetection();
	virtual int calculateDamage(Character target);
};

class Armor {
public:
	std::string m_name;
	std::shared_ptr<std::array<uint32_t, MAX_NUMBER_OF_DAMAGE_TYPES>> m_armorAmounts;
};

class Spell {
public:
	std::string m_name;
	std::shared_ptr<std::array<uint32_t, MAX_NUMBER_OF_DAMAGE_TYPES>> m_damageAmounts;
	int m_castingTime;
	int m_spellDuration;
	bool interruptable;
};