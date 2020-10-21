/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HEADER_WINDSTILLE_DISPLAY_COMPOSITOR_IMPL_HPP
#define HEADER_WINDSTILLE_DISPLAY_COMPOSITOR_IMPL_HPP

#include <geom/size.hpp>

class GraphicContextState;
class GraphicsContext;
class SceneContext;
class SceneGraph;

class CompositorImpl
{
protected:
  geom::isize m_framebuffer_size;
  geom::isize m_viewport_size;

public:
  CompositorImpl(const geom::isize& framebuffer_size, const geom::isize& viewport_size)
    : m_framebuffer_size(framebuffer_size),
      m_viewport_size(viewport_size)
  {}

  virtual ~CompositorImpl()
  {}

  virtual void render(GraphicsContext& gc, SceneContext& sc, SceneGraph* sg, const GraphicContextState& state) =0;

  geom::isize get_framebuffer_size() const { return m_framebuffer_size; }
  geom::isize get_viewport_size() const { return m_viewport_size; }
};

#endif

/* EOF */
