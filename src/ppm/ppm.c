/**
 * PPM functionality
 * 
 * Only type P6 with max RGB value of 255 is supported, currently.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define P3 3
#define P6 6

struct ppm {
	int type;
	int width;
	int height;
	int maxval;
	unsigned char *data;
};

/**
 * Write PPM
 * 
 * Returns -1 on error, otherwise 0
 */
int write_ppm(struct ppm *image, char *filename)
{
	FILE* fp = fopen(filename, "wb");
	fprintf(fp, "P%d\n%d %d\n%d\n", image->type, image->width, image->height, image->maxval);

	fwrite(image->data, 1, 3*image->width*image->height, fp);
	fclose(fp);

	return 0;
}

/**
 * Fill an existing PPM with a solid RGB color
 */
void fill_ppm(struct ppm *image, unsigned char *pixel)
{
	// STRETCH GOAL!!!
}

/**
 * Fill a rectangle in an existing PPM with a solid RGB color
 */
void fill_rect_ppm(struct ppm *image, int row, int col, int width, int height, unsigned char *pixel)
{
	// STRETCH GOAL!!!
}

/**
 * Create PPM
 * 
 * Create an empty PPM
 */
struct ppm *create_ppm(int width, int height)
{
	struct ppm *image = malloc(sizeof(struct ppm));

	image->width = width;
	image->height = height;
	image->type = P6; // TODO allow the user to pass this in
	image->maxval = 255;

	int total_bytes = 3 * width * height;

	image->data = malloc(total_bytes);
	memset(image->data, 0, total_bytes); // Clear the image to black

	return image;
}

/**
 * Read PPM
 */
struct ppm *read_ppm(char *filename)
{
	FILE *fp;
	char buffer[128];
	int width, height;
	int max_pixel;
	struct ppm *ppm;

	fp = fopen(filename, "rb");

	if (fp == NULL) {
		return NULL;
	}

	// Read the magic number
	fgets(buffer, sizeof buffer, fp);

	// TODO check to see what the magic number is and do the right thing
	// Assume it's P6 for now

	// Read the width and height
	fgets(buffer, sizeof buffer, fp);
	sscanf(buffer, "%d %d", &width, &height);

	int total_bytes = 3 * width * height;

	// Read the max pixel value
	fgets(buffer, sizeof buffer, fp);
	sscanf(buffer, "%d", &max_pixel);

	// TODO do the right thing with this
	// Assume it's 255 for now

	// TODO handle comment lines

	// Allocate a new PPM struct
	ppm = malloc(sizeof(struct ppm));
	ppm->width = width;
	ppm->height = height;
	ppm->maxval = max_pixel;

	// Allocate room for the data
	ppm->data = malloc(total_bytes);

	// Read data into the data buffer
	int bytes_read = fread(ppm->data, 1, total_bytes, fp);

	if (bytes_read != total_bytes) {
		fprintf(stderr, "only read %d/%d bytes", bytes_read, total_bytes);
	}

	fclose(fp);

	return ppm;
}

/**
 * Deallocate a ppm
 */
void free_ppm(struct ppm *p)
{
	free(p->data);
	free(p);
}

/**
 * Get pixel
 */
unsigned char *get_pixel(struct ppm *image, int row, int col, unsigned char pixel[3])
{
	int index;

	index = (row * image->width * 3) + (col * 3);
	//index = (row * p->width + col) * 3; // same thing

	pixel[0] = image->data[index+0];
	pixel[1] = image->data[index+1];
	pixel[2] = image->data[index+2];

	return image->data + index;
}

/**
 * Put pixel
 */
void put_pixel(struct ppm *image, int row, int col, unsigned char *pixel)
{
	int index;

	index = (row * image->width * 3) + (col * 3);

	image->data[index+0] = pixel[0];
	image->data[index+1] = pixel[1];
	image->data[index+2] = pixel[2];
}

/**
 * Stamp an image
 */
void stamp_image(struct ppm *image, struct ppm *dest_image, int dest_row, int dest_col) {

	for (int row = 0; row < image->height; row++) {
		for (int col = 0; col < image->width; col++) {
			unsigned char pixel[3];

			get_pixel(image, row, col, pixel);

			put_pixel(dest_image, row + dest_row, col + dest_col, pixel);
		}
	}
}

/**
 * MAIN
 */
int main(int argc, char** argv)
{
	char *output_file = NULL;
	char *stamp = NULL;
  int index;
  int c;

  opterr = 0;

  while ((c = getopt (argc, argv, "o:")) != -1)
    switch (c) 
    {
      case 'o':
        output_file = optarg;
        break;
      case '?':
        if (optopt == 'o')
          fprintf (stderr, "Option -o requires an argument\n");
        else
          fprintf (stderr, "Unknown option '-%c'\n", optopt);
        return 1;
      default:
        abort ();
    }

	struct ppm *dest_p = create_ppm(1024, 768);

	for(index = optind; index < argc; index++) {
		stamp = argv[index];
		if(index + 2 >= argc) break;
		int row = atoi(argv[index + 1]);
		int col = atoi(argv[index + 2]);
		struct ppm *p = read_ppm(stamp);
		if (p == NULL) {
			printf("File %s not found.\n", stamp);
			continue;
		} else if (p->width + row > dest_p->width || p->height + col > dest_p->height) {
			printf("Stamp %s does not fit on image.\n", stamp);
			continue;
		}

		stamp_image(p, dest_p, row, col);
		free_ppm(p);

		index = index + 2;
	}
	
	write_ppm(dest_p, output_file);
	free_ppm(dest_p);

	return 0;
}

