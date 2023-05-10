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
	if (m_state == PlayerState::Idle || m_state == PlayerState::Moving)
		move();
	rotate();
}

void Player::move() {
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
	if (moveDirection == glm::vec3{ 0.0f, 0.0f, 0.0f })
	{
		/* Deceleration code here */
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
		m_animationFrame = 0;
		m_state = PlayerState::Moving;
	}
	if (true/*m_animationFrame < Game::getInstance().m_settings.framerate / 3*/)
		moveDirection = moveDirection * m_speed * 1.0f / Game::getInstance().m_framesPerSecond;
	else if (m_animationFrame < Game::getInstance().m_settings.framerate / 2)
		moveDirection = moveDirection * m_speed * 0.75f;
	else 
		moveDirection = moveDirection * m_speed * 1.0f;
	glm::vec3 possibleNewPosition = tryMove(moveDirection);
	m_lastPosition = m_position;
	m_position = possibleNewPosition;
	m_animationFrame++;
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