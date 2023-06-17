#pragma once

#include "Items.h"

#include <glm/glm.hpp>

#define PLAYER_MAX_HEALTH 10
#define PLAYER_MAX_SPEED 1.0f


enum class PlayerState {
	Idle, Moving, Attacking, Dodging, Casting, Hit, Using, Dying, Spawning, Knockbacked
};

enum class PlayerAnimations {
	Idle, Moving, Attacking
};

struct PlayerAnimationTimes {
	/*
	Struct that contains all the frame threshholds for when 
	an animation should be played and which sprite should be used in 
	addition. Times are given in seconds
	*/
	PlayerAnimations activeAnimation = PlayerAnimations::Idle;
	float animationDuration = 0.0f;
	float animationUpdateTimer = 0.0f;
	const float idle_switchSprites = 0.25f;
	const float move_switchSprites_01 = 0.25f;
	float move_startup_01 = 0.3f;
	float move_startup_02 = 0.7f;
};

class Player {
public:
	unsigned int rendererID = -1;
	glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };
	glm::vec3 m_lastPosition{ 0.0f, 0.0f, 0.0f }; // Needed for deceleration
	float m_speed = 5.0f; // Tiles per second
	float m_rotationAngle = 0.0f;
	float m_rotationSpeed = 540.0f; //degrees per second
	bool m_facingRight = true; // Only left and right possible
	unsigned int m_currentHealth, m_maxHealth;
	int invincibilityFrame, attackCoolDownFrames;
	PlayerState m_state = PlayerState::Idle;
	int m_spriteIndex = 0;
	int m_hitBoxIndex = -1;
	Weapon* m_weapon;
	Spell* m_spell;
	Armor* m_armor;
	PlayerAnimationTimes m_animations;

	Player() = default;
	void init();
	void onSpawn();
	void onUpdate();
private:
	void move();
	void rotate();
	glm::vec3 tryMove(glm::vec3 move);
	void startAnimation(PlayerAnimations animation);
	void updateAnimation();
};