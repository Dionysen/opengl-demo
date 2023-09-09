#pragma once
#include "Camera.h"
#include "SceneObject.h"

#include <Material.h>
#include <Shader.h>
#include <Animater.h>
#include <AnimationModel.h>

class BoneAnimation : public SceneObject
{

public:
    BoneAnimation() {}
    ~BoneAnimation() {} // 删除内存

    virtual void
    initialize()
    {
        std::cout << "Init the animator ...\n";
        transform.setPosition(0.0f, 0.0f, 0.0f, WORLD);
        Material *material = new Material();
        setMaterial(material);

        Shader *shader = new Shader("shaders/anim_model.vert", "shaders/anim_model.frag");
        material->setShader(0, shader);
        material->setActiveShader(0);

        model = new AnimationModel("textures/dancing_vampire.dae");
        Animation *danceAnimation = new Animation("textures/dancing_vampire.dae", model);
        animator = new Animator(danceAnimation);
        camera = &application->getActiveCamera();

        std::cout << "The animator initialized.\n";
    }
    virtual void render(int passID)
    {
        getMaterial().bind();
        animator->UpdateAnimation(application->getDeltaTime());

        getMaterial().getActiveShader()->setMat4("projection", camera->getProjectionMatrix());
        getMaterial().getActiveShader()->setMat4("view", camera->getViewMatrix());

        auto transforms = animator->GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i)
            getMaterial().getActiveShader()->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

        // render the loaded model
        glm::mat4 modelMatric = glm::mat4(1.0f);
        modelMatric = glm::translate(modelMatric, glm::vec3(0.0f, -0.4f, 0.0f)); // translate it down so it's at the center of the scene
        modelMatric = glm::scale(modelMatric, glm::vec3(.5f, .5f, .5f));         // it's a bit too big for our scene, so scale it down
        getMaterial().getActiveShader()->setMat4("model", modelMatric);

        model->Draw(*getMaterial().getActiveShader());
    } // 渲染内容

    glm::vec3 lightPos;

    Camera *camera;
    AnimationModel *model;
    Animator *animator;
};