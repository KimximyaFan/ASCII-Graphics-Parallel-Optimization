#pragma once

#include <memory>
#include "scene/entity.h"
#include "geometry/vertex.h"
#include "material/material.h"
#include "math/vector.h"
#include "material/texture.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <vector>
#include <cmath>
#include <cstdio>   // printf
#include <limits>   // std::nextafter

std::shared_ptr<Entity> CreateCubeEntity_Flat24_Fixed(float size, float u, float v);
std::shared_ptr<Entity> CreateCubeEntity_Flat24(float);
std::shared_ptr<Entity> CreateCubeEntity_Flat24_Debug(float);

