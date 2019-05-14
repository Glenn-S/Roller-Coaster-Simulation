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

#include "shader.h"

#include <fstream>
#include <iostream>
#include <vector>

namespace opengl {

Shader::Shader(GLuint id, Shader::Type type) : m_id(id), m_shaderType(type) {}

Shader::~Shader() { release(); }

Shader::Shader(Shader &&other) : m_id(other.m_id), m_shaderType(other.m_shaderType) {
    other.m_id = 0;
    other.m_shaderType = Shader::INVALID;
}

Shader &Shader::operator=(Shader &&other) {
    if (this != &other) {
        release();
        swap(*this, other);
    }
    return *this;
}

void Shader::release() {
    glDeleteShader(m_id);
    m_id = 0;
    m_shaderType = Shader::INVALID;
}

Shader makeShader(std::string const &shaderCode, Shader::Type shaderTypeEnum) {
    auto shaderType = enumToShaderType(shaderTypeEnum);
    if (shaderType == Shader::INVALID) {
        return Shader(0, shaderType);
    }

    GLuint shaderID = glCreateShader(shaderType);

    if (shaderID == 0) {
        return Shader(0, Shader::INVALID);
    }

    {
        char const *source = shaderCode.c_str();
        glShaderSource(shaderID, 1, &source, nullptr);
    }

    glCompileShader(shaderID);

    if (!checkCompileStatus(shaderID)) {
        glDeleteShader(shaderID);
        return Shader(0, Shader::INVALID);
    }

    return Shader(shaderID, shaderType);
}

bool Shader::isValid() const {
    return m_id != 0 && m_shaderType != Shader::INVALID;
}

GLuint Shader::id() const { return m_id; }

Shader::Type Shader::type() const { return m_shaderType; }

void swap(Shader &lhs, Shader &rhs) {
    std::swap(lhs.m_id, rhs.m_id);
    std::swap(lhs.m_shaderType, rhs.m_shaderType);
}

Shader::Type enumToShaderType(GLenum shaderTypeEnum) {
    switch (shaderTypeEnum) {
        case Shader::VERTEX:
            return Shader::VERTEX;
            break;
        case Shader::GEOMETRY:
            return Shader::GEOMETRY;
            break;
        case Shader::FRAGMENT:
            return Shader::FRAGMENT;
            break;
        default:
            return Shader::INVALID;
            break;
    }
}

bool checkCompileStatus(GLuint shaderID) {
    GLint result;
    int infoLogLength;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<char> errorMsg(infoLogLength);
        glGetShaderInfoLog(shaderID, infoLogLength, nullptr, errorMsg.data());
        std::cerr << errorMsg.data() << std::endl;
        return false;
    }

    return true; // otherwise
}

std::string loadShaderStringFromFile(const std::string &filePath) {
    std::string shaderCode;
    std::ifstream fileStream(filePath);
    if (fileStream.good()) {
        std::string line;
        while (std::getline(fileStream, line)) {
            shaderCode += "\n" + line;
        }
        fileStream.close();
    } else {
        std::cerr << "Could Not Open File " << filePath << std::endl;
        return {""};
    }
    return shaderCode;
}

} // namespace opengl
