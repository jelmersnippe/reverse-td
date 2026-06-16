#pragma once

#include "raylib.h"

#include <string>
#include <unordered_map>

inline std::unordered_map<std::string, Texture2D> sprites;
inline std::unordered_map<std::string, Sound> sounds;
inline std::unordered_map<std::string, Shader> shaders;

void load_sprites();
void unload_sprites();

void load_sounds();
void unload_sounds();

void load_shaders();
void unload_shaders();

Texture2D get_sprite(const std::string& sprite_name);
Sound get_sound(const std::string& sound_name);
Shader get_shader(const std::string& shader_name);
