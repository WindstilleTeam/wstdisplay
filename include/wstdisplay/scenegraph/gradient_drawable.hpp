/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_GRADIENT_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_GRADIENT_DRAWABLE_HPP

#include <vector>
#include <memory>

#include <wstdisplay/scenegraph/vertex_array_drawable.hpp>

class GradientDrawable : public Drawable
{
public:
  GradientDrawable(std::vector<float> colors);

  void render(GraphicsContext& gc, unsigned int mask) override;

private:
  std::unique_ptr<VertexArrayDrawable> m_array;
  std::vector<float> m_colors;

private:
  GradientDrawable(const GradientDrawable&);
  GradientDrawable& operator=(const GradientDrawable&);
};

#endif

/* EOF */