#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rainbow.h"

//const int MAX_ITERATIONS = 256 ;
#define MAX_ITERATIONS 256

/*
  Represents a complex number.
  re : real part
  im : imaginary part
 */
typedef struct {
  double re;
  double im;
} complex_s;

/*
  Represents an image.
 */
typedef struct {
  unsigned int width;
  unsigned int height;
  complex_s ul; // upper_left
  complex_s br; // bottom_right
  unsigned int *bitmap; 
} image_s;

/*
  in  : c complex_s
  out : double modulus of c
 */
double modulus(complex_s c){
  return sqrt(c.re*c.re + c.im*c.im);
}

/*
  in  : c complex_s
  out : double square of modulus of c
 */
double modulus_sq(complex_s c){
  return c.re*c.re + c.im*c.im;
}

/*
  in  : c complex_s
        res (out) conjugate of c
  out : -
 */
void conjugate(complex_s c, complex_s *res){
  res->re = c.re;
  res->im =-c.im;
  return;
}

/*
  in  : c1 complex_s, c2 complex_s
        res (out) sum of c1 and c2
  out : -
 */
void sum(complex_s c1, complex_s c2, complex_s *res){
  res->re = c1.re+c2.re;
  res->im = c1.im+c2.im;
  return;
}

/*
  in  : c1 complex_s, c2 complex_s
        res (out) difference of c1 and c2
  out : -
 */
void diff(complex_s c1, complex_s c2, complex_s *res){
  res->re = c1.re-c2.re;
  res->im = c1.im-c2.im;
  return;
}

/*
  in  : c1 complex_s, c2 complex_s
        res (out) product of c1 and c2
  out : -
 */
void prod(complex_s c1, complex_s c2, complex_s *res){
  res->re = c1.re*c2.re - c1.im*c2.im;
  res->im = c1.re*c2.im + c2.re*c1.im;
  return;
}

/*
  in  : c complex_s
        res (out) square of c
  out : -
 */
void square(complex_s c, complex_s *res){
  res->re = c.re*c.re-c.im*c.im;
  res->im = 2*c.re*c.im;
  return;
}


/*
  in  :
  out :
 */
int mandelbrot(complex_s c) {
  complex_s z = {0.0, 0.0}; // z_0
  complex_s tmp;
  
  for(int n=0;n<MAX_ITERATIONS;n++){
    // if (modulus(z) > 2)
    if(modulus_sq(z) > 4)
      return n;
    square(z, &tmp);
    sum(tmp, c, &z);
  }
  return MAX_ITERATIONS;
}

/*
  in  :
  out :
 */
void mandelbrot_colors(image_s *image){
  double step_x = (image->br.re - image->ul.re) / (double)image->width;
  double step_y = (image->br.im - image->ul.im) / (double)image->height;
  complex_s c;

  for(int y=0; y < image->height; y++){
    for(int x=0; x < image->width; x++){   
      c.re = image->ul.re + ((double)x)*step_x;
      c.im = image->ul.im + ((double)y)*step_y;
      image->bitmap[x+y*image->width]=rainbow_table[mandelbrot(c)];
    }
  }
  return;
}

/*
  in  :
  out :
 */
void build_image_rgba(image_s image){
  // Open the file mandelbrot.rgba to write the colors
  FILE *file = fopen("mandelbrot.rgba", "wb");
  if (file==NULL) {
    fprintf(stderr, "Error while openning the file mandelbrot.rgba.\n");
    return;
  }

  // Writing bitmap on the file
  size_t written_items = fwrite(image.bitmap, sizeof(unsigned int),
				image.width*image.height, file);
  if (written_items < (size_t)(image.width * image.height)) {
    fprintf(stderr, "Error while writing on the file mandelbrot.rbga.\n");
    fclose(file);
    return;
  }
  
  fclose(file);
  return;
}

/*
  in  : *image - the image to initialize
        necessary needed information
  out : -
 */
void init_image(image_s *image, unsigned int width, unsigned int height, complex_s ul, complex_s br, unsigned int *bitmap){
  image->width = width;
  image->height = height;
  image->ul = ul;
  image->br = br;
  image->bitmap = bitmap;
  return;
}

/*
  Print help message.
 */
void print_help_message(void){
  printf("./mandelbrot usage.\n");
  printf("./mandelbrot <width> <height> <ul_x> <ul_y> <br_x> <br_y>\n");
  printf("\t with\n");
  printf("\t <width>    : number of pixels per line (e.g. 3840 for UHD)\n");
  printf("\t <height>   : number of lines of pixel (e.g. 2160 for UHD)\n");
  printf("\t <ul_x>     : upper left corner abscissa, a number in [-2,1]\n");
  printf("\t <ul_y>     : upper left corner ordinate, a number in [-1,1]\n");
  printf("\t <br_x>     : lower right corner abscissa, a number in [-2,1]\n");
  printf("\t <br_y>     : lower right corner ordinate, a number in [-1,1]\n");
  return;
}

/*
  Initialize the image and computes the Mandelbrot set
 */
int main(int argc, char **argv){
  image_s my_mandelbrot;
  unsigned int *bitmap;

  if (argc==1){
    /* defaul initialization of the image parameters */
    complex_s ul = {-2,-1};
    complex_s br = {1,1};
    bitmap = (unsigned int*)malloc(3840*2160*sizeof(unsigned int));
    assert(bitmap != NULL);
    
    init_image(&my_mandelbrot, 3840, 2160, ul, br, bitmap);
  }
  else if ((argc == 2 && strcmp(argv[1],"-h")==0) || (argc != 7)) {
    print_help_message();
    return EXIT_FAILURE;
  }
  else {
    /* initialization with given parameters */
    complex_s ul = {atof(argv[3]),atof(argv[4])};
    complex_s br = {atof(argv[5]),atof(argv[6])};
    bitmap = (unsigned int*)malloc(atoi(argv[1])*atoi(argv[2])*sizeof(unsigned int));
    assert(bitmap != NULL);
    init_image(&my_mandelbrot, atoi(argv[1]), atoi(argv[2]), ul, br, bitmap);
  }

  /* completing the bitmap by colors */
  printf("Mandelbrot generation...\n");
  mandelbrot_colors(&my_mandelbrot);
  printf("Mandelbrot generation done.\n");
  
  /* creating the image file */
  build_image_rgba(my_mandelbrot);
  
  /* free the memory */
  free(bitmap);
  
  return EXIT_SUCCESS;
}
