#pragma once

#include "KeyCodes.h"

class Input {
public:
	[[nodiscard]] static bool isMouseButtonDown(MouseButton mouseButton);
	[[nodiscard]] static bool isKeyDown(KeyCode keyCode);
};