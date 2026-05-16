#pragma once

#include "game_state.hpp"
#include <stack>
#include <string>

struct Scene {
    std::string name;
    void (*init)(GameState&){};
    void (*update)(GameState&){};
    void (*draw)(const GameState&){};
    void (*destroy)(GameState&){};
};

struct SceneManager {
    std::stack<Scene> scenes;

    void PushScene(GameState& state, const Scene& scene);

    void PopScene(GameState& state);

    void Update(GameState& state);

    void Dispose(GameState& state);
};

extern SceneManager SCENE_MANAGER;
