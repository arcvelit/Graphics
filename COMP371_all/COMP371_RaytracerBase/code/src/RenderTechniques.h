#ifndef RENDERTECHNIQUES_H
#define RENDERTECHNIQUES_H

#include "Types.h"
#include "OutputInfo.h"
#include "SceneInfo.h"

void renderSceneLocal(Buffer& buffer, SceneInfo& scene, OutputInfo& output);
void renderSceneGlobal(Buffer& buffer, SceneInfo& scene, OutputInfo& output);


#endif