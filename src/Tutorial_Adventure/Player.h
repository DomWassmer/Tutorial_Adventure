#pragma once

#include "Items.h"

#include <glm/glm.hpp>

#define PLAYER_MAX_HEALTH 10
#define PLAYER_MAX_SPEED 1.0f


enum class PlayerState {
	Idle, Moving, Attacking, Dodging, Casting, Hit, Using, Dying, Spawning, Knockbacked
};

struct PlayerFrames {
	/*
	Struct that contains all the frame threshholds for when 
	an animation should be played and which sprite should be used in 
	addition 
	*/
	int move_startup_01;
	int move_startup_02;
	int move_fullspeed_03;
};

class Player {
public:
	glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };
	glm::vec3 m_lastPosition{ 0.0f, 0.0f, 0.0f }; // Needed for deceleration
	float m_speed = 0;
	glm::vec3 m_orientation{ 1.0f, 0.0f, 0.0f };
	unsigned int m_currentHealth, m_maxHealth;
	int m_animationFrame, invincibilityFrame, attackCoolDownFrames;
	PlayerState m_state = PlayerState::Idle;
	int m_spriteIndex = -1;
	int m_hitBoxIndex = -1;
	Weapon* m_weapon;
	Spell* m_spell;
	Armor* m_armor;
	PlayerFrames m_frames;

	Player() = default;
	void init();
	void onSpawn();
	void onUpdate();
private:
	void move();
	glm::vec3 tryMove(glm::vec3 move);
};