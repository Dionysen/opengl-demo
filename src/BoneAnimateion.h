#pragma once
#include "Camera.h"
#include "SceneObject.h"

#include <Animater.h>
#include <AnimationModel.h>
#include <Material.h>
#include <Shader.h>

class BoneAnimation : public SceneObject
{

  public:
    BoneAnimation() {}
    ~BoneAnimation() {}

    virtual void initialize()
    {
        std::cout << "Init the animator ...\n";
        camera = &application->getActiveCamera();
        transform.setPosition(0.0f, 0.0f, 0.0f, WORLD);
        Material* material = new Material();
        setMaterial(material);

        Shader* shader = new Shader("shaders/anim_model.vert", "shaders/anim_model.frag");
        material->setShader(0, shader);
        material->setActiveShader(0);

        model = new AnimationModel("textures/dancing_vampire.dae");

        Animation* danceAnimation = new Animation("textures/dancing_vampire.dae", model);

        animator = new Animator(danceAnimation);

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
        getMaterial().getActiveShader()->setMat4("model", transform.getTransformationMatrix());

        model->Draw(*getMaterial().getActiveShader());
    }

    glm::vec3 lightPos;

    Camera*         camera;
    AnimationModel* model;
    Animator*       animator;
};