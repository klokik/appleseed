
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

#ifndef APPLESEED_RENDERER_MODELING_BSDF_SPECULARBTDF_H
#define APPLESEED_RENDERER_MODELING_BSDF_SPECULARBTDF_H

// appleseed.renderer headers.
#include "renderer/global/globaltypes.h"
#include "renderer/modeling/bsdf/ibsdffactory.h"
#include "renderer/modeling/input/inputarray.h"

// appleseed.foundation headers.
#include "foundation/platform/compiler.h"
#include "foundation/utility/autoreleaseptr.h"

// appleseed.main headers.
#include "main/dllsymbol.h"

// Forward declarations.
namespace foundation    { class DictionaryArray; }
namespace renderer      { class BSDF; }
namespace renderer      { class ParamArray; }

namespace renderer
{

//
// Specular BTDF input values.
//

DECLARE_INPUT_VALUES(SpecularBTDFInputValues)
{
    Spectrum    m_reflectance;                  // specular reflectance
    Alpha       m_reflectance_alpha;            // unused
    double      m_reflectance_multiplier;       // specular reflectance multiplier
    Spectrum    m_transmittance;                // specular transmittance
    Alpha       m_transmittance_alpha;          // unused
    double      m_transmittance_multiplier;     // specular transmittance multiplier
    double      m_from_ior;                     // from this index of refraction
    double      m_to_ior;                       // to this index of refraction
};


//
// Specular BTDF factory.
//

class DLLSYMBOL SpecularBTDFFactory
  : public IBSDFFactory
{
  public:
    // Return a string identifying this BSDF model.
    virtual const char* get_model() const OVERRIDE;

    // Return a human-readable string identifying this BSDF model.
    virtual const char* get_human_readable_model() const OVERRIDE;

    // Return a set of input metadata for this BSDF model.
    virtual foundation::DictionaryArray get_input_metadata() const OVERRIDE;

    // Create a new BSDF instance.
    virtual foundation::auto_release_ptr<BSDF> create(
        const char*         name,
        const ParamArray&   params) const OVERRIDE;
};

}       // namespace renderer

#endif  // !APPLESEED_RENDERER_MODELING_BSDF_SPECULARBTDF_H
