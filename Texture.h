#pragma once

#include "Renderer.h"

class Texture {

public:
	Texture(const std::string& fp);
	~Texture();

	void bind(unsigned int slot = 0);
	void unbind();

	int getWidth();
	int getHeight();
private:
	unsigned int renderer_id;
	std::string fp;
	unsigned char* textureBuffer;
	int width, height, bitsPerPixel;
};