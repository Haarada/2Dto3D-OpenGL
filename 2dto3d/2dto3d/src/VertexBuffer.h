#pragma once

class VertexBuffer
{
private:
	unsigned int m_RenderID;

public:
	VertexBuffer(const void* data, unsigned int size); // size in bytes
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};