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

#include "scenegraph/shader_drawable.hpp"

namespace wstdisplay {

ShaderDrawable::ShaderDrawable() :
  m_shader(),
  m_drawables()
{
  m_shader = ShaderProgram::create();
}

void
ShaderDrawable::render(GraphicsContext& gc, unsigned int mask)
{
  glUseProgram(m_shader->get_handle());
  m_shader->set_uniform1i("texture", 0);
  m_drawables.render(gc, mask);
  glUseProgram(0);
}

} // namespace wstdisplay

/* EOF */
