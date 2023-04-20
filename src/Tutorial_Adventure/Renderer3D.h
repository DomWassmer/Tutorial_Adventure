#pragma once

#include <memory>

#include "Scene.h"

class Renderer3D {
public:
	std::shared_ptr<Scene> m_activeScene;
};