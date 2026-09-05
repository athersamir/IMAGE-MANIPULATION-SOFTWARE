#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include "allheaderfile.h"

#pragma pack(push, 1)
typedef struct {
    uint16_t signature;   
    uint32_t filesize;    
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t data_offset; 
} SaveBMPHeader;

typedef struct {
    uint32_t size;          
    uint32_t  width, height;        
    uint16_t planes, bits_per_pixel; 
    uint32_t compression, image_size;     
    uint32_t  x_resolution, y_resolution;   
    uint32_t colors_used, colors_important;
} SaveBMPInfoHeader;
#pragma pack(pop)

Image *current_image = NULL;
Ihandle *displayed_iup_image = NULL;
Image *backup_image = NULL;

// display function

void update_display() {
    if (!current_image) return;

    int total_pixels = current_image->width * current_image->height;
    unsigned char *raw_rgb = malloc(total_pixels * 3);
    for (int i = 0; i < total_pixels; i++) {
        raw_rgb[i*3] = current_image->data[i].r;
        raw_rgb[i*3+1] = current_image->data[i].g;
        raw_rgb[i*3+2] = current_image->data[i].b;
    }

    Ihandle *new_iup_img = IupImageRGB(current_image->width, current_image->height, raw_rgb);
    free(raw_rgb);

    int dw = current_image->width, dh = current_image->height;
    if (dw > 400 || dh > 400) {
        double scale = (400.0 / dw < 400.0 / dh) ? (400.0 / dw) : (400.0 / dh);
        char resize_value[32];
        snprintf(resize_value, sizeof(resize_value), "%dx%d", (int)(dw * scale + 0.5), (int)(dh * scale + 0.5));
        IupSetAttribute(new_iup_img, "RESIZE", resize_value);
    }

    Ihandle *display_label = IupGetHandle("MY_DISPLAY_LABEL");
    IupSetAttributeHandle(display_label, "IMAGE", new_iup_img);
    IupRefresh(display_label);

    if (displayed_iup_image) IupDestroy(displayed_iup_image);
    displayed_iup_image = new_iup_img;
}

// open function

Image* load_bmp_image(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) { printf("Failed to open file: %s\n", filename); return NULL; }

    unsigned char info[54];
    fread(info, 1, 54, f);
    int width = *(int*)&info[18], height = *(int*)&info[22], data_offset = *(int*)&info[10];

    Image *img = malloc(sizeof(Image));
    img->width = width; img->height = height;
    img->data = malloc(width * height * sizeof(Pixel));

    fseek(f, data_offset, SEEK_SET);

    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            unsigned char bgr[3];
            fread(bgr, 1, 3, f);
            img->data[y * width + x] = (Pixel){bgr[2], bgr[1], bgr[0]};
        }
    }
    fclose(f);
    printf("Successfully loaded %dx%d image.\n", width, height);
    return img;
}

void bmp_open() {
    Ihandle *file_dlg = IupFileDlg();
    IupSetAttribute(file_dlg, "DIALOGTYPE", "OPEN");
    IupSetAttribute(file_dlg, "TITLE", "Select a BMP Image");
    IupSetAttribute(file_dlg, "EXTFILTER", "BMP Images (*.bmp)|*.bmp|");
    IupPopup(file_dlg, IUP_CENTER, IUP_CENTER);
    
    if (IupGetInt(file_dlg, "STATUS") != -1) {
        char *filename = IupGetAttribute(file_dlg, "VALUE");
        if (filename) {
            if (current_image) { free(current_image->data); free(current_image); }
            if ((current_image = load_bmp_image(filename))) update_display();
        }
    }
    IupDestroy(file_dlg);
    
}

// save function

int save_bmp_image(const char *filename) {
    if (!current_image || !current_image->data) return 0;
    FILE *f = fopen(filename, "wb");
    if (!f) return 0;

    int width = current_image->width, height = current_image->height;

    uint32_t image_size = (width * 3) * height;

    SaveBMPHeader header = {0x4D42, 54 + image_size, 0, 0, 54};
    SaveBMPInfoHeader info = {40, width, height, 1, 24, 0, image_size, 2835, 2835, 0, 0};

    fwrite(&header, sizeof(SaveBMPHeader), 1, f);
    fwrite(&info, sizeof(SaveBMPInfoHeader), 1, f);

    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            Pixel p = current_image->data[y * width + x];
            unsigned char bgr[3] = { p.b, p.g, p.r };
            fwrite(bgr, 1, 3, f);
        }
    }
    fclose(f);
    printf("Successfully saved image to: %s\n", filename);
    return 1;
}

void apply_save(void) {
    if (!current_image) {
        IupMessage("Error", "No image to save.");
        return;
    }

    Ihandle *file_dlg = IupFileDlg();
    IupSetAttribute(file_dlg, "DIALOGTYPE", "SAVE");
    IupSetAttribute(file_dlg, "EXTFILTER", "BMP Images (*.bmp)|*.bmp|");
    IupSetAttribute(file_dlg, "DEFEXT", "bmp");
    IupPopup(file_dlg, IUP_CENTER, IUP_CENTER);

    if (IupGetInt(file_dlg, "STATUS") != -1) {
        char *filename = IupGetAttribute(file_dlg, "VALUE");
        if (filename) {
            char f_name[512];
            snprintf(f_name, sizeof(f_name), "%s", filename);
            int len = strlen(f_name);
            if (len < 4 || (strcasecmp(&f_name[len-4], ".bmp") != 0)) strncat(f_name, ".bmp", 511 - len);
            save_bmp_image(f_name);
        }
    }
    IupDestroy(file_dlg);
}

// undo function

void save_undo_state(void) {
    if (!current_image) return;
    if (backup_image) { free(backup_image->data); free(backup_image); }
    
    backup_image = malloc(sizeof(Image));
    backup_image->width = current_image->width;
    backup_image->height = current_image->height;
    
    int total_pixels = current_image->width * current_image->height;
    backup_image->data = malloc(total_pixels * sizeof(Pixel));
    memcpy(backup_image->data, current_image->data, total_pixels * sizeof(Pixel));
}

void apply_undo(void) {
    if (!backup_image || !current_image) { 
        IupMessage("Error", "Nothing to undo."); 
        return; 
    }

    if (current_image->width != backup_image->width || current_image->height != backup_image->height) {
        current_image->width = backup_image->width;
        current_image->height = backup_image->height;
        current_image->data = realloc(current_image->data, backup_image->width * backup_image->height * sizeof(Pixel));
    }
    memcpy(current_image->data, backup_image->data, backup_image->width * backup_image->height * sizeof(Pixel));

    free(backup_image->data); free(backup_image); backup_image = NULL;
    update_display();
    
}