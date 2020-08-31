#ifndef RESOURCES_H
#define RESOURCES_H


#include "e172vp/vertex.h"


struct Resources {
    static std::vector<e172vp::Vertex> vertices(const std::string& resource);
    static std::vector<uint32_t> indices(const std::string& resource);
};

#endif // RESOURCES_H
