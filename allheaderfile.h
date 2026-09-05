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


    //display
    void bmp_open();
    void update_display();
    void save_undo_state();
    void apply_undo();
    void apply_save();


    // image manipulation function
    void apply_grayscale();
    void apply_inversion();
    void apply_brightness();
    void apply_blur();
    void apply_hflip();
    void apply_vflip();
    void apply_rotate90();
    void apply_crop();



#endif