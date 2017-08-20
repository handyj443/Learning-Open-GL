#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "shader.h"

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	//glm::vec3 Tangent;
	//glm::vec3 Bitangent;
};

struct Texture
{
	enum class Type
	{
		Diffuse,
		Specular,
		Normal,
		Height
	};
	u32 id;
	Type type;
	std::string path;
};

class Mesh
{
public:
	Mesh(const std::vector<Vertex> &vertices, const std::vector<u32> &indices,
		 const std::vector<Texture> &textures);
	~Mesh();

	void Draw(Shader shader) const;

	std::vector<Vertex> m_vertices;
	std::vector<u32> m_indices;
	std::vector<Texture> m_textures;

private:
	void SetupMesh();

	u32 m_VAO, m_VBO, m_EBO;
};

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<u32> &indices,
		   const std::vector<Texture> &textures)
	: m_vertices(vertices)
	, m_indices(indices)
	, m_textures(textures)
{
	SetupMesh();
}

Mesh::~Mesh()
{
	//glDeleteVertexArrays(1, &m_VAO);
	//glDeleteBuffers(1, &m_VBO);
	//glDeleteBuffers(1, &m_EBO);
}

void Mesh::SetupMesh()
{
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
				 &m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(u32),
				 &m_indices[0], GL_STATIC_DRAW);

	// define vertex format
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void *)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void *)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}

void Mesh::Draw(Shader shader) const
{
	unsigned int diffuseNr = 0;
	unsigned int specularNr = 0;
	for (unsigned int i = 0; i < m_textures.size(); i++)
	{
		// activate proper texture unit before binding
		glActiveTexture(GL_TEXTURE0 + i);
		// retrieve texture number (the N in diffuse_textureN)
		std::string textureName;
		std::string number;
		Texture::Type type = m_textures[i].type;
		if (type == Texture::Type::Diffuse)
		{
			textureName = "texture_diffuse";
			number = std::to_string(diffuseNr++);
		}
		else if (type == Texture::Type::Specular)
		{
			textureName = "texture_specular";
			number = std::to_string(specularNr);
		}

		shader.setInt(("material." + textureName + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// draw mesh
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}