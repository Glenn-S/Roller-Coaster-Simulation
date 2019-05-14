/**
 * @author	Andrew Robert Owens
 * @date January, 2019
 * @brief	CPSC 587/687 Fundamental of Computer Animation
 * Organization: University of Calgary
 *
 * Contact:	arowens [at] ucalgary.ca
 *
 * Copyright (c) 2019 - Please give credit to the author.
 *
 * @file	main.cpp
 *
 * @brief
 * This is a (very) basic program to
 * 1) load shaders from external files, and make a shader program
 * 2) load a mesh (.obj)
 * 3) make Vertex Array Object (VAO) and Vertex Buffer Object (VBO) for the mesh
 * 4) load a curve that was created
 * 5) make VAO and VBO for the curve
 * 6) animate the mesh around the curve
 *
 * take a look at the following sites for further readings:
 * learnopengl.com -> AWESOME
 * opengl-tutorial.org -> The first triangle (New OpenGL, great start)
 * antongerdelan.net -> shaders pipeline explained
 * ogldev.atspace.co.uk -> good resource
 *
 * This code has been borrowed from the CPSC 587 boilerplate code
 * provided by Andrew Owens.
 */


#pragma once

#include <string>

#include <glad/glad.h>

namespace opengl {

class Shader {
public:
    enum Type : uint32_t {
        INVALID = GL_INVALID_ENUM,
        VERTEX = GL_VERTEX_SHADER,
        GEOMETRY = GL_GEOMETRY_SHADER,
        FRAGMENT = GL_FRAGMENT_SHADER
    };

    /* Explicitly remove copy/assignment (RAII) */
    Shader(Shader const &) = delete;
    Shader &operator=(Shader const &) = delete;

    /* Retain move semantics */
    Shader(Shader &&other);
    Shader &operator=(Shader &&other);

    ~Shader();

    bool isValid() const;
    GLuint id() const;
    Shader::Type type() const;

private:
    /* Only called through makeShader() factory function */
    Shader(GLuint id, Shader::Type shaderType);

    void release();

    friend Shader makeShader(std::string const &shaderCode, Shader::Type shaderType);
    friend void swap(Shader &lhs, Shader &rhs);

private:
    GLuint m_id = 0;
    Shader::Type m_shaderType = INVALID;
};

Shader makeShader(std::string const &shaderCode, GLenum shaderType);
void swap(Shader &lhs, Shader &rhs);

Shader::Type enumToShaderType(GLenum shaderTypeEnum);
bool checkCompileStatus(GLuint shaderID);

// SHADER FILE I/O
std::string loadShaderStringFromFile(std::string const &filePath);

} // namespace opengl
