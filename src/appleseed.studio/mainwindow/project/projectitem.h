
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

#ifndef APPLESEED_STUDIO_MAINWINDOW_PROJECT_PROJECTITEM_H
#define APPLESEED_STUDIO_MAINWINDOW_PROJECT_PROJECTITEM_H

// appleseed.studio headers.
#include "mainwindow/project/itembase.h"

// Forward declarations.
namespace appleseed { namespace studio { template <typename Entity, typename ParentEntity, typename ParentItem> class CollectionItem; } }
namespace appleseed { namespace studio { class OutputItem; } }
namespace appleseed { namespace studio { class ProjectBuilder; } }
namespace appleseed { namespace studio { class SceneItem; } }
namespace renderer  { class ParamArray; }
namespace renderer  { class Project; }
namespace renderer  { class RenderLayerRule; }

namespace appleseed {
namespace studio {

class ProjectItem
  : public ItemBase
{
  public:
    ProjectItem(
        ProjectBuilder&             project_builder,
        renderer::ParamArray&       settings);

    void expand();

    void add_item(renderer::RenderLayerRule* rule);

  private:
    typedef CollectionItem<renderer::RenderLayerRule, renderer::Project, ProjectItem> RenderLayerRuleCollectionItem;

    SceneItem*                      m_scene_item;
    RenderLayerRuleCollectionItem*  m_render_layer_collection_item;
    OutputItem*                     m_output_item;
};

}       // namespace studio
}       // namespace appleseed

#endif  // !APPLESEED_STUDIO_MAINWINDOW_PROJECT_PROJECTITEM_H
