
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2010-2013 Francois Beaune, Jupiter Jazz Limited
// Copyright (c) 2014 Francois Beaune, The appleseedhq Organization
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
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

// Interface header.
#include "meshobject.h"

// appleseed.renderer headers.
#include "renderer/kernel/tessellation/statictessellation.h"
#include "renderer/modeling/object/iregion.h"
#include "renderer/modeling/object/triangle.h"

// Standard headers.
#include <cassert>
#include <string>
#include <vector>

using namespace foundation;
using namespace std;

namespace renderer
{

//
// MeshObject class implementation.
//

namespace
{
    // A dummy region that simply wraps a tessellation.
    class MeshRegion
      : public IRegion
    {
      public:
        // Constructor.
        explicit MeshRegion(StaticTriangleTess* tess)
          : m_tess(tess)
          , m_lazy_tess(tess)
        {
        }

        // Compute the local space bounding box of the region over the shutter interval.
        virtual GAABB3 compute_local_bbox() const
        {
            return m_tess->compute_local_bbox();
        }

        // Return the static triangle tessellation of the region.
        virtual Lazy<StaticTriangleTess>& get_static_triangle_tess() const
        {
            return m_lazy_tess;
        }

      private:
        StaticTriangleTess*                 m_tess;
        mutable Lazy<StaticTriangleTess>    m_lazy_tess;
    };
}

struct MeshObject::Impl
{
    StaticTriangleTess          m_tess;
    MeshRegion                  m_region;
    RegionKit                   m_region_kit;
    mutable Lazy<RegionKit>     m_lazy_region_kit;
    vector<string>              m_material_slots;

    Impl()
      : m_region(&m_tess)
      , m_lazy_region_kit(&m_region_kit)
    {
        m_region_kit.push_back(&m_region);
    }
};

MeshObject::MeshObject(
    const char*         name,
    const ParamArray&   params)
  : Object(name, params)
  , impl(new Impl())
{
}

MeshObject::~MeshObject()
{
    delete impl;
}

void MeshObject::release()
{
    delete this;
}

const char* MeshObject::get_model() const
{
    return MeshObjectFactory::get_model();
}

GAABB3 MeshObject::compute_local_bbox() const
{
    return impl->m_tess.compute_local_bbox();
}

Lazy<RegionKit>& MeshObject::get_region_kit()
{
    return impl->m_lazy_region_kit;
}

void MeshObject::reserve_vertices(const size_t count)
{
    impl->m_tess.m_vertices.reserve(count);
}

size_t MeshObject::push_vertex(const GVector3& vertex)
{
    const size_t index = impl->m_tess.m_vertices.size();
    impl->m_tess.m_vertices.push_back(vertex);
    return index;
}

size_t MeshObject::get_vertex_count() const
{
    return impl->m_tess.m_vertices.size();
}

const GVector3& MeshObject::get_vertex(const size_t index) const
{
    return impl->m_tess.m_vertices[index];
}

void MeshObject::reserve_vertex_normals(const size_t count)
{
    impl->m_tess.m_vertex_normals.reserve(count);
}

size_t MeshObject::push_vertex_normal(const GVector3& normal)
{
    assert(is_normalized(normal));

    const size_t index = impl->m_tess.m_vertex_normals.size();
    impl->m_tess.m_vertex_normals.push_back(normal);
    return index;
}

size_t MeshObject::get_vertex_normal_count() const
{
    return impl->m_tess.m_vertex_normals.size();
}

const GVector3& MeshObject::get_vertex_normal(const size_t index) const
{
    return impl->m_tess.m_vertex_normals[index];
}

size_t MeshObject::push_tex_coords(const GVector2& tex_coords)
{
    return impl->m_tess.push_uv_vertex(tex_coords);
}

size_t MeshObject::get_tex_coords_count() const
{
    return impl->m_tess.get_uv_vertex_count();
}

GVector2 MeshObject::get_tex_coords(const size_t index) const
{
    return impl->m_tess.get_uv_vertex(index);
}

void MeshObject::reserve_triangles(const size_t count)
{
    impl->m_tess.m_primitives.reserve(count);
}

size_t MeshObject::push_triangle(const Triangle& triangle)
{
    const size_t index = impl->m_tess.m_primitives.size();
    impl->m_tess.m_primitives.push_back(triangle);
    return index;
}

size_t MeshObject::get_triangle_count() const
{
    return impl->m_tess.m_primitives.size();
}

const Triangle& MeshObject::get_triangle(const size_t index) const
{
    return impl->m_tess.m_primitives[index];
}

void MeshObject::set_motion_segment_count(const size_t count)
{
    impl->m_tess.set_motion_segment_count(count);
}

size_t MeshObject::get_motion_segment_count() const
{
    return impl->m_tess.get_motion_segment_count();
}

void MeshObject::set_vertex_pose(
    const size_t        vertex_index,
    const size_t        motion_segment_index,
    const GVector3&     v)
{
    impl->m_tess.set_vertex_pose(vertex_index, motion_segment_index, v);
}

GVector3 MeshObject::get_vertex_pose(
    const size_t        vertex_index,
    const size_t        motion_segment_index) const
{
    return impl->m_tess.get_vertex_pose(vertex_index, motion_segment_index);
}

void MeshObject::clear_vertex_poses()
{
    impl->m_tess.clear_vertex_poses();
}

void MeshObject::reserve_material_slots(const size_t count)
{
    impl->m_material_slots.reserve(count);
}

size_t MeshObject::push_material_slot(const char* name)
{
    const size_t index = impl->m_material_slots.size();
    impl->m_material_slots.push_back(name);
    return index;
}

size_t MeshObject::get_material_slot_count() const
{
    return impl->m_material_slots.size();
}

const char* MeshObject::get_material_slot(const size_t index) const
{
    return impl->m_material_slots[index].c_str();
}


//
// MeshObjectFactory class implementation.
//

const char* MeshObjectFactory::get_model()
{
    return "mesh_object";
}

auto_release_ptr<MeshObject> MeshObjectFactory::create(
    const char*         name,
    const ParamArray&   params)
{
    return
        auto_release_ptr<MeshObject>(
            new MeshObject(name, params));
}

}   // namespace renderer
