#ifndef GAME_H
#define GAME_H

#include "opengl/camera.h"
#include "opengl/terrain.h"
#include "opengl/player.h"
#include "opengl/shader.h"
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN,
    GAME_LOSE
};

//define screen parameters
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

constexpr glm::vec3 START_POS = glm::vec3(3.0f, 3.0f, -1.0f);

//define game parameters
#define BUILD_X 2.0
#define BUILD_Y 2.0
#define BUILD_Z 2.0

#define BUILD_LENGTH 3.0
#define BUILD_WIDTH 3.0
#define BUILD_HEIGHT 20.0

const float blockSize = 0.2f;
const int numBlocks = 300;

class Game {
public:
    GameState       state;
    Game(GLFWwindow *window);
    ~Game();
    
    void Start();
    int GraphicsInit();
    void ProcessInput(GLFWwindow *window);
     // game loop
    void RenderObjects(std::vector<RenderObject>::iterator obsStart, 
                       std::vector<RenderObject>::iterator obsEnd);
    void Render();
    
    void DoWallCollisions();
    void CheckClickHit();

    void RenderGameMenu();
    void RenderGameLose();
    void RenderGameWin();  
};

#endif