#pragma once

#include "../misc/noncopyable.h"
#include "../texture.h"
#include "mesh.h"
#include "../shader.h"
#include "../transform.h"


#include <assimp/importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <mutex>

#include "SOIL2.h"

#include <omp.h> // Include OpenMP header

#include <format>
#include <chrono>
#include <future>


namespace engine
{
    
}