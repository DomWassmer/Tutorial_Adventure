#pragma once

#include <GLFW/glfw3.h>

// GLFW abstraction

enum class MouseButton {
	MouseButtonLeft = GLFW_MOUSE_BUTTON_LEFT,
	MouseButtonRight = GLFW_MOUSE_BUTTON_RIGHT
};

enum class CursorMode {
	CursorModeVisible = GLFW_CURSOR_NORMAL,
	CursorModeHidden = GLFW_CURSOR_HIDDEN,
	CursorModeFixed = GLFW_CURSOR_DISABLED
};

// GLFW Documentation states their keycodes are from the us keyboard.
// Caution is required for keys that change between keyboards
enum class KeyCode {
	Space = GLFW_KEY_SPACE,
	Enter = GLFW_KEY_ENTER,
	Backspace = GLFW_KEY_BACKSPACE,
	ShiftLeft = GLFW_KEY_LEFT_SHIFT,
	ControlLeft = GLFW_KEY_LEFT_CONTROL,
	AltLeft = GLFW_KEY_LEFT_ALT,
	Escape = GLFW_KEY_ESCAPE,
	Tab = GLFW_KEY_TAB,

	Key_0 = GLFW_KEY_0,
	Key_1 = GLFW_KEY_1,
	Key_2 = GLFW_KEY_2,
	Key_3 = GLFW_KEY_3,
	Key_4 = GLFW_KEY_4,
	Key_5 = GLFW_KEY_5,
	Key_6 = GLFW_KEY_6,
	Key_7 = GLFW_KEY_7,
	Key_8 = GLFW_KEY_8,
	Key_9 = GLFW_KEY_9,

	A = GLFW_KEY_A,
	B = GLFW_KEY_B,
	C = GLFW_KEY_C,
	D = GLFW_KEY_D,
	E = GLFW_KEY_E,
	F = GLFW_KEY_F,
	G = GLFW_KEY_G,
	H = GLFW_KEY_H,
	I = GLFW_KEY_I,
	J = GLFW_KEY_J,
	K = GLFW_KEY_K,
	L = GLFW_KEY_L,
	M = GLFW_KEY_M,
	N = GLFW_KEY_N,
	O = GLFW_KEY_O,
	P = GLFW_KEY_P,
	Q = GLFW_KEY_Q,
	R = GLFW_KEY_R,
	S = GLFW_KEY_S,
	T = GLFW_KEY_T,
	U = GLFW_KEY_U,
	V = GLFW_KEY_V,
	W = GLFW_KEY_W,
	X = GLFW_KEY_X,
	Y = GLFW_KEY_Y,
	Z = GLFW_KEY_Z
};