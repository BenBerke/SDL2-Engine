#pragma once
#include "core/Vector2.h"

const int SCREEN_WIDTH = 960, SCREEN_HEIGHT = 600, PIXELS_PER_UNIT = 32;
const int SCREEN_WIDTH_CENTER = static_cast<int>(SCREEN_WIDTH/2), SCREEN_HEIGHT_CENTRE = static_cast<int>(SCREEN_HEIGHT/2);

const Vector2 DEFAULT_GRAVITY = Vector2{0.0f, -0.2f};
const Vector2 DEFAULT_MAX_VELOCITY = Vector2{50.0f, 50.0f};

const float SPATIAL_GRID_SIZE = 128.0f;
