#pragma once

#include "resource.h"
#include "rasterizer.h"
#include"ObjLoader.h"
#include"rasterizer.h"
#include"MyGL.h"

#define KEY_A 0x41
#define KEY_D 0x44
#define KEY_E 0x45
#define KEY_Q 0x51
#define KEY_S 0x53
#define KEY_W 0x57
#define KEY_X 0x58
#define KEY_Z 0x5A
#define ZOOM_IN 1
#define ZOOM_OUT 0

void window_draw_buffer(HWND& hWnd, HDC& hdc, rasterizer& rast);

void handle_key_message(WPARAM virtualKey);

void update_rotationAngle(float& angle, float step);

void update_scaling(float& scale, int operation);