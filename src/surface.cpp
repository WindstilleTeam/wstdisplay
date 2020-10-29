/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include "surface.hpp"

#include <geom/quad.hpp>

#include "surface_drawing_parameters.hpp"
#include "surface_manager.hpp"
#include "scenegraph/vertex_array_drawable.hpp"

SurfacePtr
Surface::create(TexturePtr texture, const geom::frect& uv, const geom::fsize& size)
{
  return SurfacePtr(new Surface(texture, uv, size));
}

SurfacePtr
Surface::create(geom::isize const& size)
{
  return SurfacePtr(new Surface(size));
}

Surface::Surface(geom::isize const& size) :
  m_texture(),
  m_uv(),
  m_size()
{
  m_size  = geom::fsize(size);

  m_texture = Texture::create(GL_TEXTURE_2D, size);
  m_uv      = geom::frect(0.0f,
                          0.0f,
                          m_size.width()  / static_cast<float>(m_texture->get_width()),
                          m_size.height() / static_cast<float>(m_texture->get_height()));
}

Surface::Surface(TexturePtr texture, const geom::frect& uv, const geom::fsize& size) :
  m_texture(),
  m_uv(),
  m_size()
{
  m_texture = texture;
  m_uv      = uv;
  m_size    = size;
}

Surface::~Surface()
{
}

float
Surface::get_width()  const
{
  return m_size.width();
}

float
Surface::get_height() const
{
  return m_size.height();
}

geom::fsize
Surface::get_size() const
{
  return m_size;
}

TexturePtr
Surface::get_texture() const
{
  return m_texture;
}

geom::frect
Surface::get_uv() const
{
  return m_uv;
}

void
Surface::draw(GraphicsContext& gc, const glm::vec2& pos) const
{
  VertexArrayDrawable va;

  va.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  va.set_texture(m_texture);

  va.set_mode(GL_TRIANGLE_FAN);

  va.texcoord(m_uv.left(), m_uv.top());
  va.vertex(pos.x, pos.y);

  va.texcoord(m_uv.right(), m_uv.top());
  va.vertex(pos.x + m_size.width(), pos.y);

  va.texcoord(m_uv.right(), m_uv.bottom());
  va.vertex(pos.x + m_size.width(), pos.y + m_size.height());

  va.texcoord(m_uv.left(), m_uv.bottom());
  va.vertex(pos.x, pos.y + m_size.height());

  va.render(gc);
}

void
Surface::draw(GraphicsContext& gc, const SurfaceDrawingParameters& params) const
{
  VertexArrayDrawable va;
  va.set_blend_func(params.blendfunc_src, params.blendfunc_dst);
  va.set_texture(m_texture);

  va.set_mode(GL_TRIANGLE_FAN);

  float uv_left = m_uv.left();
  float uv_top = m_uv.top();
  float uv_right = m_uv.right();
  float uv_bottom = m_uv.bottom();

  if (params.hflip) {
    std::swap(uv_left, uv_right);
  }

  if (params.vflip) {
    std::swap(uv_top, uv_bottom);
  }

  geom::quad quad(params.pos.x,
            params.pos.y,
            params.pos.x + m_size.width()  * params.scale.x,
            params.pos.y + m_size.height() * params.scale.y);

  quad.rotate(params.angle);

  va.set_depth_test(params.depth_test);

  va.color(params.color);
  va.texcoord(uv_left, uv_top);
  va.vertex(quad.p1.x, quad.p1.y, params.z_pos);

  va.color(params.color);
  va.texcoord(uv_right, uv_top);
  va.vertex(quad.p2.x, quad.p2.y, params.z_pos);

  va.color(params.color);
  va.texcoord(uv_right, uv_bottom);
  va.vertex(quad.p3.x, quad.p3.y, params.z_pos);

  va.color(params.color);
  va.texcoord(uv_left, uv_bottom);
  va.vertex(quad.p4.x, quad.p4.y, params.z_pos);

  va.render(gc);
}

/* EOF */