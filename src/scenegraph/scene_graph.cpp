// Windstille Display Library
// Copyright (C) 2002-2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "scenegraph/scene_graph.hpp"

#include "scenegraph/drawable.hpp"
#include "scenegraph/drawable_group.hpp"

namespace wstdisplay {

SceneGraph::SceneGraph()
  : m_drawables(new DrawableGroup())
{
}

void
SceneGraph::add_drawable(std::shared_ptr<Drawable> drawable)
{
  m_drawables->add_drawable(drawable);
}

void
SceneGraph::remove_drawable(std::shared_ptr<Drawable> drawable)
{
  m_drawables->remove_drawable(drawable);
}

void
SceneGraph::render(GraphicsContext& gc, unsigned int mask)
{
  m_drawables->render(gc, mask);
}

void
SceneGraph::clear()
{
  m_drawables->clear();
}

} // namespace wstdisplay

/* EOF */
