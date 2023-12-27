//
// Created by Tony Adriansen on 12/14/23.
//

#include "Image.hpp"

Image Image::subImage(glm::ivec2 offset, glm::ivec2 size) const {
  Image subImage{size.x, size.y};
  subImage.pixels.resize(size.x * size.y * 4);
  for (uint32_t y = 0; y < size.y; y++) {
    for (uint32_t x = 0; x < size.x; x++) {
      uint32_t subImageIndex = (y * size.x + x) * 4;
      uint32_t imageIndex = ((y + offset.y) * width + (x + offset.x)) * 4;
      subImage.pixels[subImageIndex] = pixels[imageIndex];
      subImage.pixels[subImageIndex + 1] = pixels[imageIndex + 1];
      subImage.pixels[subImageIndex + 2] = pixels[imageIndex + 2];
      subImage.pixels[subImageIndex + 3] = pixels[imageIndex + 3];
    }
  }
  return subImage;
}
