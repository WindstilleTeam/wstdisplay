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

#include <GL/glew.h>
#include <iostream>

#include "renderbuffer.hpp"

namespace wstdisplay {

RenderbufferPtr
Renderbuffer::create(GLenum format, geom::isize const& size, int multisample)
{
  return RenderbufferPtr(new Renderbuffer(format, size, multisample));
}

Renderbuffer::Renderbuffer(GLenum format, geom::isize const& size, int multisample) :
  m_handle(0)
{
  glGenRenderbuffers(1, &m_handle);

  glBindRenderbuffer(GL_RENDERBUFFER, m_handle);

  if (multisample)
  {
    // antialiasing
    std::cout << "Antialised Renderbuffer" << std::endl;
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, multisample, format, size.width(), size.height());
  }
  else
  {
    glRenderbufferStorage(GL_RENDERBUFFER, format, size.width(), size.height());
  }
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

Renderbuffer::~Renderbuffer()
{
  glDeleteRenderbuffers(1, &m_handle);
}

GLuint
Renderbuffer::get_handle() const
{
  return m_handle;
}

} // namespace wstdisplay

/* EOF */
