#pragma once

#include "raylib.h"

const int SCREEN_WIDTH = 1440;
const int SCREEN_HEIGHT = 1200;
const Vector2 SCREEN_CENTER = {.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2};

const int TARGET_FPS = 60;

const int PLAYER_STARTING_HEALTH = 100;
const int PLAYER_SIZE = 50;

const int BASE_ENEMY_HEALTH = 3;

const int SPAWNER_SIZE = 100;

const int PROJECTILE_SIZE = 10;

const float FIRE_RATE = 150;
const float TIME_BETWEEN_SHOTS = 60 / FIRE_RATE;

const int TOWER_SIZE = 100;
const int TOWER_COST = 10;
const int TOWER_HEALTH = 10;

const int PLAYER_RANGE = 300;
