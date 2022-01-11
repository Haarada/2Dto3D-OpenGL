#include "Texture.h"
#include "offsite/stb_image.h"
Texture::Texture(const std::string& filepath)
	: m_RenderID(0), m_FilePath(filepath), m_Local_Buffer(nullptr), m_width(0), m_height(0), m_BPP(0)
{
	stbi_set_flip_vertically_on_load(1);
	m_Local_Buffer = stbi_load(filepath.c_str(), &m_width, &m_height, &m_BPP, 4);

	
}

Texture::~Texture()
{
}

void Texture::Bind(unsigned int slot) const
{
}

void Texture::Unbind() const
{
}
