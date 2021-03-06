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

#ifndef HEADER_WINDSTILLE_DISPLAY_DRAWING_CONTEXT_HPP
#define HEADER_WINDSTILLE_DISPLAY_DRAWING_CONTEXT_HPP

#include <vector>

#include <geom/fwd.hpp>
#include <surf/fwd.hpp>

#include <wstdisplay/scenegraph/drawable.hpp>

#include "texture.hpp"
#include "surface.hpp"

namespace wstdisplay {

class Compositor;
class DrawingParameters;
class GraphicsContext;
class SceneContext;
class Surface;
class SurfaceDrawingParameters;

/** The DrawingContext collects all Drawables and allows you to
    flush them all down to the graphics card in one run, this has the
    advantage that it is possible to z-sort, texture-id sort or
    otherwise optimize the drawing. In addition to that it also allows
    you do render the drawing commands to multiple buffers which might
    be usefull for post-processing effects and such. */
class DrawingContext
{
private:
  using Drawables = std::vector<std::unique_ptr<Drawable> >;
  Drawables drawingrequests;

  std::vector<glm::mat4> modelview_stack;

public:
  DrawingContext();
  ~DrawingContext();

  /** Draws everything in the drawing context to the screen */
  void render(GraphicsContext& gc);

  /** Empties the drawing context */
  void clear();

  /** Fills the screen with a given color, this is different from
      clear() in that it doesn't remove other Drawable from the
      queue */
  void fill_screen(const surf::Color& color);

  void fill_pattern(TexturePtr pattern, const geom::foffset& offset);

  void draw_line(const geom::fline& line, const surf::Color& color, float z_pos = 0);
  void draw_line(const geom::fpoint& pos1, const geom::fpoint& pos2, const surf::Color& color, float z_pos = 0);

  void draw_rect(const geom::frect& rect, const surf::Color& color, float z_pos = 0);
  void fill_rect(const geom::frect& rect, const surf::Color& color, float z_pos = 0);

  void draw_quad(const geom::fquad& quad, const surf::Color& color, float z_pos = 0);
  void fill_quad(const geom::fquad& quad, const surf::Color& color, float z_pos = 0);

  /*{ */
  void draw(std::unique_ptr<Drawable> request);
  void draw(SurfacePtr surface, const geom::fpoint& pos, float z = 0, float alpha = 0);
  void draw(SurfacePtr surface, float x, float y, float z = 0, float alpha = 0);
  void draw(SurfacePtr surface, const SurfaceDrawingParameters& params, float z_pos = 0);
  void draw(SurfacePtr surface, const geom::fpoint& pos, const geom::fquad& quad,
            const DrawingParameters& params, float z_pos = 0);
  void draw_control(SurfacePtr surface, const geom::fpoint& pos, float angle, float z_pos = 0);
  /*} */

  /** Translate the drawing context */
  void translate(float x, float y, float z = 0.0f);

  /** Set the rotation of the drawing context */
  void rotate(float angle, float x = 0.0f, float y = 0.0f, float z = 1.0f);

  /** Set the scaling of the drawing context */
  void scale(float x, float y, float z = 1.0f);

  /** multiply the modelview of the context */
  void mult(const glm::mat4& matrix);

  void push_modelview();
  void pop_modelview();
  void reset_modelview();
  void set_modelview(const glm::mat4& matrix);
  glm::mat4 get_modelview() const { return modelview_stack.back(); }

  /** Return the area of the screen that will be visible*/
  geom::frect get_clip_rect();

private:
  DrawingContext (const DrawingContext&);
  DrawingContext& operator= (const DrawingContext&);
};

} // namespace wstdisplay

#endif

/* EOF */
