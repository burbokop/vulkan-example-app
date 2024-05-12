#ifndef RESOURCES_H
#define RESOURCES_H

#include "e172vp/geometry/Mesh.h"

struct Resources {
    static e172vp::Mesh mesh(const std::string& resource);
};

#endif // RESOURCES_H
