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

#ifndef HEADER_WINDSTILLE_SHADER_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SHADER_DRAWABLE_HPP

#include <wstdisplay/shader_program.hpp>
#include <wstdisplay/scenegraph/drawable_group.hpp>
#include <wstdisplay/scenegraph/drawable.hpp>

namespace wstdisplay {

class ShaderDrawable : public Drawable
{
private:
  ShaderProgramPtr m_shader;
  DrawableGroup m_drawables;

public:
  ShaderDrawable();

  void render(GraphicsContext& gc, unsigned int mask) override;

  void set_shader(ShaderProgramPtr shader) { m_shader = shader; }
  ShaderProgramPtr get_shader() { return m_shader; }
  DrawableGroup& get_drawable_group() { return m_drawables; }

private:
  ShaderDrawable(const ShaderDrawable&);
  ShaderDrawable& operator=(const ShaderDrawable&);
};

} // namespace wstdisplay

#endif

/* EOF */
