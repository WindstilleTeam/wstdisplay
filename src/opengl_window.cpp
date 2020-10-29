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

#include "opengl_window.hpp"

#include <iostream>
#include <GL/glew.h>
#include <SDL.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "opengl_state.hpp"
#include "graphics_context.hpp"
#include "assert_gl.hpp"

namespace wstdisplay {

class OpenGLWindowImpl
{
public:
  SDL_Window*   m_window;
  SDL_GLContext m_gl_context;
  geom::isize m_size;
  std::unique_ptr<GraphicsContext> m_gc;

  OpenGLWindowImpl() :
    m_window(nullptr),
    m_gl_context(nullptr),
    m_size(),
    m_gc()
  {}

private:
  OpenGLWindowImpl(const OpenGLWindowImpl&) = delete;
  OpenGLWindowImpl& operator=(const OpenGLWindowImpl&) = delete;
};

OpenGLWindow::OpenGLWindow(const std::string& title,
                           const geom::isize& size, const geom::isize& aspect, bool fullscreen, int anti_aliasing) :
  m_impl(new OpenGLWindowImpl)
{
  m_impl->m_size = size;

  //SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1); // vsync
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  // FIXME: Should make this configurable, as Matrox G450 can't do it,
  // but works 'fine' without it
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    8);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  if (anti_aliasing)
  {
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 ); // boolean value, either it's enabled or not
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, anti_aliasing ); // 0, 2, or 4 for number of samples
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

  m_impl->m_window = SDL_CreateWindow(title.c_str(),
                                      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                      size.width(), size.height(),
                                      SDL_WINDOW_OPENGL | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
  if (!m_impl->m_window)
  {
    throw std::runtime_error("Display:: Couldn't create window");
  }
  else
  {
    m_impl->m_gl_context = SDL_GL_CreateContext(m_impl->m_window);
    if (!m_impl->m_gl_context)
    {
      throw std::runtime_error("Display:: failed to create GLContext");
    }

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
      std::ostringstream msg;
      msg << "Display:: Couldn't initialize glew: " << glewGetString(err);
      throw std::runtime_error(msg.str());
    }
    else
    {
      std::cout << "glewInit() successfull: " << glewGetString(GLEW_VERSION) << std::endl;
      std::cout << "OpenGL " << glGetString(GL_VERSION) << " detected" << std::endl;
      std::cout << "OpenGL 3.2: " << GL_VERSION_3_2 << std::endl;
      std::cout << "GL_VERSION_3_0: " << GL_VERSION_3_0 << std::endl;

      if (anti_aliasing)
        glEnable(GL_MULTISAMPLE);

      assert_gl();

      OpenGLState::init();
      m_impl->m_gc = std::make_unique<GraphicsContext>();

      glViewport(0, 0, m_impl->m_size.width(), m_impl->m_size.height());

      m_impl->m_gc->set_aspect_size(aspect);

      m_impl->m_gc->set_projection(
        glm::ortho(0.0f,
                   static_cast<float>(m_impl->m_gc->size().width()),
                   static_cast<float>(m_impl->m_gc->size().height()),
                   0.0f,
                   1000.0f,
                   -1000.0f));

      if ((false)) // disabled for the moment, as it seems to do more harm then good
      { // Magic pixel center constant, without that textures drawn in
        // pixel coordinates might end up blurry
        m_impl->m_gc->translate(0.375f, 0.375f, 0.0f);
      }
    }
  }

  assert_gl();
}

OpenGLWindow::~OpenGLWindow()
{
  SDL_GL_DeleteContext(m_impl->m_gl_context);
  SDL_DestroyWindow(m_impl->m_window);
}

void
OpenGLWindow::set_title(std::string const& title)
{
  SDL_SetWindowTitle(m_impl->m_window, title.c_str());
}

void
OpenGLWindow::set_icon(std::filesystem::path const& filename)
{
  surf::SoftwareSurface const pixeldata = surf::SoftwareSurface::from_file(filename);

  SDL_Surface* sdl_surface;
  if constexpr (std::endian::native == std::endian::big) {
    sdl_surface = SDL_CreateRGBSurfaceFrom(const_cast<uint8_t*>(static_cast<uint8_t const*>(pixeldata.get_data())),
                                           pixeldata.get_width(),
                                           pixeldata.get_height(),
                                           pixeldata.get_format() == surf::PixelFormat::RGB ? 24 : 32,
                                           pixeldata.get_pitch(),
                                           0xff000000,
                                           0x00ff0000,
                                           0x0000ff00,
                                           pixeldata.get_format() == surf::PixelFormat::RGB ? 0x00000000 : 0x000000ff);
  } else {
    sdl_surface = SDL_CreateRGBSurfaceFrom(const_cast<uint8_t*>(static_cast<uint8_t const*>(pixeldata.get_data())),
                                           pixeldata.get_width(),
                                           pixeldata.get_height(),
                                           pixeldata.get_format() == surf::PixelFormat::RGB ? 24 : 32,
                                           pixeldata.get_pitch(),
                                           0x000000ff,
                                           0x0000ff00,
                                           0x00ff0000,
                                           pixeldata.get_format() == surf::PixelFormat::RGB ? 0x00000000 : 0xff000000);
  }

  SDL_SetWindowIcon(m_impl->m_window, sdl_surface);

  SDL_FreeSurface(sdl_surface);
}

int
OpenGLWindow::get_width()  const
{
  return m_impl->m_size.width();
}

int
OpenGLWindow::get_height() const
{
  return m_impl->m_size.height();
}

geom::isize
OpenGLWindow::get_size() const
{
  return m_impl->m_size;
}

void
OpenGLWindow::set_fullscreen(bool fullscreen)
{
  if (fullscreen)
  {
    SDL_SetWindowFullscreen(m_impl->m_window, SDL_WINDOW_FULLSCREEN);
  }
  else
  {
    SDL_SetWindowFullscreen(m_impl->m_window, 0);
  }
}

void
OpenGLWindow::set_gamma(float r, float g, float b)
{
  if (SDL_SetWindowBrightness(m_impl->m_window, (r+g+b)/3.0f) == -1)
  {
    // Couldn't set gamma
  }
}

GraphicsContext&
OpenGLWindow::get_gc() const
{
  assert(m_impl->m_gc);
  return *m_impl->m_gc;
}

void
OpenGLWindow::swap_buffers()
{
  SDL_GL_SwapWindow(m_impl->m_window);
}

} // namespace wstdisplay

/* EOF */
