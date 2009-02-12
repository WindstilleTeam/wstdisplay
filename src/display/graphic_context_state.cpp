//  Flexlay - A Generic 2D Game Editor
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <math.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include "display/scene_context.hpp"
#include "graphic_context_state.hpp"

class GraphicContextStateImpl
{
public:
  int width;
  int height;
  
  Vector2f offset;
  float zoom;
  float rotation;
};

GraphicContextState::GraphicContextState()
  : impl(new GraphicContextStateImpl())
{
  impl->width  = 1;
  impl->height = 1; 
  impl->offset = Vector2f(0,0);
  impl->zoom   = 1.0f;
  impl->rotation = 0;
}

GraphicContextState::GraphicContextState(int w, int h)
  : impl(new GraphicContextStateImpl())
{  
  impl->width  = w;
  impl->height = h;
  impl->offset = Vector2f(0,0); 
  impl->zoom   = 1.0f;
  impl->rotation = 0;
}

void
GraphicContextState::set_size(int w, int h)
{
  impl->width  = w;
  impl->height = h;
}

void
GraphicContextState::push(SceneContext& sc)
{
  sc.push_modelview();

  sc.translate(impl->width/2, impl->height/2);
  sc.rotate(impl->rotation);
  sc.translate(-impl->width/2, -impl->height/2);

  sc.scale(get_zoom(), get_zoom());
  sc.translate(impl->offset.x, impl->offset.y);
}

void
GraphicContextState::pop(SceneContext& sc)
{
  sc.pop_modelview();
}

Rectf
GraphicContextState::get_clip_rect()
{
  return Rectf(Vector2f(-impl->offset.x,
                         -impl->offset.y),
               Sizef(get_width()  / impl->zoom,
                     get_height() / impl->zoom));
}

void
GraphicContextState::set_pos(const Vector2f& pos)
{
  impl->offset.x = -pos.x + (get_width()/2  / impl->zoom);
  impl->offset.y = -pos.y + (get_height()/2 / impl->zoom);
}

Vector2f
GraphicContextState::get_pos() const
{
  return Vector2f(-impl->offset.x + (get_width()/2  / impl->zoom),
                   -impl->offset.y + (get_height()/2  / impl->zoom));
}

void
GraphicContextState::set_zoom(const Vector2f& pos, float z)
{
  float old_zoom = impl->zoom;
  set_zoom(z);
  impl->offset.x = pos.x/impl->zoom - pos.x/old_zoom + impl->offset.x;
  impl->offset.y = pos.y/impl->zoom - pos.y/old_zoom + impl->offset.y;
}

void
GraphicContextState::set_zoom(float z)
{
  impl->zoom = z;
}

float
GraphicContextState::get_zoom()
{
  return impl->zoom;
}

void
GraphicContextState::zoom_to (const Rectf& rect)
{
  float center_x = (rect.left + rect.right) / 2.0f;
  float center_y = (rect.top + rect.bottom) / 2.0f;

  float width  = rect.right - rect.left;
  float height = rect.bottom - rect.top;
  float screen_relation = float(get_height())/float(get_width ());
  float rect_relation   = height/width; 
  
  //std::cout << "Screen: " << screen_relation << " Zoom: " << rect_relation << std::endl;
  if (rect_relation < screen_relation) // take width, ignore height
    {
      impl->zoom = get_width()/width; 
    }
  else // take height, ignore width
    {
      impl->zoom = get_height()/height;
    }

  impl->offset.x = (get_width()  / (2*impl->zoom)) - center_x;
  impl->offset.y = (get_height() / (2*impl->zoom)) - center_y;
}

Vector2f
GraphicContextState::screen_to_world(const Vector2f& pos_)
{
  Vector2f pos(pos_.x, pos_.y);
  float sa = sin(-impl->rotation/180.0f*M_PI);
  float ca = cos(-impl->rotation/180.0f*M_PI);

  float dx = pos.x - impl->width/2;
  float dy = pos.y - impl->height/2;

  pos.x = impl->width/2  + (ca * dx - sa * dy);
  pos.y = impl->height/2 + (sa * dx + ca * dy);

  Vector2f p((float(pos.x) / impl->zoom) - impl->offset.x, 
              (float(pos.y) / impl->zoom) - impl->offset.y);

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

/* EOF */
