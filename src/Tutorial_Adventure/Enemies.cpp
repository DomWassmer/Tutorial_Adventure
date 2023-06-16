#include "Enemies.h"

std::shared_ptr<Enemy> NPCFactory::createSkeleton(glm::vec3 position)
{
	std::shared_ptr<Enemy> skeleton;
	skeleton->m_name = "Skeleton";
	skeleton->m_position = position;
	skeleton->m_lastPosition = position;

	return skeleton;
}