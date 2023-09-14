#include "SceneObject.h"

SceneObject::SceneObject() {}

SceneObject::~SceneObject() {}

void SceneObject::initialize() {}

void SceneObject::render(int passID)
{
    material->bind();
}

void SceneObject::update(double deltaTime) {}

void SceneObject::bindMaterial() {}
