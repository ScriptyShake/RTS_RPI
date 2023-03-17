#include "Surface.h"

#include <cmath>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>
#include <glm/vec2.hpp>

#include "stb_image.h"
#include "typedefs.h"

namespace Tmpl9
{
  Surface::Surface(int w, int h, pixel* buffer) : pixels(buffer), width(w), height(h)
  {
  }

  Surface::Surface(int w, int h) : width(w), height(h)
  {
    pixels = new pixel[h * w]{ 0 };
    ownBuffer = true;
  }

  Surface::Surface(const char* file) : pixels(0), width(0), height(0)
  {
    LoadImage(file);
  }

  Surface::~Surface()
  {
    if (ownBuffer)
    {
      delete[] pixels;
    }
  }

  void Surface::LoadImage(const char* file)
  {
    if (!pixels && ownBuffer)
    {
      delete[] pixels;
      pixels = nullptr;
    }

    int n;
    unsigned char* data = stbi_load(file, &width, &height, &n, 0);
    if (data)
    {
      pixels = new pixel[height * width];
      ownBuffer = true;
      const int size = width * height;
      if (n == 1)
      {
        for (int i = 0; i < size; ++i)
        {
          const unsigned char pixel = data[i];
          pixels[i] = pixel + (pixel << 8) + (pixel << 16);
        }
      }
      else
      {
        for (int i = 0; i < size; ++i)
        {
          pixels[i] = (data[i * n + 0]) + (data[i * n + 1] << 8) + (data[i * n + 2] << 16);
        }
      }
    }
    stbi_image_free(data);
    ownBuffer = true;
  }

  void Surface::Clear(pixel c)
  {
    const int s = width * height;
    for (int i = 0; i < s; ++i)
    {
      pixels[i] = c;
    }
  }

#define OUTCODE(x,y) (((x)<xmin)?1:(((x)>xmax)?2:0))+(((y)<ymin)?4:(((y)>ymax)?8:0))

  void Surface::Line(float x1, float y1, float x2, float y2, pixel c)
  {
    // clip (Cohen-Sutherland, https://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm)
    const float xmin = 0, ymin = 0, xmax = static_cast<float>(width) - 1, ymax = static_cast<float>(height) - 1;
    int c0 = OUTCODE(x1, y1), c1 = OUTCODE(x2, y2);
    bool accept = false;
    while (1)
    {
      if (!(c0 | c1)) { accept = true; break; }
      else if (c0 & c1) break; else
      {
        float x, y;
        const int co = c0 ? c0 : c1;
        if (co & 8) x = x1 + (x2 - x1) * (ymax - y1) / (y2 - y1), y = ymax;
        else if (co & 4) x = x1 + (x2 - x1) * (ymin - y1) / (y2 - y1), y = ymin;
        else if (co & 2) y = y1 + (y2 - y1) * (xmax - x1) / (x2 - x1), x = xmax;
        else if (co & 1) y = y1 + (y2 - y1) * (xmin - x1) / (x2 - x1), x = xmin;
        if (co == c0) x1 = x, y1 = y, c0 = OUTCODE(x1, y1);
        else x2 = x, y2 = y, c1 = OUTCODE(x2, y2);
      }
    }
    if (!accept) return;
    float b = x2 - x1;
    float h = y2 - y1;
    float l = fabsf(b);
    if (fabsf(h) > l) l = fabsf(h);
    int il = static_cast<int>(l);
    float dx = b / (float)l;
    float dy = h / (float)l;
    for (int i = 0; i <= il; i++)
    {
      *(pixels + static_cast<int>(x1) + static_cast<int>(y1) * width) = c;
      x1 += dx, y1 += dy;
    }
  }

  void Surface::Plot(int x, int y, pixel c)
  {
    if (x < 0 || y < 0 || x >= width || y >= height)
    {
      return;
    }
    pixels[x + y * width] = c;
  }

  void Surface::CopyTo(Surface* d, int x, int y) const
  {
    uint* dst = d->pixels;
    uint* src = pixels;
    if ((src) && (dst))
    {
      const int srcWidth = width;
      const int dstWidth = d->width;

      const int srcHeight = height;
      const int dstHeight = d->height;

      if (x > dstWidth || y > dstHeight || x + srcWidth < 0 || y + srcHeight < 0)
        return;

      int2 srcStart = { 0.0f, 0.0f };
      int2 drawDist = { 0.0f, 0.0f };

      if (y < 0)
      {
        srcStart.y = -y;
      }
      if (x < 0)
      {
        srcStart.x = -x;
      }


      x = x < 0 ? 0 : x;
      y = y < 0 ? 0 : y;

      drawDist.x = srcWidth - srcStart.x < dstWidth - x ? srcWidth - srcStart.x : dstWidth - x;
      drawDist.y = srcHeight - srcStart.y < dstHeight - y ? srcHeight - srcStart.y : dstHeight - y;

      dst += x + y * dstWidth;
      src += srcStart.x + srcStart.y * width;

      for (int i = 0; i < drawDist.y; ++i)
      {
        memcpy(dst, src, sizeof(uint) * drawDist.x);
        dst += dstWidth;
        src += width;
      }
    }
  }

  void Surface::Box(int x1, int y1, int x2, int y2, pixel c)
  {
    Line(static_cast<float>(x1), static_cast<float>(y1), static_cast<float>(x2), static_cast<float>(y1), c);
    Line(static_cast<float>(x2), static_cast<float>(y1), static_cast<float>(x2), static_cast<float>(y2), c);
    Line(static_cast<float>(x1), static_cast<float>(y2), static_cast<float>(x2), static_cast<float>(y2), c);
    Line(static_cast<float>(x1), static_cast<float>(y1), static_cast<float>(x1), static_cast<float>(y2), c);
  }

  void Surface::Bar(int x1, int y1, int x2, int y2, pixel c)
  {
    uint* a = pixels + (y1 > 0 ? y1 : 0) * width;
    for (int y = y1 > 0 ? y1 : 0; y <= y2 && y < height; y++)
    {
      for (int x = x1 > 0 ? x1 : 0; x <= x2 && x < width; x++) a[x] = c;
      a += width;
    }
  }

  Sprite::Sprite(Surface* surface, unsigned frames) :
    width(surface->GetResolution().x / frames),
    height(surface->GetResolution().y),
    numFrames(frames),
    currentFrame(0),
    flags(0),
    start(new unsigned int* [numFrames]),
    surface(surface)
  {
    InitializeStartData();
  }

  Sprite::Sprite(const char* file, unsigned frames) :
    numFrames(frames),
    currentFrame(0),
    flags(0),
    start(new unsigned int* [numFrames]),
    surface(new Surface(file)),
    ownBuffer(true)
  {
    width = surface->GetResolution().x;
    height = surface->GetResolution().y;

    InitializeStartData();
  }

  Sprite::~Sprite()
  {
    if (ownBuffer)
      delete surface;
    for (unsigned int i = 0; i < numFrames; ++i)
    {
      delete start[i];
    }
    delete start;
  }

  void Sprite::Draw(Surface* target, const int a_X, const int a_Y) const
  {
    if ((a_X < -width) || (a_X > (target->GetResolution().x + width))) return;
    if ((a_Y < -height) || (a_Y > (target->GetResolution().y + height))) return;
    int x1 = a_X, x2 = a_X + width;
    int y1 = a_Y, y2 = a_Y + height;
    uint* src = GetBuffer() + currentFrame * width;
    if (x1 < 0) src += -x1, x1 = 0;
    if (x2 > target->GetResolution().x) x2 = target->GetResolution().x;
    if (y1 < 0) src += -y1 * width * numFrames, y1 = 0;
    if (y2 > target->GetResolution().y) y2 = target->GetResolution().y;
    uint* dest = target->GetBuffer();
    int xs;
    const int dpitch = target->GetResolution().x;
    if ((x2 > x1) && (y2 > y1))
    {
      unsigned int addr = y1 * dpitch + x1;
      const int w = x2 - x1;
      const int h = y2 - y1;
      for (int y = 0; y < h; y++)
      {
        const int line = y + (y1 - a_Y);
        const int lsx = start[currentFrame][line] + a_X;
        xs = (lsx > x1) ? lsx - x1 : 0;
        for (int x = xs; x < w; x++)
        {
          const uint c1 = *(src + x);
          if (c1 & 0xffffff) *(dest + addr + x) = c1;
        }
        addr += dpitch;
        src += width * numFrames;
      }
    }
  }

  void Sprite::InitializeStartData()
  {
    for (unsigned int f = 0; f < numFrames; ++f)
    {
      start[f] = new unsigned int[height];
      for (int y = 0; y < height; ++y)
      {
        start[f][y] = width;
        const uint* addr = GetBuffer() + f * width + y * width * numFrames;
        for (int x = 0; x < width; ++x) if (addr[x])
        {
          start[f][y] = x;
          break;
        }
      }
    }
  }

  GLTexture::GLTexture(uint w, uint h, uint type)
  {
    width = w;
    height = h;
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    if (type == DEFAULT)
    {
      // regular texture
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else if (type == INTTARGET)
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    }
    else /* type == FLOAT */
    {
      // floating point texture
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  GLTexture::~GLTexture()
  {
    glDeleteTextures(1, &ID);
  }

  void GLTexture::Bind(uint slot)
  {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, ID);
  }

  void GLTexture::CopyFrom(Surface* scr)
  {
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, scr->GetBuffer());
  }

  Shader::Shader(const char* vertexFile, const char* fragmentFile)
  {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
      vShaderFile.open(vertexFile);
      fShaderFile.open(fragmentFile);
      std::stringstream vShaderStream, fShaderStream;

      vShaderStream << vShaderFile.rdbuf();
      fShaderStream << fShaderFile.rdbuf();

      vertexCode = vShaderStream.str();
      fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
      std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
  }

  Shader::~Shader()
  {
    glDeleteProgram(ID);
  }

  void Shader::Bind()
  {
    glUseProgram(ID);
  }

  void Shader::unBind()
  {
    glUseProgram(0);
  }

  void Shader::setBool(const char* name, bool value) const
  {
    glUniform1i(glGetUniformLocation(ID, name), (int)value);
  }

  void Shader::SetInt(const char* name, int value) const
  {
    glUniform1i(glGetUniformLocation(ID, name), value);
  }

  void Shader::SetFloat(const char* name, float value) const
  {
    glUniform1f(glGetUniformLocation(ID, name), value);
  }

  void Shader::SetInputTexture(uint slot, const char* name, const GLTexture* texture) const
  {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture->ID);
    glUniform1i(glGetUniformLocation(ID, name), slot);
  }

  void Shader::checkCompileErrors(unsigned shader, const std::string& type)
  {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
      }
    }
    else
    {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if (!success)
      {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
      }
    }
  }
}
