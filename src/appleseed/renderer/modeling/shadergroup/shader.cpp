
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2014 Esteban Tovagliari, The appleseedhq Organization
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
#include "shader.h"

// appleseed.renderer headers.
#include "renderer/global/globallogger.h"
#include "renderer/modeling/shadergroup/shaderparamparser.h"
#include "renderer/utility/paramarray.h"

// appleseed.foundation headers.
#include "foundation/utility/foreach.h"
#include "foundation/utility/searchpaths.h"
#include "foundation/utility/string.h"
#include "foundation/utility/uid.h"

// Standard headers.
#include <cassert>
#include <fstream>
#include <string>

using namespace foundation;
using namespace std;

namespace renderer
{

//
// Shader class implementation.
//

namespace
{
    const UniqueID g_class_uid = new_guid();
}

struct Shader::Impl
{
    Impl(
        const char*         type,
        const char*         shader,
        const char*         layer,
        const ParamArray&   params)
      : m_type(type)
      , m_shader(shader)
    {
        for (const_each<StringDictionary> i = params.strings(); i; ++i)
        {
            try
            {
                ShaderParamParser parser(i.it().value());

                switch (parser.param_type())
                {
                  case OSLParamTypeColor:
                    {
                        float r, g, b;
                        parser.parse_three_values<float>(r, g, b, true);
                        m_params.insert(ShaderParam::create_color_param(i.it().name(), r, g, b));
                    }
                    break;

                  case OSLParamTypeFloat:
                    {
                        const float val = parser.parse_one_value<float>();
                        m_params.insert(ShaderParam::create_float_param(i.it().name(), val));
                    }
                    break;

                  case OSLParamTypeInt:
                    {
                        const int val = parser.parse_one_value<int>();
                        m_params.insert(ShaderParam::create_int_param(i.it().name(), val));
                    }
                    break;

                  case OSLParamTypeNormal:
                    {
                        float x, y, z;
                        parser.parse_three_values<float>(x, y, z);
                        m_params.insert(ShaderParam::create_normal_param(i.it().name(), x, y, z));
                    }
                    break;

                  case OSLParamTypePoint:
                    {
                        float x, y, z;
                        parser.parse_three_values<float>(x, y, z);
                        m_params.insert(ShaderParam::create_point_param(i.it().name(), x, y, z));
                    }
                    break;

                  case OSLParamTypeString:
                    {
                        m_params.insert(
                            ShaderParam::create_string_param(
                                i.it().name(),
                                parser.parse_string_value().c_str()));
                    }
                    break;

                  case OSLParamTypeVector:
                    {
                        float x, y, z;
                        parser.parse_three_values<float>(x, y, z);
                        m_params.insert(ShaderParam::create_vector_param(i.it().name(), x, y, z));
                    }
                    break;

                  default:
                    RENDERER_LOG_ERROR(
                        "error adding osl param %s, of unknown type %s; will use the default value.",
                        i.it().name(),
                        i.it().value());
                }
            }
            catch (const ExceptionOSLParamParseError&)
            {
                RENDERER_LOG_ERROR(
                    "error parsing osl param value, param = %s, value = %s; will use the default value.",
                    i.it().name(),
                    i.it().value());
            }

            RENDERER_LOG_DEBUG("added osl param %s.", i.it().name());
        }
    }

    string                  m_type;
    string                  m_shader;
    ShaderParamContainer    m_params;
};

Shader::Shader(
    const char*          type,
    const char*          shader,
    const char*          layer,
    const ParamArray&    params)
  : Entity(g_class_uid, params)
  , impl(new Impl(type, shader, layer, params))
{
    // We use the layer name as the Entity name, as it's unique.
    set_name(layer);
}

Shader::~Shader()
{
    delete impl;
}

void Shader::release()
{
    delete this;
}

const char* Shader::get_type() const
{
    return impl->m_type.c_str();
}

const char* Shader::get_shader() const
{
    return impl->m_shader.c_str();
}

const char* Shader::get_layer() const
{
    return get_name();
}

const ShaderParamContainer& Shader::shader_params() const
{
    return impl->m_params;
}

bool Shader::add(OSL::ShadingSystem& shading_system)
{
    for (each<ShaderParamContainer> i = impl->m_params; i; ++i)
    {
        if (!i->add(shading_system))
            return false;
    }

    // For some reason, OSL only supports the surface shader usage.
    // So, we ignore the user shader type specified in the XML file, 
    // and hardcode "surface" here. TODO: research this...
    //if (!shading_system.Shader(get_type(), get_shader(), get_layer()))
    if (!shading_system.Shader("surface", get_shader(), get_layer()))
    {
        RENDERER_LOG_ERROR("error adding shader %s, %s.", get_shader(), get_layer());
        return false;
    }

    return true;
}

void Shader::get_shader_info(
    const SearchPaths&  searchpaths,
    bool&               has_emission,
    bool&               has_transparency) const
{
    has_emission = false;
    has_transparency = false;
    
    string filename = get_shader();
    filename += ".oso";
    filename = searchpaths.qualify(filename);

    ifstream file;
    file.open(filename.c_str());

    if (!file.is_open())
    {
        RENDERER_LOG_ERROR("couldn't open shader %s.", filename.c_str());
        return;
    }
    
    // search for the emission and transparent closures.
    string line;
    while (!file.eof())
    {
        getline(file, line);
        if (line.find("\"emission\"", 0) != string::npos)
            has_emission = true;
        
        if (line.find("\"transparent\"", 0) != string::npos)
            has_transparency = true;
        
        if (has_emission && has_transparency)
            break;
    }
}

}   // namespace renderer
