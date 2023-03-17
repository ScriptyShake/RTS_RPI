#include "Scene.h"
#include "camera.h"
#include "model.h"
#include "Physics.h"
#include "shaders.h"
#include "Template/stb_image.h"

Tmpl9::Scene::Scene()
{
	
}

// Draw tank model in the Tick
void Tmpl9::Scene::DrawModel(Shader2* shader, Model* obj, glm::mat4 view, glm::mat4 projection, glm::mat4 model, bool selected, bool is_enemy)
{
	shader->use();

	// Create transformations
	model = translate(model, glm::vec3(0.0f, -0.4f, -0.4f)); // translate it down so it's at the center of the scene
	model = scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); // it's a bit too big for our scene, so scale it down

	// Set uniforms
	shader->setMat4("projection", projection);
	shader->setMat4("view", view);
	shader->setMat4("model", model);

    if (selected)
    {
	     shader->setVec3("color", {1.0, 0.0, 0.0});
    }
    else if (is_enemy)
    {
	     shader->setVec3("color", {1.0, 1.0, 0.0});
    }
    else
    {
	     shader->setVec3("color", {1.0, 1.0, 1.0});
    }

	// render the loaded model
	obj->Draw(*shader);
}

unsigned Tmpl9::Scene::loadTexture(char const* path)
{
	unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

Tmpl9::Scene::~Scene()
{
	
}
