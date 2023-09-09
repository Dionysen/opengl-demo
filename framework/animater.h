#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <Animation.h>
#include <Animdata.h>
#include <Bone.h>

class Animator
{
public:
    Animator(Animation *animation);

    void UpdateAnimation(float dt);

    void PlayAnimation(Animation *pAnimation);
    void CalculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform);

    inline std::vector<glm::mat4> GetFinalBoneMatrices()
    {
        return m_FinalBoneMatrices;
    }

private:
    std::vector<glm::mat4> m_FinalBoneMatrices;
    Animation *m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;
};