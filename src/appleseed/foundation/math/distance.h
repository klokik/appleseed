
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

#ifndef APPLESEED_FOUNDATION_MATH_DISTANCE_H
#define APPLESEED_FOUNDATION_MATH_DISTANCE_H

// appleseed.foundation headers.
#include "foundation/math/aabb.h"
#include "foundation/math/scalar.h"
#include "foundation/math/vector.h"

// Standard headers.
#include <cassert>
#include <cstddef>

namespace foundation
{

//
// Distance functions.
//

// Compute the square distance between two points.
template <typename T, size_t N>
T square_distance(
    const Vector<T, N>& a,
    const Vector<T, N>& b);

// Compute the square distance between a point and a line.
template <typename T, size_t N>
T square_distance_point_line(
    const Vector<T, N>& point,
    const Vector<T, N>& a,          // a point on the line
    const Vector<T, N>& v);         // direction of the line, unit-length

// Compute the square distance between a point and a line segment.
template <typename T, size_t N>
T square_distance_point_segment(
    const Vector<T, N>& point,
    const Vector<T, N>& a,          // one end of the segment
    const Vector<T, N>& b);         // the other end

// Compute the square distance between a point and an AABB.
// Return 0 if the point is inside the AABB.
template <typename T, size_t N>
T square_distance(
    const Vector<T, N>& p,
    const AABB<T, N>&   bbox);
template <typename T, size_t N>
T square_distance(
    const AABB<T, N>&   bbox,
    const Vector<T, N>& p);


//
// Distance functions implementation.
//

template <typename T, size_t N>
inline T square_distance(
    const Vector<T, N>& a,
    const Vector<T, N>& b)
{
    return square_norm(b - a);
}

template <typename T, size_t N>
inline T square_distance_point_line(
    const Vector<T, N>& point,
    const Vector<T, N>& a,
    const Vector<T, N>& v)
{
    assert(is_normalized(v));

    const Vector<T, N> u = point - a;
    const Vector<T, N> d = u - dot(u, v) * v;

    return square_norm(d);
}

template <typename T, size_t N>
inline T square_distance_point_segment(
    const Vector<T, N>& point,
    const Vector<T, N>& a,
    const Vector<T, N>& b)
{
    assert(square_norm(b - a) > T(0.0));

    const Vector<T, N> u = point - a;
    const Vector<T, N> v = b - a;
    const T s = saturate(dot(u, v) / square_norm(v));
    const Vector<T, N> d = u - s * v;

    return square_norm(d);
}

template <typename T, size_t N>
inline T square_distance(
    const Vector<T, N>& p,
    const AABB<T, N>&   bbox)
{
    T d = T(0.0);

    for (size_t i = 0; i < N; ++i)
    {
        // todo: reimplement without branches, measure performances.
        if (p[i] < bbox.min[i])
            d += square(bbox.min[i] - p[i]);
        else if (p[i] > bbox.max[i])
            d += square(bbox.max[i] - p[i]);
    }

    return d;
}

template <typename T, size_t N>
inline T square_distance(
    const AABB<T, N>&   bbox,
    const Vector<T, N>& p)
{
    return square_distance(p, bbox);
}

}       // namespace foundation

#endif  // !APPLESEED_FOUNDATION_MATH_DISTANCE_H
