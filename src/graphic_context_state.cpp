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

#include "graphic_context_state.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene_context.hpp"

namespace wstdisplay {

class GraphicContextStateImpl
{
public:
  int width = 1;
  int height = 1;

  geom::foffset offset = geom::foffset(0.0f, 0.0f);
  float zoom = 1.0f;
  float rotation = 0.0f;
};

GraphicContextState::GraphicContextState() :
  impl(new GraphicContextStateImpl())
{
}

GraphicContextState::GraphicContextState(int w, int h) :
  impl(new GraphicContextStateImpl())
{
  impl->width = w;
  impl->height = h;
  impl->offset = geom::foffset(0.0f, 0.0f);
  impl->zoom = 1.0f;
  impl->rotation = 0;
}

void
GraphicContextState::set_size(int w, int h)
{
  impl->width  = w;
  impl->height = h;
}

glm::mat4
GraphicContextState::get_matrix() const
{
  glm::mat4 matrix = glm::mat4(1.0f);

  matrix = glm::translate(matrix,
                          glm::vec3(static_cast<float>(impl->width)  / 2.0f,
                                    static_cast<float>(impl->height) / 2.0f,
                                    0.0f));
  matrix = glm::rotate(matrix, impl->rotation, glm::vec3(0.0f, 0.0f, 1.0f));
  matrix = glm::translate(matrix,
                          glm::vec3(static_cast<float>(-impl->width)  / 2.0f,
                                    static_cast<float>(-impl->height) / 2.0f,
                                    0.0f));

  matrix = glm::scale(matrix, glm::vec3(get_zoom(), get_zoom(), 1.0f));

  matrix = glm::translate(matrix, glm::vec3(impl->offset.x(), impl->offset.y(), 0.0f));

  return matrix;
}

void
GraphicContextState::push(SceneContext& sc) const
{
  sc.push_modelview();

  sc.translate(static_cast<float>(impl->width)  / 2.0f,
               static_cast<float>(impl->height) / 2.0f);
  sc.rotate(impl->rotation);
  sc.translate(static_cast<float>(-impl->width)  / 2.0f,
               static_cast<float>(-impl->height) / 2.0f);

  sc.scale(get_zoom(), get_zoom());
  sc.translate(impl->offset.x(), impl->offset.y());
}

void
GraphicContextState::pop(SceneContext& sc) const
{
  sc.pop_modelview();
}

geom::frect
GraphicContextState::get_clip_rect()
{
  return geom::frect(geom::fpoint(-impl->offset.x(), -impl->offset.y()),
                     geom::fsize(static_cast<float>(get_width())  / impl->zoom,
                                 static_cast<float>(get_height()) / impl->zoom));
}

void
GraphicContextState::set_pos(geom::fpoint const& pos)
{
  impl->offset = geom::foffset(-pos.x() + (static_cast<float>(get_width())  / 2.0f / impl->zoom),
                               -pos.y() + (static_cast<float>(get_height()) / 2.0f / impl->zoom));
}

geom::foffset
GraphicContextState::get_offset() const
{
  return impl->offset;
}

geom::fpoint
GraphicContextState::get_pos() const
{
  return geom::fpoint(-impl->offset.x() + (static_cast<float>(get_width())  / 2.0f / impl->zoom),
                      -impl->offset.y() + (static_cast<float>(get_height()) / 2.0f / impl->zoom));
}

void
GraphicContextState::set_zoom(geom::fpoint const& pos, float z)
{
  float const old_zoom = impl->zoom;
  set_zoom(z);
  impl->offset = geom::foffset(pos.x() / impl->zoom - pos.x() / old_zoom + impl->offset.x(),
                               pos.y() / impl->zoom - pos.y() / old_zoom + impl->offset.y());
}

void
GraphicContextState::set_zoom(float z)
{
  impl->zoom = z;
}

float
GraphicContextState::get_zoom() const
{
  return impl->zoom;
}

void
GraphicContextState::zoom_to (const geom::frect& rect)
{
  float center_x = (rect.left() + rect.right()) / 2.0f;
  float center_y = (rect.top() + rect.bottom()) / 2.0f;

  float width  = rect.right() - rect.left();
  float height = rect.bottom() - rect.top();
  float screen_relation = float(get_height())/float(get_width ());
  float rect_relation   = height / width;

  //std::cout << "Screen: " << screen_relation << " Zoom: " << rect_relation << std::endl;
  if (rect_relation < screen_relation) // take width, ignore height
  {
    impl->zoom = static_cast<float>(get_width()) / width;
  }
  else // take height, ignore width
  {
    impl->zoom = static_cast<float>(get_height()) / height;
  }

  impl->offset = geom::foffset((static_cast<float>(get_width())  / (2.0f * impl->zoom)) - center_x,
                               (static_cast<float>(get_height()) / (2.0f * impl->zoom)) - center_y);
}

geom::fpoint
GraphicContextState::screen_to_world(geom::fpoint const& pos_)
{
  glm::vec2 pos(pos_.x(), pos_.y());
  float sa = sinf(-impl->rotation / 180.0f * glm::pi<float>());
  float ca = cosf(-impl->rotation / 180.0f * glm::pi<float>());

  float dx = pos.x - static_cast<float>(impl->width)  / 2.0f;
  float dy = pos.y - static_cast<float>(impl->height) / 2.0f;

  pos.x = static_cast<float>(impl->width)  / 2.0f + (ca * dx - sa * dy);
  pos.y = static_cast<float>(impl->height) / 2.0f + (sa * dx + ca * dy);

  glm::vec2 p((pos.x / impl->zoom) - impl->offset.x(),
              (pos.y / impl->zoom) - impl->offset.y());

  return p;
}

void
GraphicContextState::set_rotation(float angle)
{
  impl->rotation = angle;
}

float
GraphicContextState::get_rotation()
{
  return impl->rotation;
}

int
GraphicContextState::get_width()  const
{
  return impl->width;
}

int
GraphicContextState::get_height() const
{
  return impl->height;
}

geom::isize
GraphicContextState::get_size() const
{
  return { impl->width, impl->height };
}

GraphicContextState
GraphicContextState::clone() const
{
  GraphicContextState tmp;

  tmp.impl->width    = impl->width;
  tmp.impl->height   = impl->height;
  tmp.impl->offset   = impl->offset;
  tmp.impl->zoom     = impl->zoom;
  tmp.impl->rotation = impl->rotation;

  return tmp;
}

} // namespace wstdisplay

/* EOF */
