#ifndef __LIGHTMANAGER_H__
#define __LIGHTMANAGER_H__
#include "graphics/Light.h"

namespace sx
{
  class LightManager
  {
     std::vector<Light> lights;
    public:
    friend class Application;
  };
}

#endif // __LIGHTMANAGER_H__