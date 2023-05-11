#include "Game.h"
#include "Player.h"
#include "Input/Input.h"

void Player::init()
{
	// Need more implementation for loading chracters. In this case this is the new game implementaiton
	// Implementation for Walpurgia
	
	// Setting up default frames
	//m_frames = setPlayerFrames();
}

void Player::onSpawn()
{

}

void Player::onUpdate()
{
	// Order here is important for the checking of possible state transitions
	move();
	rotate();
	updateAnimation();
}

void Player::move() {
	// Can only move while in state Moving or Idle
	if (m_state != PlayerState::Idle && m_state != PlayerState::Moving)
		return;
	glm::vec3 moveDirection{ 0.0f, 0.0f, 0.0f };
	glm::vec3 newPosition = m_position;
	if (Input::isKeyDown(KeyCode::W))
		moveDirection.y += 1.0f;
	if (Input::isKeyDown(KeyCode::A))
		moveDirection.x -= 1.0f;
	if (Input::isKeyDown(KeyCode::S))
		moveDirection.y -= 1.0f;
	if (Input::isKeyDown(KeyCode::D))
		moveDirection.x += 1.0f;

	// return to Idle state
	if (moveDirection == glm::vec3{ 0.0f, 0.0f, 0.0f })
	{
		startAnimation(PlayerAnimations::Idle);
		m_state = PlayerState::Idle();
		return;
	}
	/* Rotation Code */
	if (moveDirection.x == -1.0f)
		m_facingRight = false;
	else if (moveDirection.x == 1.0f)
		m_facingRight = true;

	/* Acceleration Code */
	if (moveDirection.x != 0 && moveDirection.z != 0)
		moveDirection = moveDirection * 0.71f;
	if (m_state != PlayerState::Moving)
	{
		startAnimation(PlayerAnimations::Moving);
		m_state = PlayerState::Moving;
	}
	if (m_animations.animationDuration < m_animations.move_startup_01)
		moveDirection = moveDirection * m_speed * 0.5f * Game::getInstance().m_elapsedTimeSeconds;
	else if (m_animations.animationDuration < m_animations.move_startup_02)
		moveDirection = moveDirection * m_speed * 0.75f * Game::getInstance().m_elapsedTimeSeconds;
	else 
		moveDirection = moveDirection * m_speed * 1.0f * Game::getInstance().m_elapsedTimeSeconds;
	glm::vec3 possibleNewPosition = tryMove(moveDirection);
	m_lastPosition = m_position;
	m_position = possibleNewPosition;
}

void Player::rotate()
{
	if (m_rotationAngle == 0.0f && m_facingRight || m_rotationAngle == 180.0f && !m_facingRight)
		return;
	if (m_facingRight)
		m_rotationAngle -= m_rotationSpeed / Game::getInstance().m_framesPerSecond;
	else
		m_rotationAngle += m_rotationSpeed / Game::getInstance().m_framesPerSecond;
	m_rotationAngle = glm::clamp(m_rotationAngle, 0.0f, 180.0f);
}

glm::vec3 Player::tryMove(glm::vec3 move)
{
	return m_position + move;
}

void Player::startAnimation(PlayerAnimations animation)
{
	m_animations.animationDuration = 0.0f;
	if (animation == PlayerAnimations::Idle)
	{
		m_animations.activeAnimation = PlayerAnimations::Idle;
		m_spriteIndex = 0;
		return;
	}
	if (animation == PlayerAnimations::Moving)
	{
		m_animations.activeAnimation = PlayerAnimations::Moving;
		m_spriteIndex = 0;
		return;
	}
}

void Player::updateAnimation()
{
	m_animations.animationDuration += Game::getInstance().m_elapsedTimeSeconds;
	m_animations.animationUpdateTimer += Game::getInstance().m_elapsedTimeSeconds;
	if (m_animations.activeAnimation == PlayerAnimations::Idle)
	{
		if (m_animations.animationUpdateTimer < m_animations.idle_switchSprites)
			m_spriteIndex = 0;
		else if (m_animations.animationUpdateTimer < 2 * m_animations.idle_switchSprites)
			m_spriteIndex = 1;
		else
		{
			m_spriteIndex = 0;
			m_animations.animationUpdateTimer = Game::getInstance().m_elapsedTimeSeconds;
		}
		return;
	}

	if (m_animations.activeAnimation == PlayerAnimations::Moving)
	{
		if (m_animations.animationUpdateTimer < m_animations.move_switchSprites_01)
			m_spriteIndex = 0;
		else if (m_animations.animationUpdateTimer < 2 * m_animations.move_switchSprites_01)
			m_spriteIndex = 2;
		else if (m_animations.animationUpdateTimer < 3 * m_animations.move_switchSprites_01)
			m_spriteIndex = 1;
		else if (m_animations.animationUpdateTimer < 4 * m_animations.move_switchSprites_01)
			m_spriteIndex = 2;
		else
		{
			m_spriteIndex = 0;
			m_animations.animationUpdateTimer = Game::getInstance().m_elapsedTimeSeconds;
		}
		return;
	}
}