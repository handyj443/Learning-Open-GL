#pragma once

#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.h"
#include "mesh.h"

u32 TextureFromFile(const char *path, const std::string &directory,
					bool gamma = false);

class Model
{
public:
	Model(const char *path, bool gamma = false);
	void Draw(Shader shader) const;

private:
	void loadModel(std::string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat,
											  aiTextureType aiType,
											  Texture::Type type);

	// DATA
	std::vector<Mesh> m_meshes;
	std::vector<Texture> m_texturesLoaded;
	std::string m_directory;
	bool gammaCorrection;
};

Model::Model(const char *path, bool gamma) : gammaCorrection(gamma)
{
	loadModel(path);
}

void Model::Draw(Shader shader) const
{
	for (unsigned int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].Draw(shader);
	}
}

inline void Model::loadModel(std::string path) 
{
	Assimp::Importer importer;
	const aiScene *scene
		= importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	m_directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);

}

inline void Model::processNode(aiNode *node, const aiScene *scene) 
{
	// process all the node's meshes
	for (u32 i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(processMesh(mesh, scene));
	}

	// process all the node's nodes
	for (u32 i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

inline Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	std:: vector<Vertex> vertices;
	std::vector<u32> indices;
	std::vector<Texture> textures;

	// process vertex positions, normals, and texture coordinates
	for (u32 i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vec;
		vec.x = (float)mesh->mVertices[i].x; // Position
		vec.y = (float)mesh->mVertices[i].y;
		vec.z = (float)mesh->mVertices[i].z;
		vertex.Position = vec;
		vec.x = (float)mesh->mNormals[i].x; // Normal
		vec.y = (float)mesh->mNormals[i].y;
		vec.z = (float)mesh->mNormals[i].z;
		vertex.Normal = vec;
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = (float)mesh->mTextureCoords[0][i].x; // TexCoords
			vec.y = (float)mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		vec.x = (float)mesh->mTangents[i].x; // Tangent
		vec.y = (float)mesh->mTangents[i].y;
		vec.z = (float)mesh->mTangents[i].z;
		vertex.Tangent = vec;
		vec.x = (float)mesh->mBitangents[i].x; // Bitangent
		vec.y = (float)mesh->mBitangents[i].y;
		vec.z = (float)mesh->mBitangents[i].z;
		vertex.Bitangent = vec;

		vertices.push_back(vertex);
	}

	// process indices
	for (u32 i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace &face = mesh->mFaces[i];
		for (u32 j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// process material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		// diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(
			material, aiTextureType_DIFFUSE, Texture::Type::Diffuse);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(
			material, aiTextureType_SPECULAR, Texture::Type::Specular);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// normal maps
		std::vector<Texture> normalMaps = loadMaterialTextures(
			material, aiTextureType_NORMALS, Texture::Type::Normal);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// height maps
		std::vector<Texture> heightMaps = loadMaterialTextures(
			material, aiTextureType_HEIGHT, Texture::Type::Height);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

inline std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat,
														aiTextureType aiType,
														Texture::Type type)
{
	std::vector<Texture> textures;
	for (u32 i = 0; i < mat->GetTextureCount(aiType); i++)
	{
		aiString texPath;
		mat->GetTexture(aiType, i, &texPath);
		bool skip = false;
		for (const Texture &texture : m_texturesLoaded)
		{
			if (texture.path == texPath.C_Str())
			{
				textures.push_back(texture);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			texture.id = TextureFromFile(texPath.C_Str(), m_directory);
			texture.type = type;
			texture.path = texPath.C_Str();
			textures.push_back(texture);
		}
	}
	return textures;
}


u32 TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
	std::string filename = directory + '/';
	filename += path;

	u32 textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
