#include "core/asset_manager.hpp"
#include "core/input.hpp"
#include "core/key.hpp"
#include "core/key_maps.hpp"
#include "game_state.hpp"
#include "raylib.h"
#include "scenes/main_menu_scene.hpp"
#include "systems/scene_manager.hpp"
#include "systems/targeting.hpp"

#include "globals.hpp"
#include "systems/enemy_system.hpp"
#include "systems/projectile_system.hpp"
#include "systems/spawner_system.hpp"
#include "systems/tower_system.hpp"
#include <iostream>

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Reverse Tiddy");
    SetExitKey(KEY_NULL);
    SetTargetFPS(TARGET_FPS);
    InitAudioDevice();

    load_sprites();
    load_sounds();

    KeyMaps::init();

    GameState state = {};

    SCENE_MANAGER.PushScene(state, MAIN_MENU_SCENE);

    while (!state.should_exit && !WindowShouldClose()) {
        for (size_t i = 0; i < input_frame.state.key_pressed.size(); i++) {
            if (input_frame.state.key_pressed[i]) std::cout << "Pressed: " << key_to_string((Key)i) << std::endl;
        }
        for (size_t i = 0; i < input_frame.state.mouse_pressed.size(); i++) {
            if (input_frame.state.mouse_pressed[i]) std::cout << "Pressed: " << mouse_to_string((Mouse)i) << std::endl;
        }

        input_frame.update();

        SCENE_MANAGER.Update(state);
    }

    SCENE_MANAGER.Clear(state);

    CloseAudioDevice();

    unload_sounds();
    unload_sprites();

    CloseWindow();

    return 0;
}
