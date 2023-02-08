#ifndef TERRAIN_H
#define TERRAIN_H

#include <glm/glm.hpp>
#include <vector>
#include <cstdlib>

#include "opengl/shader.h"

const int block_indices[] = {
    0, 1, 4, //front face
    0, 2, 4,
    0, 3, 6, //left face
    0, 2, 6,
    1, 5, 7, //right face
    1, 4, 7,
    5, 7, 6, //back face
    5, 3, 6, 
    0, 1, 5, //bottom face
    0, 3, 5,
    2, 4, 7, //top face
    2, 6, 7
};

const int wall_indices[] = {
    0, 1, 3,
    0, 2, 3
};

class RenderObject{
public:
    unsigned int VAO;
    unsigned long NumVertices;
    glm::vec3 Position; //relative to (startX, startY, startZ)
    glm::vec4 Color;
    RenderObject(unsigned int v, int nv, glm::vec3 p, glm::vec4 c): VAO(v), NumVertices(nv), Position(p), Color(c){}
};

class Block : public RenderObject{
public:
    bool Interactive;
    Block(unsigned int v, glm::vec3 p, glm::vec4 c, bool i) : RenderObject(v, sizeof(block_indices)/sizeof(int), p, c), Interactive(i){}
};

class Wall : public RenderObject {
public:
    Wall(unsigned int v, glm::vec3 p, glm::vec4 c): RenderObject(v, sizeof(wall_indices)/sizeof(int), p, c){}
};

class Terrain {
    public:
        Terrain(float startX, float startY, float startZ,
         float length, float width, float height, float blockSize, int numBlocks);

        std::vector<Wall>* walls;
        std::vector<Block>* blocks;
        
        //VAO is interface with game render loop
        unsigned int w_VAO[3];
        unsigned int b_VAO; 

        void InitVAO(unsigned int* VAO, const float vertices[], unsigned long vertBytes, const int indices[], unsigned long idxBytes);
        void GenerateWalls();
        void GenerateBlocks();
        glm::vec3 terrainOffset;
        glm::vec3 terrainMax;
        glm::vec3 blockVector;

        bool ComputeRayBoxCollision(glm::vec3 boxPos, glm::vec3 camPos, glm::vec3 camDir);
        

    private:
        float startX;
        float startY;
        float startZ;

        float lenX;
        float lenY;
        float lenZ;

        float blockSize;
        int numBlocks;

        float block_vertices[3 * 8];
        void ComputeBlockData(float blockSize);
        float wall_vertices[3][3 * 4];
        void ComputeWallData(float lenX, float lenY, float lenZ);

        
};

bool computeRayBoxCollision(Block block, glm::vec3 camPos, glm::vec3 camDir);

#endif