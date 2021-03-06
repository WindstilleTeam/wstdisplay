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
#include <assert.h>
#include <iostream>

#include <geom/size.hpp>

#include "assert_gl.hpp"
#include "renderbuffer.hpp"
#include "framebuffer.hpp"
#include "assert_gl.hpp"

namespace wstdisplay {

FramebufferPtr
Framebuffer::create_with_texture(GLenum target, geom::isize const& size, int multisample)
{
  FramebufferPtr framebuffer(new Framebuffer);
  framebuffer->create_with_texture_internal(target, size, multisample);
  return framebuffer;
}

FramebufferPtr
Framebuffer::create(geom::isize const& size, int multisample)
{
  FramebufferPtr framebuffer(new Framebuffer);
  framebuffer->create_internal(GL_RGB8, size, multisample);
  return framebuffer;
}

FramebufferPtr
Framebuffer::create_hdr(geom::isize const& size, int multisample)
{
  FramebufferPtr framebuffer(new Framebuffer);
  framebuffer->create_internal(GL_RGBA16F, size, multisample);
  return framebuffer;
}

Framebuffer::Framebuffer() :
  m_handle(0),
  m_size(),
  m_texture(),
  m_color_buffer(),
  m_depth_stencil_buffer()
{
  assert_gl();
  glGenFramebuffers(1, &m_handle);
  assert_gl();
}

Framebuffer::~Framebuffer()
{
  glDeleteFramebuffers(1, &m_handle);
}

TexturePtr
Framebuffer::get_texture()
{
  assert(m_texture);
  return m_texture;
}

int
Framebuffer::get_width()  const
{
  return m_size.width();
}

int
Framebuffer::get_height() const
{
  return m_size.height();
}

geom::isize
Framebuffer::get_size() const
{
  return m_size;
}

GLuint
Framebuffer::get_handle() const
{
  return m_handle;

}

void
Framebuffer::create_with_texture_internal(GLenum target, geom::isize const& size, int multisample)
{
  assert(!size.is_empty());

  assert_gl();

  m_size = size;
  m_texture = Texture::create(target, size);
  m_depth_stencil_buffer = Renderbuffer::create(GL_DEPTH24_STENCIL8, size, multisample);

  int previous_framebuffer = 0;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previous_framebuffer);

  // FIXME: Should use push/pop_framebuffer instead, but don't have pointer to Framebuffer here
  glBindFramebuffer(GL_FRAMEBUFFER, m_handle);

  // bind texture and renderbuffers to the framebuffer
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture->get_target(), m_texture->get_handle(), 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,   GL_RENDERBUFFER, m_depth_stencil_buffer->get_handle());
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_stencil_buffer->get_handle());

  assert_gl();

  check_completness();

  glBindFramebuffer(GL_FRAMEBUFFER, previous_framebuffer);
}

void
Framebuffer::create_internal(GLenum format, geom::isize const& size, int multisample)
{
  assert_gl();

  m_size = size;
  m_color_buffer = Renderbuffer::create(format, size, multisample);
  m_depth_stencil_buffer = Renderbuffer::create(GL_DEPTH24_STENCIL8, size, multisample);

  int previous_framebuffer = 0;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previous_framebuffer);

  // FIXME: Should use push/pop_framebuffer instead, but don't have pointer to Framebuffer here
  glBindFramebuffer(GL_FRAMEBUFFER, m_handle);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,  GL_RENDERBUFFER, m_color_buffer->get_handle());
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,   GL_RENDERBUFFER, m_depth_stencil_buffer->get_handle());
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_stencil_buffer->get_handle());
  assert_gl();

  check_completness();

  glBindFramebuffer(GL_FRAMEBUFFER, previous_framebuffer);

  assert_gl();
}

void
Framebuffer::check_completness()
{
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  switch(status)
  {
    case GL_FRAMEBUFFER_COMPLETE:
      std::cout << "Framebuffer ok" << std::endl;
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
      std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n";
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
      std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n";
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
      std::cout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n";
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
      std::cout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n";
      break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
      std::cout << "GL_FRAMEBUFFER_UNSUPPORTED\n";
      break;
    default:
      std::cout << "Framebuffer status failure: " << status << std::endl;
      break;
  }
}

void
Framebuffer::blit(geom::irect const& srcrect, geom::irect const& dstrect,
                  GLbitfield mask, GLenum filter)
{
  assert_gl();

  int previous_framebuffer = 0;
  glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &previous_framebuffer);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_handle);

  glBlitFramebuffer(srcrect.left(), srcrect.top(), srcrect.right(), srcrect.bottom(),
                    dstrect.left(), dstrect.top(), dstrect.right(), dstrect.bottom(),
                    mask, filter);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, previous_framebuffer);

  assert_gl();
}

} // namespace wstdisplay

/* EOF */
