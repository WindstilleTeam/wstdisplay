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

#include "texture.hpp"

#include <iostream>
#include <stdint.h>
#include <sstream>
#include <stdexcept>

#include <glm/ext.hpp>

#include <geom/rect.hpp>
#include <geom/io.hpp>

#include "assert_gl.hpp"
#include "software_surface.hpp"
#include "texture_manager.hpp"

namespace wstdisplay {

TexturePtr
Texture::create(SoftwareSurface const& image, GLint format)
{
  return TexturePtr(new Texture(image, format));
}

TexturePtr
Texture::create(GLenum target, geom::isize const& size, GLint format)
{
  return TexturePtr(new Texture(target, size, format));
}

Texture::Texture() :
  m_target(0),
  m_handle(0),
  m_size(0, 0)
{
  glGenTextures(1, &m_handle);
  assert_gl();
}

Texture::Texture(GLenum target, geom::isize const& size, GLint format) :
  m_target(target),
  m_handle(0),
  m_size(size)
{
  assert_gl();

  if (!GLEW_ARB_texture_non_power_of_two) {
    if (!glm::isPowerOfTwo(m_size.width()) || !glm::isPowerOfTwo(m_size.height())) {
      std::cout  << "Texture::Texture(): texture dimensions have non power of two size: " << m_size;

      m_size = geom::isize(glm::ceilPowerOfTwo(m_size.width()),
                           glm::ceilPowerOfTwo(m_size.height()));
    }
  }

  glGenTextures(1, &m_handle);
  assert_gl();

  glBindTexture(GL_TEXTURE_2D, m_handle);

  glTexImage2D(target, 0, format, m_size.width(), m_size.height(), 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);

  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  assert_gl();
}

Texture::Texture(SoftwareSurface const& image, GLint glformat) :
  m_target(GL_TEXTURE_2D),
  m_handle(0),
  m_size(image.get_size())
{
  assert_gl();

  glGenTextures(1, &m_handle);
  assert_gl();

  if (!GLEW_ARB_texture_non_power_of_two) {
    if (!glm::isPowerOfTwo(image.get_width()) || !glm::isPowerOfTwo(image.get_height())) {
      std::ostringstream str;
      str << "Texture::Texture(): image dimensions have non power of two size: "
          << image.get_width() << "x" << image.get_height();
      throw std::runtime_error(str.str());
    }
  }

  if (image.get_format() != surf::PixelFormat::RGB8 &&
      image.get_format() != surf::PixelFormat::RGBA8) {
    throw std::runtime_error("image not in RGB8 or RGBA8 format");
  }

  // FIXME: User SDL_ConvertSurface to bring images in the right format
  // SDL_ConvertSurface(bmp, screen->format, SDL_SWSURFACE);

  try
  {
    GLint maxt;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxt);

    if(image.get_width() > maxt || image.get_height() > maxt)
    {
      throw std::runtime_error("Texture size not supported");
    }

    GLint sdl_format;
    int bytes_per_pixel;

    if (image.get_format() == surf::PixelFormat::RGB8)
    {
      sdl_format = GL_RGB;
      bytes_per_pixel = 3;
    }
    else if (image.get_format() == surf::PixelFormat::RGBA8)
    {
      sdl_format = GL_RGBA;
      bytes_per_pixel = 4;
    }
    else
    {
      throw std::runtime_error("Texture: Image format not supported");
    }

    glBindTexture(GL_TEXTURE_2D, m_handle);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, image.get_pitch() / bytes_per_pixel);

    if ((false))
    { // no mipmapping
      glTexImage2D(m_target, 0, glformat,
                   image.get_width(), image.get_height(), 0, sdl_format,
                   GL_UNSIGNED_BYTE, image.get_data());

      glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    { // use mipmapping
      gluBuild2DMipmaps(m_target, glformat,
                        image.get_width(), image.get_height(), sdl_format,
                        GL_UNSIGNED_BYTE, image.get_data());

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    assert_gl();

    glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    assert_gl();
  }
  catch(...)
  {
    throw;
  }
}

Texture::~Texture()
{
  glDeleteTextures(1, &m_handle);
}

int
Texture::get_width() const
{
  return m_size.width();
}

int
Texture::get_height() const
{
  return m_size.height();
}

geom::isize
Texture::get_size() const
{
  return m_size;
}

GLuint
Texture::get_handle() const
{
  return m_handle;
}

void
Texture::put(SoftwareSurface const& image, const geom::irect& srcrect, int x, int y)
{
  assert_gl();

  GLint sdl_format;
  int bytes_per_pixel;

  if (image.get_format() == surf::PixelFormat::RGB8)
  {
    sdl_format = GL_RGB;
    bytes_per_pixel = 3;
  }
  else if (image.get_format() == surf::PixelFormat::RGBA8)
  {
    sdl_format = GL_RGBA;
    bytes_per_pixel = 4;
  }
  else
  {
    throw std::runtime_error("Texture: SoftwareSurface format not supported");
  }

  glBindTexture(GL_TEXTURE_2D, m_handle);

  // FIXME: Add some checks here to make sure image has the right format
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // FIXME: Does SDL always use 4?
  glPixelStorei(GL_UNPACK_ROW_LENGTH,
                image.get_pitch() / bytes_per_pixel);

  glTexSubImage2D(m_target, 0, x, y,
                  srcrect.width(), srcrect.height(), sdl_format, GL_UNSIGNED_BYTE,
                  static_cast<uint8_t const*>(image.get_data())
                  + srcrect.top()  * image.get_pitch()
                  + srcrect.left() * bytes_per_pixel);

  assert_gl();
}

void
Texture::put(SoftwareSurface const& image, int x, int y)
{
  put(image, geom::irect(0, 0, image.get_width(), image.get_height()), x, y);
}

void
Texture::set_wrap(GLenum mode)
{
  assert_gl();

  glBindTexture(GL_TEXTURE_2D, m_handle);

  glTexParameteri(m_target, GL_TEXTURE_WRAP_S, mode);
  glTexParameteri(m_target, GL_TEXTURE_WRAP_T, mode);
  glTexParameteri(m_target, GL_TEXTURE_WRAP_R, mode); // FIXME: only good for 3d textures?!

  assert_gl();
}

void
Texture::set_filter(GLenum mode)
{
  assert_gl();

  glBindTexture(GL_TEXTURE_2D, m_handle);

  glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, mode);
  glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, mode);

  assert_gl();
}

SoftwareSurface
Texture::get_software_surface() const
{
  glBindTexture(GL_TEXTURE_2D, m_handle);

  SoftwareSurface surface = SoftwareSurface::create(surf::PixelFormat::RGBA8, m_size);

  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface.get_data());

  return surface;
}

GLenum
Texture::get_target() const
{
  return m_target;
}

} // namespace wstdisplay

/* EOF */
