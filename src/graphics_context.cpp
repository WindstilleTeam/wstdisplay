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

#include "graphics_context.hpp"

#include <iostream>
#include <errno.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string.h>

#include <geom/rect.hpp>
#include <geom/line.hpp>
#include <geom/quad.hpp>
#include <surf/color.hpp>
#include <surf/palette.hpp>

#include "assert_gl.hpp"

#include "scenegraph/vertex_array_drawable.hpp"

namespace wstdisplay {

namespace {
int g_default_framebuffer = 0;
std::vector<FramebufferPtr> framebuffers;

const char default_vert_source[] = R"(#version 330 core

in vec2 texcoord;
in vec4 diffuse;
in vec3 position;

out vec2 texcoord_v;
out vec4 diffuse_v;

uniform mat4 modelviewprojection;

void main()
{
  texcoord_v = texcoord;
  diffuse_v = diffuse;
  gl_Position = modelviewprojection * vec4(position, 1.0);
}
)";

const char default_frag_source[] = R"(#version 330 core

uniform sampler2D diffuse_texture;

in vec2 texcoord_v;
in vec4 diffuse_v;

layout(location = 0) out vec4 fragRGBAf;

void main()
{
  fragRGBAf = texture(diffuse_texture, texcoord_v) * diffuse_v;
}
)";

} // namespace

GraphicsContext::GraphicsContext() :
  m_size(640, 480),
  m_cliprects(),
  m_default_shader(),
  m_white_texture(),
  m_modelview_stack(),
  m_projection(1.0f),
  m_vertex_arrays(*this)
{
  assert_gl();

  m_modelview_stack.emplace(1.0f);

  m_default_shader = ShaderProgram::from_string(default_vert_source,
                                                default_frag_source);

  m_white_texture = Texture::create(SoftwareSurface::create(surf::PixelFormat::RGBA8, geom::isize(1, 1),
                                                            surf::palette::white));

  glUseProgram(m_default_shader->get_handle());

  assert_gl();
}

GraphicsContext::~GraphicsContext()
{
}

void
GraphicsContext::clear(surf::Color const& color)
{
  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void
GraphicsContext::draw_line(const geom::fline& line, const surf::Color& color)
{
  draw_line(line.p1, line.p2, color);
}

void
GraphicsContext::draw_line_with_normal(const geom::fline& line, const surf::Color& color)
{
  glm::vec2 normal = (line.p2.as_vec() - line.p1.as_vec());

  normal = glm::vec2(-normal.y, normal.x);
  normal = glm::normalize(normal);
  normal *= -32.0f;

  glm::vec2 p3 = line.p1.as_vec() + 0.5f * (line.p2.as_vec() - line.p1.as_vec());

  draw_line(line,   color);
  draw_line(p3, p3 + normal, surf::Color(0.0f, 1.0f, 1.0f));
}

void
GraphicsContext::draw_line(const geom::fpoint& pos1, const geom::fpoint& pos2, const surf::Color& color)
{
  VertexArrayDrawable va;

  //state.enable(GL_LINE_SMOOTH);
  va.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  va.set_mode(GL_LINES);

  va.color(color);
  va.vertex(pos1.x(), pos1.y());

  va.color(color);
  va.vertex(pos2.x(), pos2.y());

  va.render(*this, ~0u);
}

void
GraphicsContext::fill_quad(const geom::fquad& quad, const surf::Color& color)
{
  VertexArrayDrawable va;

  va.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  va.set_mode(GL_TRIANGLE_FAN);

  va.color(color);
  va.vertex(quad.p1.x(), quad.p1.y());

  va.color(color);
  va.vertex(quad.p2.x(), quad.p2.y());

  va.color(color);
  va.vertex(quad.p3.x(), quad.p3.y());

  va.color(color);
  va.vertex(quad.p4.x(), quad.p4.y());

  va.render(*this, ~0u);
}

void
GraphicsContext::draw_quad(const geom::fquad& quad, const surf::Color& color)
{
  VertexArrayDrawable va;

  va.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  va.set_mode(GL_LINE_LOOP);

  va.color(color);
  va.vertex(quad.p1.x(), quad.p1.y());

  va.color(color);
  va.vertex(quad.p2.x(), quad.p2.y());

  va.color(color);
  va.vertex(quad.p3.x(), quad.p3.y());

  va.color(color);
  va.vertex(quad.p4.x(), quad.p4.y());

  va.render(*this, ~0u);
}

void
GraphicsContext::fill_rect(const geom::frect& rect, const surf::Color& color)
{
  VertexArrayDrawable va;

  va.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  va.set_mode(GL_TRIANGLE_FAN);

  va.color(color);
  va.vertex(rect.left(),  rect.top());

  va.color(color);
  va.vertex(rect.right(), rect.top());

  va.color(color);
  va.vertex(rect.right(), rect.bottom());

  va.color(color);
  va.vertex(rect.left(),  rect.bottom());

  va.render(*this, ~0u);
}

void
GraphicsContext::draw_rect(const geom::frect& rect, const surf::Color& color)
{
  VertexArrayDrawable va;

  va.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  va.set_mode(GL_LINE_LOOP);

  va.color(color);
  va.vertex(rect.left(),  rect.top());

  va.color(color);
  va.vertex(rect.right(), rect.top());

  va.color(color);
  va.vertex(rect.right(), rect.bottom());

  va.color(color);
  va.vertex(rect.left(),  rect.bottom());

  va.render(*this, ~0u);
}

void
GraphicsContext::fill_rounded_rect(const geom::frect& rect, float radius, const surf::Color& color)
{
  // Keep radius in the limits, so that we get a circle instead of
  // just graphic junk
  radius = std::min(radius, std::min(rect.width()/2, rect.height()/2));

  // inner rectangle
  geom::frect irect(rect.left()    + radius,
                    rect.top()     + radius,
                    rect.right()   - radius,
                    rect.bottom()  - radius);

  VertexArrayDrawable va;

  va.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  int n = 8;
  va.set_mode(GL_TRIANGLE_STRIP);
  for(int i = 0; i <= n; ++i)
  {
    float x = sinf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;
    float y = cosf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;

    // v2
    va.color(color);
    va.vertex(irect.right() + x, irect.top() - y);
    // v1
    va.color(color);
    va.vertex(irect.left()  - x, irect.top() - y);
  }
  for(int i = 0; i <= n; ++i)
  {
    float x = cosf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;
    float y = sinf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;

    // v2
    va.color(color);
    va.vertex(irect.right() + x, irect.bottom() + y);
    // v1
    va.color(color);
    va.vertex(irect.left()  - x, irect.bottom() + y);
  }
  va.render(*this, ~0u);
}

void
GraphicsContext::draw_rounded_rect(const geom::frect& rect, float radius, const surf::Color& color)
{
  // Keep radius in the limits, so that we get a circle instead of
  // just graphic junk
  radius = std::min(radius, std::min(rect.width()/2, rect.height()/2));

  // inner rectangle
  geom::frect irect(rect.left()    + radius,
                    rect.top()     + radius,
                    rect.right()   - radius,
                    rect.bottom()  - radius);

  VertexArrayDrawable va;

  // state.enable(GL_LINE_SMOOTH);
  va.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  int n = 4;
  va.set_mode(GL_LINE_STRIP);
  for(int i = 0; i <= n; ++i)
  {
    float x = sinf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;
    float y = cosf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;

    va.color(color);
    va.vertex(irect.left()  - x, irect.top() - y);
  }
  for(int i = 0; i <= n; ++i)
  {
    float x = cosf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;
    float y = sinf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;

    va.color(color);
    va.vertex(irect.left()  - x, irect.bottom() + y);
  }

  for(int i = 0; i <= n; ++i)
  {
    float x = sinf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;
    float y = cosf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;

    va.color(color);
    va.vertex(irect.right() + x, irect.bottom() + y);
  }

  for(int i = 0; i <= n; ++i)
  {
    float x = cosf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;
    float y = sinf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;

    va.color(color);
    va.vertex(irect.right() + x, irect.top() - y);
  }
  // go back to start
  va.color(color);
  va.vertex(irect.left(), irect.top() - radius);

  va.render(*this, ~0u);
}

void
GraphicsContext::draw_circle(const geom::fpoint& pos, float radius, const surf::Color& color, int segments)
{
  assert(segments >= 0);

  VertexArrayDrawable va;

  //state.enable(GL_LINE_SMOOTH);
  va.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  float n = static_cast<float>(segments) / 4.0f;
  va.set_mode(GL_LINE_STRIP);

  va.color(color);
  va.vertex(radius + pos.x(), pos.y());
  for(int i = 1; i < segments; ++i)
  {
    float x = cosf(static_cast<float>(i) * glm::half_pi<float>() / n) * radius;
    float y = sinf(static_cast<float>(i) * glm::half_pi<float>() / n) * radius;

    va.color(color);
    va.vertex(x + pos.x(), y + pos.y());
  }
  va.color(color);
  va.vertex(radius + pos.x(), pos.y());

  va.render(*this, ~0u);
}

void
GraphicsContext::fill_circle(const geom::fpoint& pos, float radius, const surf::Color& color, int segments)
{
  assert(segments >= 0);

  VertexArrayDrawable va;

  va.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  float n = static_cast<float>(segments) / 4.0f;
  va.set_mode(GL_TRIANGLE_FAN);

  va.color(color);
  va.vertex(pos.x(), pos.y());

  va.color(color);
  va.vertex(radius + pos.x(), pos.y());
  for(int i = 1; i < segments; ++i) {
    float x = cosf(static_cast<float>(i) * glm::half_pi<float>() / n) * radius;
    float y = sinf(static_cast<float>(i) * glm::half_pi<float>() / n) * radius;

    va.color(color);
    va.vertex(x + pos.x(), y + pos.y());
  }
  va.color(color);
  va.vertex(radius + pos.x(), pos.y());

  va.render(*this, ~0u);
}

void
GraphicsContext::draw_arc(const geom::fpoint& pos, float radius, float start, float end, const surf::Color& color, int segments)
{
  assert(segments >= 0);

  if (fabsf(end - start) >= 360.0f)
  {
    draw_circle(pos, radius, color, segments);
  }
  else
  {
    float step  = glm::two_pi<float>() / static_cast<float>(segments);

    if (start > end)
      std::swap(start, end);

    VertexArrayDrawable va;

    va.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    start = glm::radians(start);
    end   = glm::radians(end);

    va.set_mode(GL_LINE_STRIP);

    va.color(color);
    va.vertex(pos.x(), pos.y());

    for(float angle = start; angle < end; angle += step) {
      va.color(color);
      va.vertex((cosf(angle) * radius) + pos.x(),
                (sinf(angle) * radius) + pos.y());
    }

    va.color(color);
    va.vertex((cosf(end) * radius) + pos.x(),
              (sinf(end) * radius) + pos.y());

    va.color(color);
    va.vertex(pos.x(), pos.y());

    va.render(*this, ~0u);
  }
}

void
GraphicsContext::fill_arc(const geom::fpoint& pos, float radius, float start, float end, const surf::Color& color, int segments)
{
  assert(segments >= 0);

  if (fabsf(end - start) >= 360.0f)
  {
    fill_circle(pos, radius, color, segments);
  }
  else
  {
    float step  = glm::two_pi<float>() / static_cast<float>(segments);

    if (start > end)
      std::swap(start, end);

    VertexArrayDrawable va;

    va.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    start = glm::radians(start);
    end   = glm::radians(end);

    va.set_mode(GL_TRIANGLE_FAN);

    va.color(color);
    va.vertex(pos.x(), pos.y());

    for(float angle = start; angle < end; angle += step) {
      va.color(color);
      va.vertex((cosf(angle) * radius) + pos.x(),
                (sinf(angle) * radius) + pos.y());
    }

    va.color(color);
    va.vertex(cosf(end) * radius + pos.x(),
              sinf(end) * radius + pos.y());

    va.render(*this, ~0u);
  }
}

void
GraphicsContext::draw_grid(const geom::fpoint& offset, const geom::fsize& size, const surf::Color& rgba)
{
  VertexArrayDrawable va;

  va.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  va.set_mode(GL_LINES);
  //glsurf::Color4ub(rgba.r, rgba.g, rgba.b, rgba.a);

  float start_x = fmodf(offset.x(), size.width());
  float start_y = fmodf(offset.y(), size.height());

  for(float x = start_x; x < static_cast<float>(m_size.width()); x += size.width())
  {
    va.color(rgba);
    va.vertex(x, 0.0f);

    va.color(rgba);
    va.vertex(x, static_cast<float>(m_size.height()));
  }

  for(float y = start_y; y < static_cast<float>(m_size.height()); y += size.height())
  {
    va.color(rgba);
    va.vertex(0.0f, y);

    va.color(rgba);
    va.vertex(static_cast<float>(m_size.width()), y);
  }

  va.render(*this);
}

void
GraphicsContext::push_cliprect(const geom::irect& rect_)
{
  assert_gl();

  geom::irect rect = rect_;

  if (!m_cliprects.empty())
  {
    rect = geom::irect(std::max(rect.left(), m_cliprects.back().left()),
                       std::max(rect.top(),  m_cliprects.back().top()),
                       std::min(rect.right(),  m_cliprects.back().right()),
                       std::min(rect.bottom(), m_cliprects.back().bottom()));
  }

  m_cliprects.push_back(rect);

  glScissor(rect.left(), size().height() - rect.top() - rect.height(),
            rect.width(), rect.height());
  glEnable(GL_SCISSOR_TEST);

  assert_gl();
}

void
GraphicsContext::pop_cliprect()
{
  assert_gl();

  assert(!m_cliprects.empty());

  m_cliprects.pop_back();

  if (!m_cliprects.empty())
  {
    const geom::irect& rect = m_cliprects.back();

    glScissor(rect.left(), size().height() - rect.top() - rect.height(),
              rect.width(), rect.height());
  }
  else
  {
    glDisable(GL_SCISSOR_TEST);
  }

  assert_gl();
}

geom::isize
GraphicsContext::size() const
{
  return m_size;
}

void
GraphicsContext::push_framebuffer(FramebufferPtr framebuffer)
{
  assert_gl();

  if (framebuffers.empty()) {
    g_default_framebuffer = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &g_default_framebuffer);
  }

  framebuffers.push_back(framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.back()->get_handle());
  assert_gl();
}

void
GraphicsContext::pop_framebuffer()
{
  assert_gl();

  assert(!framebuffers.empty());

  framebuffers.pop_back();

  if (!framebuffers.empty())
  {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.back()->get_handle());
  }
  else
  {
    glBindFramebuffer(GL_FRAMEBUFFER, g_default_framebuffer);
  }

  assert_gl();
}

FramebufferPtr
GraphicsContext::get_framebuffer()
{
  if (framebuffers.empty())
    return FramebufferPtr();
  else
    return framebuffers.back();
}

void
GraphicsContext::set_ortho(geom::isize const& size)
{
  m_size = size;

  set_projection(
    glm::ortho(0.0f,
               static_cast<float>(size.width()),
               static_cast<float>(size.height()),
               0.0f,
               1000.0f,
               -1000.0f));
}

void
GraphicsContext::set_projection(glm::mat4 const& mat)
{
  m_projection = mat;
}

void
GraphicsContext::set_modelview(glm::mat4 const& mat)
{
  m_modelview_stack.top() = mat;
}

void
GraphicsContext::push_matrix()
{
  m_modelview_stack.push(m_modelview_stack.top());
}

void
GraphicsContext::pop_matrix()
{
  m_modelview_stack.pop();
}

void
GraphicsContext::mult_matrix(glm::mat4 const& mat)
{
  m_modelview_stack.top() = m_modelview_stack.top() * mat;
}

void
GraphicsContext::translate(float x, float y, float z)
{
  m_modelview_stack.top() = glm::translate(m_modelview_stack.top(),
                                           glm::vec3(x, y, z));
}

void
GraphicsContext::scale(float x, float y, float z)
{
  m_modelview_stack.top() = glm::scale(m_modelview_stack.top(),
                                       glm::vec3(x, y, z));
}

void
GraphicsContext::rotate(float degree, float x, float y, float z)
{
  m_modelview_stack.top() = glm::rotate(m_modelview_stack.top(),
                                        glm::radians(degree), glm::vec3(x, y, z));
}

} // namespace wstdisplay

/* EOF */
