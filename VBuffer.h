#pragma once

class VBuffer {

private:
	unsigned int renderer_id;
public:
	VBuffer(const void* data, unsigned int size);
	~VBuffer();

	void bind();
	void unbind();

	void update(const void* data, unsigned int size);
};