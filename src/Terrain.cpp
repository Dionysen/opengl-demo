#include "Terrain.h"
#include "Application.h"
#include "MyScene.h"
#include "Shader.h"
#include "Texture2D.h"
#include "Texture2DArray.h"

Terrain::Terrain(Sun *sun) : sun(sun) {}

Terrain::~Terrain() {}

void Terrain::initialize() {

    application->addEventListener(this);

    std::cout << "Terrain create shader ...\n";
    Shader *shader1 = new Shader("shaders/terrain.vert", "shaders/terrain.frag",
                                 "shaders/terrain.geom", "shaders/terrain.tesc",
                                 "shaders/terrain.tese");
    std::cout << "Terrain create shader successed!\n";

    std::cout << "Terrain create displacement texture ...\n";
    Texture2D* terrainDisplacement1 = new Texture2D();
    terrainDisplacement1->initFromFile("textures/height16bit2.png", ImageFormat::IMAGE_FORMAT_R16);
    terrainDisplacement1->changeTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    terrainDisplacement1->changeTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    std::cout << "Terrain create displacement texture successed!\n";

    std::cout << "Terrain create normal texture ...\n";
    Texture2D* terrainNormals1 = new Texture2D();
    terrainNormals1->initFromFile("textures/normals2.png", ImageFormat::IMAGE_FORMAT_RGB8_LINEAR);
    terrainNormals1->changeTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    terrainNormals1->changeTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    std::cout << "Terrain create normal texture successed!\n";

    std::cout << "Terrain create texture array ...\n";
    std::vector<std::string> terrainTextures1;
    terrainTextures1.push_back("textures/6.jpg");
    terrainTextures1.push_back("textures/2.jpg");
    terrainTextures1.push_back("textures/8.jpg");
    terrainTextures1.push_back("textures/1.png");
    Texture2DArray* terrainTexturesArray1 = new Texture2DArray();
    terrainTexturesArray1->initFromFiles(terrainTextures1);
    std::cout << "Terrain create texture array successed!\n";

    Material* material = new Material();
    material->setShader(0, shader1);
    material->setActiveShader(0);
    material->setTexture(0, terrainDisplacement1);
    material->setTexture(1, terrainNormals1);
    material->setTexture(2, terrainTexturesArray1);

    setMaterial(material);

    glPolygonOffset(7.5f, 15.0f);
    glPatchParameteri(GL_PATCH_VERTICES, 4);

    // 设置配置参数
    displacementScale = 150.0f;  // 变形
    tessellatedTriWidth = 20.0f;  // 细分三角形宽度
    gridsize = glm::ivec2(64, 64);  // 网格数量，64*64
    size = glm::vec2(1000.0f, 1000.0f); // 1km
    patchCount = gridsize.x * gridsize.y; // 图元数量=64*64

    enableWireFrame = false;
    // 设置摄像机
    camera = &application->getActiveCamera();

    // 太阳的投影矩阵
    sunProjectionMatrix = glm::ortho(
        -size.x / 2.0f, size.x / 2.0f, -size.y / 2.0f, size.y / 2.0f,
        sun->DISTANCE - size.x / 2.0f, sun->DISTANCE + size.x / 2.0f);

    // 太阳的偏移矩阵
    bias = glm::mat4(
        glm::vec4(0.5f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 0.5f, 0.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    std::cout << "Terrain initialized!\n";
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glPolygonOffset(7.5f, 15.0f);
}

void Terrain::render(int passID) {
    glm::mat4 sunMVP = sunProjectionMatrix * sun->getTransform().getInverseTransformationMatrix();
 
    int width = application->getWindowWidth();
    int height = application->getWindowHeight();
     
    getMaterial().setActiveShader(0);
    getMaterial().bind();

    // imgui pre
    glm::mat4 model = glm::mat4(1.0);
    static glm::vec3 terrainMove = glm::vec3(0.0, -47.0, 0.0);
    model = glm::translate(model, terrainMove);
    // imgui
    if (ImGui::CollapsingHeader("Terrain")) {
        ImGui::DragFloat3("Terrain Position", glm::value_ptr(terrainMove), 0.1f, -1000.0f, 1000.0f, "%.3f", 0);
    }
    // imgui post
    material->getActiveShader()->setMat4("model", model);
   
    glUniform2i(21, gridsize.x, gridsize.y);
    glUniform1f(24, displacementScale);
    glUniform2f(25, size.x, size.y);
    glUniform1f(22, tessellatedTriWidth);
    material->getActiveShader()->setBool("enableWireFrame", enableWireFrame);

    glUniformMatrix4fv(11, 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
    glUniformMatrix4fv(13, 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix()));
    glUniform4fv(14, 6, camera->getFrustum().getPackedPlaneData());
    glUniform2i(23, width, height);
    glUniform3fv(30, 1, (GLfloat*)&sun->getDirectionalLight().getDirection());
    glUniform3fv(31, 1, (GLfloat*)&sun->getDirectionalLight().getColor());

    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_PATCHES, 0, 4, patchCount); 
}

void Terrain::update(double deltaTime) {}

void Terrain::keyCallback(const int key, const int scanCode, const int action,
                          const int mods) {
    if (key == GLFW_KEY_O) {
        displacementScale -= displacementScale * 0.03f;
    }
    else if (key == GLFW_KEY_P) {
        displacementScale += displacementScale * 0.03f;
    }

    else if (key == GLFW_KEY_K) {
        tessellatedTriWidth = glm::max(tessellatedTriWidth - 1.0f, 0.0f);
    }
    else if (key == GLFW_KEY_L) {
        tessellatedTriWidth += 1.0f;
    }
}
