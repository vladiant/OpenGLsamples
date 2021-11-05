#include "image.h"

#include <assert.h>
#include <errno.h>

bool o_image_init_from_filename(struct o_image *self, const char *filename) {
  assert(self != NULL);
  assert(filename != NULL);

  bool ok = png_decoder_init_from_filename(&self->png, filename);
  if (ok) {
    self->type = O_IMAGE_TYPE_PNG;
    self->width = self->png.width;
    self->height = self->png.height;

    switch (self->png.format) {
      case PNG_COLOR_TYPE_RGB:
        self->format = O_IMAGE_FORMAT_RGB;
        break;
      case PNG_COLOR_TYPE_RGB_ALPHA:
        self->format = O_IMAGE_FORMAT_RGBA;
        break;
      default:
        printf("PNG image format %d not handled\n", self->png.format);
        return false;
    }

    return true;
  }

  printf("Unknown or unhandled image format.\n");
  return false;
}

void o_image_clear(struct o_image *self) {
  assert(self != NULL);

  if (self->type == O_IMAGE_TYPE_PNG) png_clear(&self->png);
}

ssize_t o_image_read(struct o_image *self, void *buffer, size_t size,
                     size_t *first_row, size_t *num_rows) {
  assert(self != NULL);

  switch (self->type) {
    case O_IMAGE_TYPE_PNG:
      return png_read(&self->png, buffer, size, first_row, num_rows);
    default:
      errno = ENXIO;
      return -1;
  }
}
