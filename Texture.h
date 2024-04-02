#pragma once

#include "Renderer.h"

class Texture {

public:
	unsigned int renderer_id;

	Texture() = default;
	Texture(const std::string& fp);
	~Texture();

	void bind(unsigned int slot = 0);
	void unbind();

	int getWidth();
	int getHeight();
private:
	std::string fp;
	unsigned char* textureBuffer;
	int width, height, bitsPerPixel;
};