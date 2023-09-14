#pragma once

#include "EventListener.h"
#include "Material.h"
#include "Transform.h"

class Application;

class SceneObject : public EventListener
{

  public:
    SceneObject();
    virtual ~SceneObject();
    inline void setMaterial(Material* material) { this->material = material; }
    inline void setApplication(Application* app) { application = app; }

    virtual void initialize();
    virtual void render(int passID = 0);
    virtual void update(double deltaTime);

    inline Material&  getMaterial() { return *material; }
    inline Transform& getTransform() { return transform; }

  protected:
    Application* application;
    Material*    material;
    Transform    transform;

    virtual void bindMaterial();
};