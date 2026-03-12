#include "PNGLoader.h"

#include "stb_image.h"
#include "stream/ScopedArchiveBuffer.h"
#include "RGBAImage.h"

namespace image
{

ImagePtr PNGLoader::load(ArchiveFile& file) const
{
    archive::ScopedArchiveBuffer buffer(file);

    int width = 0, height = 0, channels = 0;

    // Force 4-channel RGBA output. This handles palette, grey, grey+alpha,
    // RGB, RGBA, and 16-bit PNG (stb_image scales 16-bit down to 8-bit for us)
    stbi_uc* pixels = stbi_load_from_memory(
        buffer.buffer,
        static_cast<int>(buffer.length),
        &width, &height,
        &channels,
        STBI_rgb_alpha   // always outputs RGBA
    );

    if (!pixels)
    {
        rError() << "PNGLoader: failed to decode " << file.getName()
                 << " (" << stbi_failure_reason() << ")\n";
        return {};
    }

    RGBAImagePtr image(new RGBAImage(width, height));
    std::memcpy(image->getPixels(), pixels, static_cast<std::size_t>(width * height * 4));

    stbi_image_free(pixels);

    return image;
}

ImageTypeLoader::Extensions PNGLoader::getExtensions() const
{
    Extensions extensions;
    extensions.push_back("png");
    return extensions;
}

}
