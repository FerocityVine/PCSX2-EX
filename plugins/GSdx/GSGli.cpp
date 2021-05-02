
#undef countof
#include <gli.hpp>
#include <glm/glm.hpp>

#include "GSGli.h"

gli_texture::gli_texture(const char* filename)
{
    mGliTexture = gli::load(filename);
}

void* gli_texture::GetData(size_t layer, size_t face, size_t level)
{
    if (!mGliTexture.has_value())
        return nullptr;

    return std::any_cast<gli::texture>(mGliTexture).data(layer, face, level);

}

gli_extent gli_texture::GetExtent(size_t layer)
{
    if (!mGliTexture.has_value())
        return {0,0,0};

    auto gliExtent = std::any_cast<gli::texture>(mGliTexture).extent(layer);

	return {gliExtent.x, gliExtent.y, gliExtent.z};
}