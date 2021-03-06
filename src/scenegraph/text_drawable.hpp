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

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_TEXT_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_TEXT_DRAWABLE_HPP

#include <glm/gtc/type_ptr.hpp>

#include <wstdisplay/graphics_context.hpp>

#include "font/ttf_font.hpp"

namespace wstdisplay {

class TextDrawable : public wstdisplay::Drawable
{
private:
  TTFFont& m_font;
  std::string text;

public:
  TextDrawable(TTFFont& font, const std::string& text_, const glm::vec2& pos_, float z_pos_, const glm::mat4& modelview_)
    : Drawable(pos_, z_pos_, modelview_),
      m_font(font),
      text(text_)
  {}
  ~TextDrawable() override {}

  void render(wstdisplay::GraphicsContext& gc, unsigned int mask) override {
    gc.push_matrix();
    gc.mult_matrix(modelview);
    m_font.draw(gc, pos.as_vec(), text);
    gc.pop_matrix();
  }
};

} // namespace wstdisplay

#endif

/* EOF */
