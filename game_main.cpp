#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "opengl/game.h"

#include <iostream>


int main(){
    printf("starting program \n");
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ScrapeRace", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    Game game = Game(window); //will call graphics init
    game.Start(); 
     // render loop
    // -----------
    printf("starting render loop\n");
    while (!glfwWindowShouldClose(window))
    {
        if (game.state == GAME_MENU){
            game.RenderGameMenu();
        }
        else if (game.state == GAME_LOSE){
            game.RenderGameLose();
        }
        else if (game.state == GAME_WIN){
            game.RenderGameWin();
        }
        else{
            game.ProcessInput(window);
            game.DoWallCollisions();
            game.Render();
        }
    }
}



