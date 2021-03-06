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

#include "blitter.hpp"

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "software_surface.hpp"

namespace wstdisplay {

void generate_border(SoftwareSurface& surface,
                     int x_pos, int y_pos, int width, int height)
{
  assert(surface.get_format() == surf::PixelFormat::RGBA8);

  uint8_t* data = static_cast<uint8_t*>(surface.get_data());
  int pitch = surface.get_pitch();

  // duplicate the top line
  memcpy(data + (y_pos-1)*pitch + 4*x_pos,
         data + (y_pos)*pitch + 4*x_pos,
         4*width);

  // duplicate the bottom line
  memcpy(data + (y_pos+height)*pitch + 4*x_pos,
         data + (y_pos+height-1)*pitch + 4*x_pos,
         4*width);

  // duplicate left and right borders
  for(int y = y_pos-1; y < y_pos + height+1; ++y)
  {
    uint32_t* p = reinterpret_cast<uint32_t*> (data + (y*pitch + 4*(x_pos-1)));
    *p = *(p+1);
    p = reinterpret_cast<uint32_t*> (data + (y*pitch + 4*(x_pos + width)));
    *p = *(p-1);
  }
}

} // namespace wstdisplay

/* EOF */
