#include "asset_manager.hpp"

#include "raylib.h"

#include <cassert>
#include <filesystem>
#include <format>
#include <iostream>
#include <string>
#include <unordered_map>

void load_sprites() {
    const std::filesystem::path target_path{"res/sprites"};

    try {
        for (std::filesystem::directory_entry const& dir_entry : std::filesystem::directory_iterator{target_path}) {
            if (!std::filesystem::is_regular_file(dir_entry.path())) { continue; }

            const std::string sprite_name = dir_entry.path().stem().string();

            const Image image = LoadImage(dir_entry.path().string().data());
            const Texture2D texture = LoadTextureFromImage(image);
            SetTextureFilter(texture, TEXTURE_FILTER_POINT);
            UnloadImage(image);

            std::cout << "Loaded sprite: " << sprite_name << std::endl;

            sprites.insert(std::make_pair(sprite_name, texture));
        }
    } catch (std::filesystem::filesystem_error const& ex) {
        std::cout << "Error occured during file operation!\n" << ex.what() << std::endl;
    }
}

void unload_sprites() {
    for (const auto& [_, texture] : sprites) {
        UnloadTexture(texture);
    }
}

Texture2D get_sprite(const std::string& sprite_name) {
    const auto sprite = sprites.find(sprite_name);
    assert(sprite != sprites.end() && std::format("Could not find sprite with name {}", sprite_name).c_str());

    return sprite->second;
}

void load_sounds() {
    const std::filesystem::path target_path{"res/sounds"};

    try {
        for (std::filesystem::directory_entry const& dir_entry : std::filesystem::directory_iterator{target_path}) {
            if (!std::filesystem::is_regular_file(dir_entry.path())) { continue; }

            const std::string sound_name = dir_entry.path().stem().string();

            const auto path = dir_entry.path().string();
            const auto path_as_char = path.c_str();
            const Sound sound = LoadSound(path_as_char);

            std::cout << "Loaded sound: " << sound_name << std::endl;

            sounds.insert(std::make_pair(sound_name, sound));
        }
    } catch (std::filesystem::filesystem_error const& ex) {
        std::cout << "Error occured during file operation!\n" << ex.what() << std::endl;
    }
}

void unload_sounds() {
    for (const auto& [_, sound] : sounds) {
        UnloadSound(sound);
    }
}

Sound get_sound(const std::string& sound_name) {
    const auto sound = sounds.find(sound_name);
    assert(sound != sounds.end() && std::format("Could not find sound with name {}", sound_name).c_str());

    return sound->second;
}

void load_shaders() {
    const std::filesystem::path target_path{"res/shaders"};

    try {
        for (std::filesystem::directory_entry const& dir_entry : std::filesystem::directory_iterator{target_path}) {
            if (!std::filesystem::is_regular_file(dir_entry.path())) { continue; }

            const std::string shader_name = dir_entry.path().stem().string();

            const Shader shader = LoadShader(nullptr, dir_entry.path().string().data());

            std::cout << "Loaded shader: " << shader_name << std::endl;

            shaders.insert(std::make_pair(shader_name, shader));
        }
    } catch (std::filesystem::filesystem_error const& ex) {
        std::cout << "Error occured during file operation!\n" << ex.what() << std::endl;
    }
}

void unload_shaders() {
    for (const auto& [_, shader] : shaders) {
        UnloadShader(shader);
    }
}

Shader get_shader(const std::string& shader_name) {
    const auto shader = shaders.find(shader_name);
    assert(shader != shaders.end() && std::format("Could not find shader with name {}", shader_name).c_str());

    return shader->second;
}
