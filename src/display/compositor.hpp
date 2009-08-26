/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#include <boost/scoped_ptr.hpp>

class CompositorImpl;
class DrawingRequest;
class GraphicContextState;
class SceneContext;
class SceneGraph;

class Compositor
{
public:
  Compositor();
  ~Compositor();

  void render(SceneContext& sc, SceneGraph* sg, const GraphicContextState& state);
  void eval(DrawingRequest* request);

private:
  void render_with_framebuffers(SceneContext& sc, SceneGraph* sg);
  void render_without_framebuffers(SceneContext& sc, SceneGraph* sg, 
                                   const GraphicContextState& state);

  void render_lightmap(SceneContext& sc, SceneGraph* sg);

private:
  Compositor(const Compositor&);
  Compositor& operator=(const Compositor&);

private:
  boost::scoped_ptr<CompositorImpl> impl;
};

#endif

/* EOF */