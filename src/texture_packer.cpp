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

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <memory>

#include <geom/point.hpp>
#include <geom/rect.hpp>
#include <surf/save.hpp>

#include "software_surface.hpp"
#include "texture_packer.hpp"

namespace wstdisplay {

class TextureSpace
{
private:
  geom::irect          rect;
  bool          used;
  std::unique_ptr<TextureSpace> left;
  std::unique_ptr<TextureSpace> right;

public:
  TextureSpace(const geom::irect& rect_) :
    rect(rect_),
    used(false),
    left(),
    right()
  {}

  ~TextureSpace()
  {}

  bool allocate(const geom::isize& size, geom::irect& out_rect)
  {
    if (size.width()  <= rect.width() &&
        size.height() <= rect.height())
    {
      if (!used)
      {
        used = true;
        out_rect = geom::irect(geom::ipoint(rect.left(), rect.top()), size);

        // FIXME: Make this alterate between horizontal and
        // vertical splitting or chose whichever split options
        // leads to less 'ugly' rectangle (how much different does
        // this make in terms of packing density?)
        left.reset(new TextureSpace(geom::irect(out_rect.left(),  out_rect.bottom(),
                                         out_rect.right(), rect.bottom())));

        right.reset(new TextureSpace(geom::irect(out_rect.right(), out_rect.top(),
                                          rect.right(), rect.bottom())));

        return true;
      }
      else
      {
        return
          left->allocate(size, out_rect) ||
          right->allocate(size, out_rect);
      }
    }
    else
    {
      // geom::isize doesn't fit here
      return false;
    }
  }

private:
  TextureSpace(const TextureSpace&);
  TextureSpace& operator=(const TextureSpace&);
};

class TexturePackerTexture
{
private:
  TexturePtr     texture;
  TextureSpace   space;

public:
  TexturePackerTexture(const geom::isize& size) :
    texture(Texture::create(GL_TEXTURE_2D, size)),
    space(geom::irect(geom::ipoint(0, 0), size))
  {
  }

  ~TexturePackerTexture()
  {}

  TexturePtr get_texture() const { return texture; }

  bool allocate(const geom::isize& size, geom::irect& out_rect, TexturePtr& out_texture)
  {
    if (space.allocate(size, out_rect))
    {
      out_texture = texture;
      return true;
    }
    else
    {
      return false;
    }
  }

private:
  TexturePackerTexture(const TexturePackerTexture&);
  TexturePackerTexture& operator=(const TexturePackerTexture&);
};

TexturePacker::TexturePacker(const geom::isize& texture_size_) :
  texture_size(texture_size_),
  textures()
{
}

TexturePacker::~TexturePacker()
{
  for(Textures::iterator i = textures.begin(); i != textures.end(); ++i)
    delete *i;
  textures.clear();
}

bool
TexturePacker::allocate(const geom::isize& size, geom::irect& rect, TexturePtr& out_texture)
{
  for(Textures::iterator i = textures.begin(); i != textures.end(); ++i)
  {
    if ((*i)->allocate(size, rect, out_texture))
    {
      return true;
    }
  }

  textures.push_back(new TexturePackerTexture(texture_size));
  return textures.back()->allocate(size, rect, out_texture);
}

SurfacePtr
TexturePacker::upload(SoftwareSurface const& surface)
{
  // Add a 1px border around surfaces to avoid blending artifacts
  //SoftwareSurface surface = in_surface.add_1px_border();

  geom::isize    size(surface.get_width()+2, surface.get_height()+2);
  geom::irect    rect;
  TexturePtr texture;

  if (!allocate(size, rect, texture))
  {
    throw std::runtime_error("TexturePacker::upload: texture space allocation failed");
  }
  else
  {
    // duplicate border pixel

    // top
    texture->put(surface, geom::irect(geom::ipoint(0, 0), geom::isize(surface.get_width(), 1)),
                 rect.left()+1, rect.top());
    // bottom
    texture->put(surface, geom::irect(geom::ipoint(0, surface.get_height()-1), geom::isize(surface.get_width(), 1)),
                 rect.left()+1, rect.bottom()-1);
    // left
    texture->put(surface, geom::irect(geom::ipoint(0, 0), geom::isize(1, surface.get_height())),
                 rect.left(), rect.top()+1);
    // right
    texture->put(surface, geom::irect(geom::ipoint(surface.get_width()-1, 0), geom::isize(1, surface.get_height())),
                 rect.right()-1, rect.top()+1);

    // duplicate corner pixels
    texture->put(surface, geom::irect(geom::ipoint(0, 0), geom::isize(1, 1)),
                 rect.left(), rect.top());
    texture->put(surface, geom::irect(geom::ipoint(surface.get_width()-1, 0), geom::isize(1, 1)),
                 rect.right()-1, rect.top());
    texture->put(surface, geom::irect(geom::ipoint(surface.get_width()-1, surface.get_height()-1), geom::isize(1, 1)),
                 rect.right()-1, rect.bottom()-1);
    texture->put(surface, geom::irect(geom::ipoint(0, surface.get_height()-1), geom::isize(1, 1)),
                 rect.left(), rect.bottom()-1);

    // draw the main surface
    texture->put(surface, rect.left()+1, rect.top()+1);

    return Surface::create(texture,
                           geom::frect(static_cast<float>(rect.left()+1)   / static_cast<float>(texture->get_width()),
                                 static_cast<float>(rect.top()+1)    / static_cast<float>(texture->get_height()),
                                 static_cast<float>(rect.right()-1)  / static_cast<float>(texture->get_width()),
                                 static_cast<float>(rect.bottom()-1) / static_cast<float>(texture->get_height())),
                           geom::fsize(static_cast<float>(surface.get_width()), static_cast<float>(surface.get_height())));
  }
}

void
TexturePacker::save_all_as_png() const
{
  for(Textures::const_iterator i = textures.begin(); i != textures.end(); ++i)
  {
    TexturePtr texture = (*i)->get_texture();
    SoftwareSurface surface = texture->get_software_surface();

    char filename[1024];
    sprintf(filename, "/tmp/texture_packer%04d.png", int(i - textures.begin()));
    std::cout << "Saving: " << filename << std::endl;
    surf::save(surface, filename, "png");
  }
}

} // namespace wstdisplay

/* EOF */
