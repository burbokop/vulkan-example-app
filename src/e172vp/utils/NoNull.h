#pragma once

#include <cassert>
#include <memory>
#include <utility>

namespace e172vp {

/// Can only be null when moved
template<typename T>
class NoNull {
public:
    NoNull() = delete;

    NoNull(T v)
        : m_v(std::move(v))
    {
        assert(m_v);
    }

    NoNull(const NoNull&) = default;
    NoNull& operator=(const NoNull& o) = default;

    NoNull(NoNull&& o)
        : m_v(std::exchange(o.m_v, nullptr))
    {
        assert(m_v);
    }

    NoNull& operator=(NoNull&& o)
    {
        m_v = std::exchange(o.m_v, nullptr);
        assert(m_v);
        return *this;
    }

    auto operator->()
    {
        assert(m_v);
        return m_v;
    }

    const auto operator->() const
    {
        assert(m_v);
        return m_v;
    }

    auto operator*()
    {
        assert(m_v);
        return *m_v;
    }

    const auto operator*() const
    {
        assert(m_v);
        return *m_v;
    }

    const auto& toNullable() const
    {
        assert(m_v);
        return m_v;
    }

private:
    T m_v;
};

template<typename T>
using Unique = NoNull<std::unique_ptr<T>>;

template<typename T>
using Shared = NoNull<std::shared_ptr<T>>;

template<typename T>
using RawPtr = NoNull<T*>;

}
