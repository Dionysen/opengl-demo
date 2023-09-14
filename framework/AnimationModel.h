#ifndef _ANIMATIONMODEL_H_
#define _ANIMATIONMODEL_H_

#include <Model.h>

#include <Animdata.h>
#include <AssimpGlmHelpers.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class AnimationModel : public Model
{
  public:
    AnimationModel(string const& path, bool gamma = false);
    ~AnimationModel();
    inline auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    inline int&  GetBoneCount() { return m_BoneCounter; }

  private:
    std::map<string, BoneInfo> m_BoneInfoMap;

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    void SetVertexBoneDataToDefault(Vertex& vertex);
    void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
    void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

    int m_BoneCounter = 0;
};

#endif  //_ANIMATIONMODEL_H_