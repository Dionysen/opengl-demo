#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    // bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    // weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct meshTexture
{
    unsigned int id;
    string       type;
    string       path;
};

class Mesh
{
  public:
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<meshTexture>  textures;

    unsigned int VAO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<meshTexture> textures);

    void Draw(Shader& shader);

  private:
    unsigned int VBO, EBO;
    void         setupMesh();
};
#endif