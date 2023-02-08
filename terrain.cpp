#include "opengl/terrain.h"

Terrain::Terrain(float sX, float sY, float sZ, float l, float w, float h, float bs, int nb){
    startX = sX;
    startY = sY;
    startZ = sZ; 
    lenX = l;
    lenY = w;    
    lenZ = h;
    blockSize = bs;
    numBlocks = nb;

    //precompute useful vectors
    terrainOffset = glm::vec3(startX, startY, startZ);
    terrainMax = glm::vec3(startX + lenX, startY + lenY, startZ + lenZ);
    blockVector = glm::vec3(blockSize);

    //init vertex data and bind to correct VAO objects
    ComputeBlockData(blockSize);
    InitVAO(&b_VAO, block_vertices, sizeof(block_vertices), block_indices, sizeof(block_indices));
    ComputeWallData(lenX, lenY, lenZ); 
    for (int i = 0; i < 3; i++){
        InitVAO(&w_VAO[i], wall_vertices[i], sizeof(wall_vertices[i]), wall_indices, sizeof(wall_indices));
    }

    //create game objects and link to VAO objects
    blocks = new std::vector<Block>;
    GenerateBlocks();
    walls = new std::vector<Wall>;
    GenerateWalls();

}

void Terrain::InitVAO(unsigned int *VAO,  //I think this will work
                      const float vertices[], 
                      unsigned long vertBytes, 
                      const int indices[],
                      unsigned long idxBytes){ 
    unsigned int VBO, EBO;
    
    glGenVertexArrays(1, VAO);
    glBindVertexArray(*VAO);

    glGenBuffers(1, &VBO); //buffer for data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertBytes, vertices, GL_STATIC_DRAW); //bound to VAO

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO); //buffer for indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxBytes, indices, GL_STATIC_DRAW);

    glBindVertexArray(0); //we have finished setting our VAO up
}

void Terrain::ComputeBlockData(float bs){
    float cube_vertices[] = { 
        0.0f, 0.0f, 0.0f, //0
        bs,   0.0f, 0.0f, //1
        0.0f, bs,   0.0f, //2
        0.0f, 0.0f, bs, //3
        bs,   bs,   0.0f, //4
        bs,   0.0f, bs, //5
        0.0f, bs,   bs, //6
        bs,   bs,   bs  //7
    };
    memcpy(block_vertices, cube_vertices, sizeof(block_vertices));
}


void Terrain::ComputeWallData(float lenX, float lenY, float lenZ){
    float leftWall_vertices[] = {
        0.0f, 0.0f, 0.0f,
        0.0f, lenY, 0.0f,
        0.0f, 0.0f, lenZ,
        0.0f, lenY, lenZ
    };
    memcpy(wall_vertices[0], leftWall_vertices, sizeof(wall_vertices[0]));
    float botWall_vertices[] = {
        0.0f, 0.0f, 0.0f,
        lenX, 0.0f, 0.0f,
        0.0f, 0.0f, lenZ,
        lenX, 0.0f, lenZ
    };
    memcpy(wall_vertices[1], botWall_vertices, sizeof(wall_vertices[1]));
    float frontWall_vertices[] = {
        0.0f, 0.0f, 0.0f,
        lenX, 0.0f, 0.0f,
        0.0f, lenY, 0.0f,
        lenX, lenY, 0.0f
    };
    memcpy(wall_vertices[2], frontWall_vertices, sizeof(wall_vertices[2]));
}

float randCoord(float max, float offset){
    max -= offset; //keeps blocks from sticking out
    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    return r * max; 
}

void Terrain::GenerateBlocks(){ //places the blocks in the world
    blocks->clear();
    for (int i = 0; i < numBlocks; i++){
        glm::vec3 pos = glm::vec3(randCoord(lenX, blockSize), randCoord(lenY, blockSize), randCoord(lenZ, blockSize));
        glm::vec4 color = glm::vec4(1.0f); //white
        bool interactive = false;
        if (i % 5 == 0){
            color = glm::vec4(0.96, 0.57, 0.04, 1.0); //orange
            interactive = true;
        }
        blocks->push_back(Block(b_VAO, pos, color, interactive));
    }
    printf("terrain generated with a size of: %lu \n", blocks->size());
}

void Terrain::GenerateWalls(){
    walls->clear();
    Wall leftWall = Wall(w_VAO[0], glm::vec3(0.0f), glm::vec4(0.01, 0.04, 0.54, 1.0));
    walls->push_back(leftWall);
    Wall rightWall = Wall(w_VAO[0], glm::vec3(lenX, 0.0f, 0.0f), glm::vec4(0.01, 0.04, 0.54, 1.0));
    walls->push_back(rightWall);
    Wall botWall = Wall(w_VAO[1], glm::vec3(0.0f), glm::vec4(0.01, 0.04, 0.54, 1.0));
    walls->push_back(botWall);
    Wall topWall = Wall(w_VAO[1], glm::vec3(0.0f, lenY, 0.0f), glm::vec4(0.01, 0.04, 0.54, 1.0));
    walls->push_back(topWall);
    // Wall frontWall = Wall(w_VAO[2], glm::vec3(0.0f), glm::vec4(0.01, 0.04, 0.54, 1.0));
    Wall backWall = Wall(w_VAO[2], glm::vec3(0.0f, 0.0f, lenZ), glm::vec4(0.09, 0.79, 0.02, 1.0));
    walls->push_back(backWall);
}

bool Terrain::ComputeRayBoxCollision(glm::vec3 boxPos, glm::vec3 camPos, glm::vec3 invCamDir){
    glm::vec3 boxMin = boxPos;
    glm::vec3 boxMax = boxPos + blockVector;

    float tmin = 0.0, tmax = INFINITY;

    float t1 = (boxMin.x - camPos.x) * invCamDir.x;
    float t2 = (boxMax.x - camPos.x) * invCamDir.x;

    tmin = std::max(tmin, std::min(std::min(t1, t2), tmax)); //accounts for possible nans
    tmax = std::min(tmax, std::max(std::max(t1, t2), tmin));

    t1 = (boxMin.y - camPos.y) * invCamDir.y;
    t2 = (boxMax.y - camPos.y) * invCamDir.y;

    tmin = std::max(tmin, std::min(std::min(t1, t2), tmax));
    tmax = std::min(tmax, std::max(std::max(t1, t2), tmin));

    t1 = (boxMin.z - camPos.z) * invCamDir.z;
    t2 = (boxMax.z - camPos.z) * invCamDir.z;

    tmin = std::max(tmin, std::min(std::min(t1, t2), tmax));
    tmax = std::min(tmax, std::max(std::max(t1, t2), tmin));
    
    return tmin < tmax;
}
