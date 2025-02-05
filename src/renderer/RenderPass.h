#pragma once

#include "pch.h"

class RenderPass
{
  public:
    virtual ~RenderPass() = default;
    virtual void execute() = 0;
};