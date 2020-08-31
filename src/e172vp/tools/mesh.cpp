
#include <iostream>
#include <string.h>

#include "mesh.h"

e172vp::Mesh::Mesh() {

}

e172vp::Mesh e172vp::Mesh::load(const char *path) {
    Mesh result;

    FILE *file = fopen(path, "r");
    if(file == NULL) {
        return Mesh();
    }

    while(true) {
        std::string line;
        if (fscanf(file, "%s", line.data()) == EOF) break;
        else if (strcmp(line.c_str(), "v" ) == 0) {
            float x = 0;
            float y = 0;
            float z = 0;
            fscanf(file, "%f %f %f\n", &x, &y, &z);
            result.vertices.push_back({ x, y, z });
        }
        else if (strcmp(line.c_str(), "vt") == 0){
            float x = 0;
            float y = 0;
            fscanf(file, "%f %f\n", &x, &y);
            result.uvMap.push_back({ x, y });
        }

        else if (strcmp(line.c_str(), "vn") == 0){
            float x = 0;
            float y = 0;
            float z = 0;
            fscanf(file, "%f %f %f\n", &x, &y, &z);
            result.normals.push_back({ x, y, z });
        }
        else if (strcmp(line.c_str(), "f" ) == 0){
            uint32_t iv1x = 0;
            uint32_t iv1y = 0;
            uint32_t iv1z = 0;
            uint32_t iv2x = 0;
            uint32_t iv2y = 0;
            uint32_t iv2z = 0;
            uint32_t iv3x = 0;
            uint32_t iv3y = 0;
            uint32_t iv3z = 0;
            fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &iv1x, &iv1y, &iv1z, &iv2x, &iv2y, &iv2z, &iv3x, &iv3y, &iv3z);

            result.vertexIndices.push_back(iv1x - 1);
            result.vertexIndices.push_back(iv2x - 1);
            result.vertexIndices.push_back(iv3x - 1);

            result.uvIndices.push_back(iv1y - 1);
            result.uvIndices.push_back(iv2y - 1);
            result.uvIndices.push_back(iv3y - 1);

            result.normalIndices.push_back(iv1z - 1);
            result.normalIndices.push_back(iv2z - 1);
            result.normalIndices.push_back(iv3z - 1);
        }
    }
    return result;
}
