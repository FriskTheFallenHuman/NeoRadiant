#include "JPEGLoader.h"

#include "stb_image.h"
#include "ifilesystem.h"
#include "stream/ScopedArchiveBuffer.h"
#include "RGBAImage.h"

namespace image
{

ImagePtr JPEGLoader::load(ArchiveFile& file) const
{
    archive::ScopedArchiveBuffer buffer(file);

    int width = 0, height = 0, channels = 0;

    // Force 4-channel RGBA output regardless of the source format (RGB, grey, etc.)
    stbi_uc* pixels = stbi_load_from_memory(
        buffer.buffer,
        static_cast<int>(buffer.length),
        &width, &height,
        &channels,
        STBI_rgb_alpha   // always outputs RGBA
    );

    if (!pixels)
    {
        rError() << "JPEGLoader: failed to decode " << file.getName()
                 << " (" << stbi_failure_reason() << ")\n";
        return {};
    }

    RGBAImagePtr image(new RGBAImage(width, height));
    std::memcpy(image->getPixels(), pixels, static_cast<std::size_t>(width * height * 4));

    stbi_image_free(pixels);

    return image;
}

ImageTypeLoader::Extensions JPEGLoader::getExtensions() const
{
    Extensions extensions;
    extensions.push_back("jpg");
    extensions.push_back("jpeg");
    return extensions;
}

}
