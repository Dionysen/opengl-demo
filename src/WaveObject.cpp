#include "WaveObject.h"

WaveObject::WaveObject() {}

WaveObject::~WaveObject()
{
    glDeleteVertexArrays(1, &surfaceVAO);
    glDeleteBuffers(1, &surfaceVBO);
    glDeleteBuffers(1, &EBO);
}

void WaveObject::initialize()
{ // ---------------------------
    // shader
    Shader *shader = new Shader("shaders/surface.vert", "shaders/surface.frag");
    Material *material = new Material();
    material->setShader(0, shader);
    material->setActiveShader(0);
    material->bind();
    setMaterial(material);

    // get camera
    camera = &application->getActiveCamera();

    indexSize = (N - 1) * (M - 1) * 6;
    GLuint *indices = new GLuint[indexSize];

    int p = 0;

    for (int j = 0; j < N - 1; j++)
        for (int i = 0; i < M - 1; i++)
        {
            indices[p++] = i + j * N;
            indices[p++] = (i + 1) + j * N;
            indices[p++] = i + (j + 1) * N;

            indices[p++] = (i + 1) + j * N;
            indices[p++] = (i + 1) + (j + 1) * N;
            indices[p++] = i + (j + 1) * N;
        }

    // Element buffer object
    glGenVertexArrays(1, &surfaceVAO);
    glBindVertexArray(surfaceVAO);
    glGenBuffers(1, &surfaceVBO);
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * sizeof(GLuint), indices,
                 GL_STATIC_DRAW);
    delete[] indices;
    // ----------------
    // init wave
    omega = glm::vec2(0.2, 0.2);
    times = 0;
    modelScale = 0.477;
    seacolor = glm::vec3(0.65, 0.80, 0.95);
    A = 3e-10f;
    V = 60.0f;
    L_x = 10000;
    L_z = 10000;

    wave_model = new Wave(N, M, L_x, L_z, omega, V, A, 1);
    lightPos = glm::vec3(0.0, 400.0, 0.0);
}

void WaveObject::render(int passID)
{
    // IMGUI
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(modelScale));
    static glm::vec3 waveMove = glm::vec3(-670.0f, 0.0f, 1000.0f);
    // Wind strengh
    // float A = 3e-7f;
    // Wind speed
    // float V = 30;
    // Wind direction
    // glm::vec2 omega;
    static float om = 0.1;
    if (ImGui::CollapsingHeader("Wave"))
    {
        ImGui::DragFloat("Wave Scale", &modelScale, 0.001f, 0.01f, 1.0f, "%.3f", 0);
        ImGui::DragFloat3("Wave Position", glm::value_ptr(waveMove), 0.1f, -1000.0f, 1000.0f, "%.3f", 0);
        ImGui::DragFloat3("Light Position", glm::value_ptr(lightPos), 0.1, -1000, 1000, "%.3f", 0);

        if (ImGui::SliderFloat("Wind strengh", &A, 3e-13f, 3e-6f, "%.10f", 0))
            wave_model = new Wave(N, M, L_x, L_z, omega, V, A, 1);
        if (ImGui::SliderFloat("Wind speed", &V, 5, 300, "%.10f", 0))
            wave_model = new Wave(N, M, L_x, L_z, omega, V, A, 1);
        if (ImGui::SliderFloat("Omega", &om, 0.0, 10.0))
        {
            omega = glm::vec2(om, om);
            wave_model = new Wave(N, M, L_x, L_z, omega, V, A, 1);
        }
        if (ImGui::SliderFloat("L_X", &L_x, 0.0, 100000.0))
        {
            L_z = L_x;
            wave_model = new Wave(N, M, L_x, L_z, omega, V, A, 1);
        }
        ImGui::ColorPicker3("Sea Color", glm::value_ptr(seacolor));
    }
    model = glm::translate(model, waveMove);

    times += 0.10;
    buildTessendorfWaveMesh(wave_model);

    getMaterial().useShader();
    getMaterial().getShader(0)->setVec3("light.position", lightPos);
    getMaterial().getShader(0)->setVec3("viewPos", camera->getPosition());
    getMaterial().getShader(0)->setFloat("heightMin", heightMin * modelScale);
    getMaterial().getShader(0)->setFloat("heightMax", heightMax * modelScale);
    // light
    getMaterial().getShader(0)->setVec3("light.ambient", 1.0f, 1.0f, 1.0f);
    getMaterial().getShader(0)->setVec3("light.diffus", 1.0f, 1.0f, 1.0f);
    getMaterial().getShader(0)->setVec3("light.specular", 1.0f, 0.9f, 0.7f);

    getMaterial().getShader(0)->setVec3("skyColor", seacolor);
    getMaterial().getShader(0)->setFloat("material.shininess", 32.0f);

    getMaterial().getShader(0)->setMat4("projection", camera->getProjectionMatrix());
    getMaterial().getShader(0)->setMat4("view", camera->getViewMatrix());
    getMaterial().getShader(0)->setMat4("model", model);

    glBindVertexArray(surfaceVAO);
    glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void WaveObject::buildTessendorfWaveMesh(Wave *wave_model)
{
    int nVertex = N * M;

    wave_model->buildField(times);
    vec3 *heightField = wave_model->heightField;
    vec3 *normalField = wave_model->normalField;

    int p = 0;

    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
        {
            int index = j * N + i;

            if (heightField[index].y > heightMax)
                heightMax = heightField[index].y;
            else if (heightField[index].y < heightMin)
                heightMin = heightField[index].y;
        }

    glBindVertexArray(surfaceVAO);
    glBindBuffer(GL_ARRAY_BUFFER, surfaceVBO);

    int fieldArraySize = sizeof(glm::vec3) * nVertex;
    glBufferData(GL_ARRAY_BUFFER, fieldArraySize * 2, NULL, GL_STATIC_DRAW);

    // Copy height to buffer
    glBufferSubData(GL_ARRAY_BUFFER, 0, fieldArraySize, heightField);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                          (GLvoid *)0);
    glEnableVertexAttribArray(0);

    // Copy normal to buffer
    glBufferSubData(GL_ARRAY_BUFFER, fieldArraySize, fieldArraySize,
                    normalField);
#ifdef __x86_64__
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                          (void *)(long)fieldArraySize);
#else
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                          (void *)fieldArraySize);
#endif
    glEnableVertexAttribArray(1);
}