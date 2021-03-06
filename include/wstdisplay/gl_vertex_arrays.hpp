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

#ifndef HEADER_SUPERTUX_VIDEO_GL_GL_VERTEX_ARRAYS_HPP
#define HEADER_SUPERTUX_VIDEO_GL_GL_VERTEX_ARRAYS_HPP

#include <stddef.h>
#include <span>

#include <GL/glew.h>

namespace wstdisplay {

class GraphicsContext;

class GLVertexArrays final
{
public:
  GLVertexArrays(GraphicsContext& context);
  ~GLVertexArrays();

  void bind();

  void set_positions(std::span<float const> data);
  void set_texcoords(std::span<float const> data);
  void set_colors(std::span<float const> data);

private:
  GraphicsContext& m_gc;
  GLuint m_vao;
  GLuint m_positions_buffer;
  GLuint m_texcoords_buffer;
  GLuint m_color_buffer;

private:
  GLVertexArrays(const GLVertexArrays&) = delete;
  GLVertexArrays& operator=(const GLVertexArrays&) = delete;
};

} // namespace wstdisplay

#endif

/* EOF */
