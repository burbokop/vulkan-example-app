#ifndef STRINGVECTOR_H
#define STRINGVECTOR_H

#include <vector>
#include <string>

namespace e172vp {

struct StringVector {
    static std::vector<std::string> mergePair(const std::vector<std::string> &sv0, const std::vector<std::string> &sv1);
    static std::vector<std::string> subtract(const std::vector<std::string> &sv0, const std::vector<std::string> &sv1);

    template<typename T>
    static void fillCStrContainer(const std::vector<std::string> &sv, T& container) {
        container.resize(sv.size());
        for(size_t i = 0; i < sv.size(); ++i) {
            container[i] = sv[i].c_str();
        }
    }

    static std::string toString(const std::vector<std::string> &vector);
};

}
#endif // STRINGVECTOR_H
