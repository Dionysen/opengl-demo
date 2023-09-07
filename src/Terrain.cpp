#include "Terrain.h"
#include "Application.h"
#include "MyScene.h"
#include "Shader.h"
#include "Texture2D.h"
#include "Texture2DArray.h"

Terrain::Terrain(Sun *sun) : sun(sun) {}

Terrain::~Terrain() {}

void Terrain::initialize()
{

    application->addEventListener(this);

    transform.setPosition(0.0, -47.0, 0.0, WORLD);

    std::cout << "Terrain create shader ...\n";
    shader = new Shader("shaders/terrain.vert", "shaders/terrain.frag",
                        "shaders/terrain.geom", "shaders/terrain.tesc",
                        "shaders/terrain.tese");
    std::cout << "Terrain create shader successed!\n";

    // material 1
    loadMaterial();
    setMaterial(material1);

    glPolygonOffset(7.5f, 15.0f);
    glPatchParameteri(GL_PATCH_VERTICES, 4);

    displacementScale = 150.0f;
    tessellatedTriWidth = 20.0f;
    gridsize = glm::ivec2(64, 64);
    size = glm::vec2(1000.0f, 1000.0f); // 1km
    patchCount = gridsize.x * gridsize.y;

    enableWireFrame = false;

    camera = &application->getActiveCamera();

    bias = glm::mat4(
        glm::vec4(0.5f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 0.5f, 0.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    std::cout << "Terrain initialized!\n";
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glPolygonOffset(7.5f, 15.0f);
}

void Terrain::render(int passID)
{
    // shadow(TO DO)
    // sunProjectionMatrix = glm::ortho(
    //     -size.x / 2.0f, size.x / 2.0f, -size.y / 2.0f, size.y / 2.0f,
    //     sun->DISTANCE - size.x / 2.0f, sun->DISTANCE + size.x / 2.0f);
    // glm::mat4 sunMVP = sunProjectionMatrix * sun->getTransform().getInverseTransformationMatrix();

    int width = application->getWindowWidth();
    int height = application->getWindowHeight();

    static int selectedMaterial = 1;

    // imgui pre
    static glm::vec3 terrainMove = glm::vec3(0.0, -47.0, 0.0);
    transform.setPosition(terrainMove, WORLD);

    static float scale = 2.0f;
    transform.setScale(scale, scale, scale, WORLD);

    // imgui
    if (ImGui::CollapsingHeader("Terrain"))
    {
        ImGui::DragFloat3("Terrain Position", glm::value_ptr(terrainMove), 0.1f, -1000.0f, 1000.0f, "%.3f", 0);

        ImGui::Separator();
        ImGui::Text("Material");
        ImGui::RadioButton("Material1", &selectedMaterial, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Material2", &selectedMaterial, 2);
        ImGui::SameLine();
        ImGui::RadioButton("Material3", &selectedMaterial, 3);

        if (selectedMaterial == 1)
            setMaterial(material1);
        else if (selectedMaterial == 2)
            setMaterial(material2);
        else if (selectedMaterial == 3)
            setMaterial(material3);

        ImGui::Separator();
        ImGui::SliderFloat("Displacement Scale", &displacementScale, 0, 300, "%.3f");
        ImGui::SliderFloat("Tessellated TriWidth", &tessellatedTriWidth, 10, 60, "%.3f");
        ImGui::SliderFloat("Scale", &scale, 1.0f, 50.0f);
    }

    getMaterial().bind();
    // imgui post
    getMaterial().getActiveShader()->setMat4("model", transform.getTransformationMatrix());

    glUniform2i(21, gridsize.x, gridsize.y);
    glUniform1f(24, displacementScale);
    glUniform2f(25, size.x, size.y);
    glUniform1f(22, tessellatedTriWidth);
    getMaterial().getActiveShader()->setBool("enableWireFrame", enableWireFrame);

    glUniformMatrix4fv(11, 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
    glUniformMatrix4fv(13, 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix()));
    glUniform4fv(14, 6, camera->getFrustum().getPackedPlaneData());
    glUniform2i(23, width, height);
    glUniform3fv(30, 1, (GLfloat *)&sun->getDirectionalLight().getDirection());
    glUniform3fv(31, 1, (GLfloat *)&sun->getDirectionalLight().getColor());

    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_PATCHES, 0, 4, patchCount);
}

void Terrain::update(double deltaTime) {}

void Terrain::keyCallback(const int key, const int scanCode, const int action,
                          const int mods)
{
    if (key == GLFW_KEY_O)
    {
        displacementScale -= displacementScale * 0.03f;
    }
    else if (key == GLFW_KEY_P)
    {
        displacementScale += displacementScale * 0.03f;
    }

    else if (key == GLFW_KEY_K)
    {
        tessellatedTriWidth = glm::max(tessellatedTriWidth - 1.0f, 0.0f);
    }
    else if (key == GLFW_KEY_L)
    {
        tessellatedTriWidth += 1.0f;
    }
}

void Terrain::loadMaterial()
{
    std::cout << "Terrain create displacement texture ...\n";
    Texture2D *terrainDisplacement1 = new Texture2D();
    terrainDisplacement1->initFromFile("textures/height16bit1.png", ImageFormat::IMAGE_FORMAT_R16);
    terrainDisplacement1->changeTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    terrainDisplacement1->changeTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    std::cout << "Terrain create displacement texture successed!\n";

    std::cout << "Terrain create normal texture ...\n";
    Texture2D *terrainNormals1 = new Texture2D();
    terrainNormals1->initFromFile("textures/normals1.png", ImageFormat::IMAGE_FORMAT_RGB8_LINEAR);
    terrainNormals1->changeTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    terrainNormals1->changeTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    std::cout << "Terrain create normal texture successed!\n";

    std::cout << "Terrain create texture array ...\n";
    std::vector<std::string> terrainTextures1;
    terrainTextures1.push_back("textures/1.png");
    terrainTextures1.push_back("textures/2.jpg");
    terrainTextures1.push_back("textures/3.jpg");
    terrainTextures1.push_back("textures/4.jpg");
    Texture2DArray *terrainTexturesArray1 = new Texture2DArray();
    terrainTexturesArray1->initFromFiles(terrainTextures1);
    std::cout << "Terrain create texture array successed!\n";

    std::cout << "Terrain create displacement texture ...\n";
    Texture2D *terrainDisplacement2 = new Texture2D();
    terrainDisplacement2->initFromFile("textures/height16bit2.png", ImageFormat::IMAGE_FORMAT_R16);
    terrainDisplacement2->changeTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    terrainDisplacement2->changeTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    std::cout << "Terrain create displacement texture successed!\n";

    std::cout << "Terrain create normal texture ...\n";
    Texture2D *terrainNormals2 = new Texture2D();
    terrainNormals2->initFromFile("textures/normals2.png", ImageFormat::IMAGE_FORMAT_RGB8_LINEAR);
    terrainNormals2->changeTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    terrainNormals2->changeTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    std::cout << "Terrain create normal texture successed!\n";

    std::cout << "Terrain create texture array ...\n";
    std::vector<std::string> terrainTextures2;
    terrainTextures2.push_back("textures/6.jpg");
    terrainTextures2.push_back("textures/2.jpg");
    terrainTextures2.push_back("textures/8.jpg");
    terrainTextures2.push_back("textures/1.png");
    Texture2DArray *terrainTexturesArray2 = new Texture2DArray();
    terrainTexturesArray2->initFromFiles(terrainTextures2);
    std::cout << "Terrain create texture array successed!\n";

    std::cout << "Terrain create displacement texture ...\n";
    Texture2D *terrainDisplacement3 = new Texture2D();
    terrainDisplacement3->initFromFile("textures/height16bit3.png", ImageFormat::IMAGE_FORMAT_R16);
    terrainDisplacement3->changeTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    terrainDisplacement3->changeTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    std::cout << "Terrain create displacement texture successed!\n";

    std::cout << "Terrain create normal texture ...\n";
    Texture2D *terrainNormals3 = new Texture2D();
    terrainNormals3->initFromFile("textures/normals3.png", ImageFormat::IMAGE_FORMAT_RGB8_LINEAR);
    terrainNormals3->changeTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    terrainNormals3->changeTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    std::cout << "Terrain create normal texture successed!\n";

    //  material 1
    material1 = new Material();
    material1->setShader(0, shader);
    material1->setActiveShader(0);
    material1->setTexture(0, terrainDisplacement1);
    material1->setTexture(1, terrainNormals1);
    material1->setTexture(2, terrainTexturesArray1);
    // material 2
    material2 = new Material();
    material2->setShader(0, shader);
    material2->setActiveShader(0);
    material2->setTexture(0, terrainDisplacement2);
    material2->setTexture(1, terrainNormals2);
    material2->setTexture(2, terrainTexturesArray2);
    // material 3
    material3 = new Material();
    material3->setShader(0, shader);
    material3->setActiveShader(0);
    material3->setTexture(0, terrainDisplacement3);
    material3->setTexture(1, terrainNormals3);
    material3->setTexture(2, terrainTexturesArray1);
}
