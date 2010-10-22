/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#include "display/opengl_window.hpp"

#include <GL/glew.h>
#include <SDL.h>
#include <SDL/SDL_image.h>

#include "util/util.hpp" 
#include "display/opengl_state.hpp"
#include "display/display.hpp"
#include "display/assert_gl.hpp"
#include "app/config.hpp"

class OpenGLWindowImpl
{
public:
  SDL_Surface* m_window;
  Size         m_size;

  OpenGLWindowImpl() :
    m_window(0),
    m_size()
  {}
};

OpenGLWindow::OpenGLWindow(const std::string& title, 
                           const Size& size, const Size& aspect, bool fullscreen, int anti_aliasing) :
  m_impl(new OpenGLWindowImpl)
{
  m_impl->m_size = size;

  SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1); // vsync
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 

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
  
  SDL_WM_SetCaption(title.c_str(), title.c_str());
  SDL_WM_SetIcon(IMG_Load(Pathname("icon.png").get_sys_path().c_str()), NULL);

  m_impl->m_window = SDL_SetVideoMode(size.width, size.height,
                                      0, SDL_OPENGL | (fullscreen ? SDL_FULLSCREEN : 0));

  if (!m_impl->m_window)
  {
    throw std::runtime_error("Display:: Couldn't create window");
  }
  else
  {
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

      glViewport(0, 0, m_impl->m_window->w, m_impl->m_window->h);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();

      Display::aspect_size = aspect;

      glOrtho(0.0, Display::get_width(), Display::get_height(),
              0.0, 1000.0, -1000.0);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      if (0) // disabled for the moment, as it seems to do more harm then good
      { // Magic pixel center constant, without that textures drawn in
        // pixel coordinates might end up blurry       
        glTranslated(0.375f, 0.375f, 0.0);
      }

      if (anti_aliasing)
        glEnable(GL_MULTISAMPLE); 

      assert_gl("setup projection");

      OpenGLState::init();
    }
  }
}

OpenGLWindow::~OpenGLWindow()
{
  // must not free m_impl->m_window, see:
  // http://sdl.beuc.net/sdl.wiki/SDL_SetVideoMode
} 

int  
OpenGLWindow::get_width()  const
{
  return m_impl->m_window->w; 
}

int 
OpenGLWindow::get_height() const 
{
  return m_impl->m_window->h; 
}

Size 
OpenGLWindow::get_size() const
{
  return Size(m_impl->m_window->w, m_impl->m_window->h); 
}

void
OpenGLWindow::set_fullscreen(bool fullscreen)
{
  Uint32 flags = SDL_OPENGL;

  if (fullscreen)
    flags |= SDL_FULLSCREEN;

  m_impl->m_window = SDL_SetVideoMode(m_impl->m_size.width, m_impl->m_size.height, 0, flags);

  if (!m_impl->m_window)
  {
    throw std::runtime_error("OpenGLWindow: Couldn't create window");
  }
}

void
OpenGLWindow::set_gamma(float r, float g, float b)
{
  if (SDL_SetGamma(r, g, b) == -1)
  {
    // Couldn't set gamma
  }
}

void
OpenGLWindow::swap_buffers()
{
  SDL_GL_SwapBuffers();
}

/* EOF */
