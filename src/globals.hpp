#pragma once

#include "core/data.hpp"

const int DEFAULT_SPRITE_SIZE = 16;

const int SCREEN_WIDTH = 1440;
const int SCREEN_HEIGHT = 1200;
const Vec2F SCREEN_SIZE = {.x = SCREEN_WIDTH, .y = SCREEN_HEIGHT};
const Vec2F SCREEN_CENTER = SCREEN_SIZE * 0.5f;

const int TARGET_FPS = 60;

const int PLAYER_STARTING_HEALTH = 100;
const int PLAYER_SIZE = 48;

const int BASE_ENEMY_HEALTH = 3;

const int SPAWNER_SCALE = 4;
const int SPAWNER_SIZE = DEFAULT_SPRITE_SIZE * SPAWNER_SCALE;

const int PROJECTILE_SIZE = 10;

const float FIRE_RATE = 150;
const float TIME_BETWEEN_SHOTS = 60 / FIRE_RATE;

const int TOWER_SCALE = 4;
const int TOWER_SIZE = DEFAULT_SPRITE_SIZE * TOWER_SCALE;
const int TOWER_COST = 10;
const int TOWER_HEALTH = 10;

const int PLAYER_RANGE = 300;
