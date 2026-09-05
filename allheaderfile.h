#ifndef IMAGE_H
#define IMAGE_H

#include <iup.h>
#include <stdint.h> 

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Pixel;

typedef struct {
    int width;
    int height;
    Pixel *data;
} Image;

extern Image *current_image;
extern Ihandle *displayed_iup_image;
extern Image *backup_image;

Image* load_bmp_image(const char *filename);

void bmp_open();

void update_display();
void save_undo_state(void);
void apply_undo(void);
void apply_save(void);


// image manipulation function
void apply_grayscale();
void apply_inversion(void);
void apply_brightness(void);
void apply_blur(void);
void apply_hflip(void);
void apply_vflip(void);
void apply_rotate90(void);
void apply_crop(void);



#endif