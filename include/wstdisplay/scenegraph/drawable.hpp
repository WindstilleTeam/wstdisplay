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

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_DRAWABLE_HPP

#include <glm/glm.hpp>

#include <wstdisplay/texture.hpp>

namespace wstdisplay {

class GraphicsContext;

class Drawable
{
protected:
  geom::fpoint pos;
  float    z_pos;
  glm::mat4   modelview;
  unsigned int render_mask;

public:
  Drawable()
    : pos(0.0f, 0.0f),
      z_pos(0.0f),
      modelview(glm::mat4(1.0)),
      render_mask(1) // FIXME: Evil hardcoded constant
  {}

  Drawable(const geom::fpoint& pos_, float z_pos_ = 0,  const glm::mat4& modelview_ = glm::mat4(1.0))
    : pos(pos_),
      z_pos(z_pos_),
      modelview(modelview_),
      render_mask(1) // FIXME: Evil hardcoded constant
  {}
  virtual ~Drawable() {}

  /**
   * The draw() method does the important work in Drawable,
   * ie. it is the place where you can access the screen with raw
   * OpenGL methods.
   */
  virtual void render(GraphicsContext& gc, unsigned int mask) = 0;

  /** Returns the position at which the request should be drawn */
  float get_z_pos() const { return z_pos; }

  glm::mat4 get_modelview() const
  { return modelview; }

  void set_render_mask(unsigned int mask) { render_mask = mask; }
  unsigned int get_render_mask() const { return render_mask; }

  void set_pos(const geom::fpoint& pos_) { pos = pos_; }

private:
  Drawable (const Drawable&);
  Drawable& operator= (const Drawable&);
};

} // namespace wstdisplay

#endif

/* EOF */
