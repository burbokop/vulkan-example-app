#include "stringvector.h"

#include <algorithm>
#include <set>
#include <sstream>

std::vector<std::string> e172vp::StringVector::mergePair(const std::vector<std::string> &sv0, const std::vector<std::string> &sv1) {
    std::vector<std::string> result = sv0;
    for(auto e : sv1) {
        if(std::find(result.begin(), result.end(), e) == result.end()) {
            result.push_back(e);
        }
    }
    return result;
}

std::vector<std::string> e172vp::StringVector::subtract(const std::vector<std::string> &sv0, const std::vector<std::string> &sv1) {
    std::set<std::string> __tmp_set(sv0.begin(), sv0.end());
    for (const auto& extension : sv1) {
        __tmp_set.erase(extension);
    }
    return std::vector<std::string>(__tmp_set.begin(), __tmp_set.end());
}


std::string e172vp::StringVector::toString(const std::vector<std::string> &vector) {
    std::stringstream stream;

    size_t i = 0;
    stream << std::string("[");
    for(auto a : vector) {
        stream << a << ((i == vector.size() - 1) ? std::string() : ", ");
        ++i;
    }
    stream << std::string("]");
    return stream.str();
}
