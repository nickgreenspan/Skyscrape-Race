#include "opengl/game.h"

GLFWwindow      *window;
Shader          *shader;
Camera          *camera;
Player          *player;
Terrain         *terrain;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;

Game::Game(GLFWwindow *win){
    window = win;
    if (this->GraphicsInit() != 1) {
        throw std::runtime_error("openGL graphics init failed");
    }
    printf("initializing shader \n");
    shader = new Shader("dependencies/shaders/coords.vs", "dependencies/shaders/coords.fs");
    printf("initializing camera \n");
    camera = new Camera(START_POS);
    printf("initializing player \n");
    player = new Player();
    printf("initializing terrain \n");
    terrain = new Terrain(BUILD_X, BUILD_Y, BUILD_Z, BUILD_LENGTH, BUILD_WIDTH, BUILD_HEIGHT, blockSize, numBlocks);
}

Game::~Game(){
    delete shader;
    delete camera;
    delete player;
    delete terrain;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

int Game::GraphicsInit(){
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback); 

    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST); 
    return 1;
}

void Game::Start() {
    state = GAME_ACTIVE;
}

void Game::RenderGameMenu(){

}

void Game::RenderGameLose(){
    printf("Game Over!\n");
    exit(0);
}

void Game::RenderGameWin(){
    printf("You win!\n");
    exit(0);
}

void Game::RenderObjects(
    std::vector<RenderObject>::iterator obsStart, 
    std::vector<RenderObject>::iterator obsEnd){
    unsigned int currVAO = 0;
    std::vector<RenderObject>::iterator curr;
    for(curr = obsStart; curr != obsEnd; curr++){
        if (curr->VAO != currVAO){
            currVAO = curr->VAO;
            glBindVertexArray(currVAO);
        }
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, terrain->terrainOffset);
        model = glm::translate(model, curr->Position);
        shader->setMat4("model", model);
        shader->setVec4("aColor", curr->Color);

        glDrawElements(GL_TRIANGLES, curr->NumVertices, GL_UNSIGNED_INT, (void*)0);
    }
    glBindVertexArray(0); //unbind
}

void Game::Render(){
    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    shader->use();
    
    //get view matrix from camera
    glm::mat4 view = camera->GetViewMatrix();
    shader->setMat4("view", view);

    //keep projection matrix fixed for now
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shader->setMat4("projection", projection);
    
    RenderObjects(terrain->walls->begin(), terrain->walls->end());
    RenderObjects(terrain->blocks->begin(), terrain->blocks->end());

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Game::CheckClickHit(){
    glm::vec3 camPos = camera->Position;
    glm::vec3 invCamDir = camera->ComputeInverseVector(camera->Front);

    //loop through terrain boxes and check for intersection
    bool hit = false;
    glm::vec3 closestHit = glm::vec3(0.0f);
    float closestHitDist = INFINITY;
    glm::vec3 blockPos;
    for (auto curr = terrain->blocks->begin(); curr != terrain->blocks->end(); curr++){
        blockPos = curr->Position + terrain->terrainOffset;
        if (glm::distance(camPos, blockPos) < closestHitDist){  //if block in question is further away then current closest hit, it can be ignored
            if(terrain->ComputeRayBoxCollision(blockPos, camPos, invCamDir)){
                closestHitDist = glm::distance(camPos, blockPos);
                closestHit = blockPos + terrain->blockVector; //might need to worry about rendering within the block
                hit = true;
            }
        }
    }
    if (hit){
        printf("hit!\n");
        player->clicksLeft--;
        camera->SetPos(closestHit);
    } else{
        printf("no hit\n");
    }
}

void Game::DoWallCollisions(){
    glm::vec3 cPos = camera->Position;
    glm::vec3 terrainMin = terrain->terrainOffset;
    glm::vec3 terrainMax = terrain->terrainMax;
    if(cPos.x < terrainMin.x || cPos.y < terrainMin.y || cPos.x > terrainMax.x || cPos.y > terrainMax.y){
        printf("OUT OF BOUNDS!\n");
        player->lives--;
        if (player->lives == 0){
            state = GAME_LOSE;
        }
        camera->Reset();
    }
    if(cPos.z > terrainMax.z){
        state = GAME_WIN;
    } 
}

void Game::ProcessInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        camera->Reset();
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        printf("player position: (%f, %f, %f) \n", camera->Position.x, camera->Position.y, camera->Position.z);
    
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
        CheckClickHit();
    }
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
  
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    
    lastX = xpos;
    lastY = ypos;

    camera->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

