#include "Items.h"

void Weapon::attack() 
{
	auto payload = hitDetection();

	if (payload.targetsHit.empty() && payload.objectsHit.empty())
	{
		return;
	}
}

Weapon::HitPayload Weapon::hitDetection() 
{
	HitPayload payload;

	return payload;
}

int Weapon::calculateDamage(Character target) 
{
	int result = 0; 
	for (size_t i = 0; i < MAX_NUMBER_OF_DAMAGE_TYPES; i++)
	{
		if (m_damageAmounts->at(i) > target.m_armorTypes->at(i))
			result += m_damageAmounts->at(i) - target.m_armorTypes->at(i);
	}
	if (!result) // Always deals minimum of 1 damage
		result++;
	return result;
}