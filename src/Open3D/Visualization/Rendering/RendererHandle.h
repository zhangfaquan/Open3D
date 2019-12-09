// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2019 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace open3d {

namespace visualization {

enum class eEntityType : std::uint16_t {
    None = 0,

    View,
    Scene,

    Geometry,
    Light,
    Camera,
    Material,
    MaterialInstance,
    Texture,

    VertexBuffer,
    IndexBuffer,

    Count
};

// RenderEntityHandle - handle type for entities inside Renderer
// Can be used in STL containers as key
struct REHandle_abstract {
    static const std::uint16_t kBadId = 0;
    const eEntityType type = eEntityType::None;

    inline size_t Hash() const {
        return static_cast<std::uint16_t>(type) << 16 | id;
    }

    bool operator==(const REHandle_abstract& other) const {
        return id == other.id && type == other.type;
    }

    bool operator<(const REHandle_abstract& other) const {
        return Hash() < other.Hash();
    }

    explicit operator bool() const {
        return id != kBadId;
    }

    REHandle_abstract() : type(eEntityType::None), id(kBadId) {}

    std::uint16_t GetId() const { return id; }

protected:
    REHandle_abstract(const eEntityType aType, const std::uint16_t aId)
        : type(aType), id(aId) {}

    static std::array<std::uint16_t, static_cast<size_t>(eEntityType::Count)> uid_table;

    std::uint16_t id = kBadId;
};

std::ostream& operator<<(std::ostream& os, const REHandle_abstract& uid);

// REHandle is used for specification of handle types to prevent
// errors with passing, assigning or comparison of different kinds of handles
template <eEntityType entityType>
struct REHandle : public REHandle_abstract {
    static const REHandle kBad;

    static REHandle Next() {
        const auto index = static_cast<std::uint16_t>(entityType);
        auto id = ++uid_table[index];
        if (id == REHandle_abstract::kBadId) {
            uid_table[index] = REHandle_abstract::kBadId + 1;
            id = REHandle_abstract::kBadId + 1;
        }

        return std::move(REHandle(id));
    }

    static REHandle Concretize(const REHandle_abstract& abstract) {
        if (abstract.type != entityType) {
            // assert("Incompatible render uid types!\n");
            return REHandle();
        }

        return REHandle(abstract.GetId());
    }

    REHandle() : REHandle_abstract(entityType, REHandle_abstract::kBadId) {}
    REHandle(const REHandle& other)
        : REHandle_abstract(entityType, other.id)
    {}

    REHandle& operator=(const REHandle& other){
        id = other.id;
        return *this;
    }

private:
    explicit REHandle(const std::uint16_t aId)
        : REHandle_abstract(entityType, aId) {}
};

template <eEntityType entityType>
const REHandle<entityType> REHandle<entityType>::kBad;

typedef REHandle<eEntityType::View> ViewHandle;
typedef REHandle<eEntityType::Scene> SceneHandle;
typedef REHandle<eEntityType::Geometry> GeometryHandle;
typedef REHandle<eEntityType::Light> LightHandle;
typedef REHandle<eEntityType::Camera> CameraHandle;
typedef REHandle<eEntityType::Material> MaterialHandle;
typedef REHandle<eEntityType::MaterialInstance> MaterialInstanceHandle;
typedef REHandle<eEntityType::Texture> TextureHandle;
typedef REHandle<eEntityType::VertexBuffer> VertexBufferHandle;
typedef REHandle<eEntityType::IndexBuffer> IndexBufferHandle;

}
}

namespace std
{
template<>
class hash<open3d::visualization::REHandle_abstract> {
public:
    size_t operator()(const open3d::visualization::REHandle_abstract &uid) const
    {
        return uid.Hash();
    }
};
}