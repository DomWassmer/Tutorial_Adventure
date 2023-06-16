#pragma once

#include "Game.h"

#include <memory>
#include <string>

#include <glm/glm.hpp>

class AI;

class NPC {
public:
	std::string m_name;
	glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };
	glm::vec3 m_lastPosition{ 0.0f, 0.0f, 0.0f }; // Needed for deceleration
	float m_speed = 5.0f; // Tiles per second
	float m_rotationAngle = 0.0f;
	float m_rotationSpeed = 540.0f; //degrees per second
	bool m_facingRight = true; // Only left and right possible
	unsigned int m_currentHealth, m_maxHealth;
	int m_spriteIndex = 0;
	
	AI* ai = nullptr;
};

class Enemy : public NPC {
public:
	int invincibilityFrame, attackCoolDownFrames;
	unsigned int state = -1;
	int m_hitBoxIndex = -1;

};

class NPCFactory {
public:
	static std::shared_ptr<Enemy> createSkeleton(glm::vec3 position);
};