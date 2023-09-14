#pragma once

#include "Texture.h"

class FBO
{
  public:
    FBO();
    ~FBO();

    virtual void create();
    virtual void destroy();
    virtual void bind() const;
    virtual void pushToGPU(bool deleteAfterPush = false);

    void bindDefaultFramebuffer() const;
    void attachTextureToColorBuffer(int colorAttachmentNumber, const Texture& texture);

    void attachTextureToDepthBuffer(const Texture& texture);
    void attachTextureToStencilBuffer(const Texture& texture);
    void attachTextureToDepthAndStencilBuffer(const Texture& texture);

  private:
    unsigned int name;

    int colorAtachmentCount;
};
