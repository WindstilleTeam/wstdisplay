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

#ifndef HEADER_WINDSTILLE_DISPLAY_COMPOSITOR_HPP
#define HEADER_WINDSTILLE_DISPLAY_COMPOSITOR_HPP

#include <geom/size.hpp>

#include "framebuffer.hpp"

class GraphicContextState;
class GraphicsContext;
class SceneContext;
class SceneGraph;

class Compositor
{
public:
  Compositor(const geom::isize& framebuffer_size, const geom::isize& viewport_size);

  void render(GraphicsContext& gc, SceneContext& sc, SceneGraph* sg, const GraphicContextState& state);

  geom::isize get_framebuffer_size() const;
  geom::isize get_viewport_size() const;

private:
  void render_lightmap(GraphicsContext& gc);

private:
  geom::isize m_framebuffer_size;
  geom::isize m_viewport_size;

  FramebufferPtr m_screen;
  FramebufferPtr m_lightmap;

private:
  Compositor(const Compositor&);
  Compositor& operator=(const Compositor&);
};

#endif

/* EOF */