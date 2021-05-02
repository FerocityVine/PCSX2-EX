#pragma once

#include <any>

struct gli_extent
{
    int x, y, z;
};

struct gli_texture
{
    gli_texture(const char *filename);

    void* GetData(size_t layer, size_t face, size_t level);
    gli_extent GetExtent(size_t layer);

	private:
	std::any mGliTexture;
};