#pragma once
#include <string>
#include <GLES3/gl3.h>
#include <glm/vec2.hpp>

namespace Tmpl9
{
  using pixel = unsigned int;
  using uint = unsigned int;

  /**
   * \brief Basic surface class that stores pixel array
   */
  class Surface
  {
  public:
    Surface() = default;
    /**
     * \brief create surface using existing buffer
     * \param w width of the Surface
     * \param h height of the Surface
     * \param buffer the buffer to use
     */
    Surface(int w, int h, pixel* buffer);
    /**
     * \brief create empty Surface
     * \param w width of surface
     * \param h height of surface
     */
    Surface(int w, int h);
    /**
     * \brief create surface from file
     * \param file file to create surface from
     */
    Surface(const char* file);
    ~Surface();
    // operations
    /**
     * \brief load file and initialize buffer
     * \param file file to load
     */
    void LoadImage(const char* file);
    /**
     * \brief returns the raw pixel buffer
     * \return The Surfaces buffer
     */
    pixel* GetBuffer() const {return pixels;}
    /**
     * \brief clear the surface
     * \param c color to clear to
     */
    void Clear(pixel c);
    /**
     * \brief draw line on surface
     * \param x1 
     * \param y1 
     * \param x2 
     * \param y2 
     * \param c color
     */
    void Line(float x1, float y1, float x2, float y2, pixel c);
    /**
     * \brief draw pixel on surface
     * \param x 
     * \param y 
     * \param c color
     */
    void Plot(int x, int y, pixel c);
    /**
     * \brief copy surface to other surface
     * \param d destination surface
     * \param x 
     * \param y 
     */
    void CopyTo(Surface* d, int x, int y) const;
    /**
     * \brief draw outline of box
     * \param x1 
     * \param y1 
     * \param x2 
     * \param y2 
     * \param c color of box
     */
    void Box(int x1, int y1, int x2, int y2, pixel c);
    /**
     * \brief draw solid box
     * \param x1 
     * \param y1 
     * \param x2 
     * \param y2 
     * \param c color of bar
     */
    void Bar(int x1, int y1, int x2, int y2, pixel c);
    /**
     * \brief get surface resolution
     * \return surface resolution
     */
    glm::ivec2 GetResolution() const { return glm::ivec2{width, height};}
  private:
    pixel* pixels = nullptr;
    int width{}, height{};
    bool ownBuffer = false;
  };

  /**
   * \brief Simple sprite that divides surface into frames
   */
  class Sprite
  {
  public:
    /**
     * \brief create sprite from surface without taking ownership
     * \param surface surface to create sprite from
     * \param frames frames of sprite
     */
    Sprite(Surface* surface, unsigned int frames);
    /**
     * \brief create sprite from file
     * \param file file to create sprite from
     * \param frames frames of sprite
     */
    Sprite(const char* file, unsigned int frames);
    ~Sprite();

    /**
     * \brief draw sprite to surface
     * \param target surface to draw sprite to
     * \param a_X 
     * \param a_Y 
     */
    void Draw(Surface* target, int a_X, int a_Y) const;
    /**
     * \brief set sprite flags
     * \param flags 
     */
    void SetFlags(unsigned int flags) { this->flags = flags; }
    /**
     * \brief set frame of sprite
     * \param index frame to use
     */
    void SetFrame(unsigned int index) { currentFrame = index % numFrames; }
    /**
     * \brief get the resolution of the sprite
     * \return resolution of sprite
     */
    glm::ivec2 GetResolution() const {return glm::ivec2{width, height};}
    /**
     * \brief get the surface buffer
     * \return raw surface buffer
     */
    pixel* GetBuffer() const {return surface->GetBuffer();}
    /**
     * \brief get sprite frames
     * \return frames
     */
    unsigned int frames() const {return numFrames;}
    /**
     * \brief get the surface of the sprite
     * \return the sprite's surface
     */
    Surface* GetSurface() const {return surface;}
    /**
     * \brief magic
     */
    void InitializeStartData();
  private:
    int width, height;
    unsigned int numFrames;
    unsigned int currentFrame;
    unsigned int flags;
    unsigned int** start;
    Surface* surface;
    bool ownBuffer = false;
  };

  /**
   * \brief A GL texture
   */
  class GLTexture
  {
  public:
    enum { DEFAULT = 0, FLOAT = 1, INTTARGET = 2};

    /**
     * \brief create GL texture
     * \param w 
     * \param h 
     * \param type 
     */
    GLTexture(uint w, uint h, uint type = DEFAULT);
    ~GLTexture();

    /**
     * \brief bind texture
     * \param slot texture slot to bind to
     */
    void Bind(uint slot = 0);
    /**
     * \brief create texture from surface
     * \param scr surface to create texture from
     */
    void CopyFrom(Surface* scr);

    GLuint ID = 0;
    uint width = 0, height = 0;
  };

  /**
   * \brief basic shader that accepts a vertex and fragment shader file
   */
  class Shader
  {
  public:
    /**
     * \brief create Shader program from files
     * \param vertexFile vertex shader
     * \param fragmentFile fragment shader
     */
    Shader(const char* vertexFile, const char* fragmentFile);
    ~Shader();

    /**
     * \brief bind program
     */
    void Bind();
    /**
     * \brief unbind program
     */
    void unBind();
    /**
     * \brief set boolean
     * \param name name of variable
     * \param value 
     */
    void setBool(const char* name, bool value) const;
    /**
     * \brief set integer
     * \param name name of variable
     * \param value 
     */
    void SetInt(const char* name, int value) const;
    /**
     * \brief set float
     * \param name name of variable
     * \param value 
     */
    void SetFloat(const char* name, float value) const;
    /**
     * \brief bind texture
     * \param slot slot to bind texture to
     * \param name name of variable
     * \param texture texture to bind
     */
    void SetInputTexture(uint slot, const char* name, const GLTexture* texture) const;
  private:
    static void checkCompileErrors(unsigned int shader, const std::string& type);
    unsigned int ID;
  };
}
