#pragma once

#include "Texture.h"
#include <string>


class Texture2D : public Texture
{
  public:
    Texture2D();
    ~Texture2D();

    virtual void pushToGPU(bool deleteAfterPush = false);
};
