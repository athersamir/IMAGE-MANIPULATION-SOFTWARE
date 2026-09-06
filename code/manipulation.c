#include <stdlib.h>
#include <stdio.h>
#include "allheaderfile.h"


    void apply_grayscale() {
        if (current_image == NULL){
            IupMessage("Error", "Please open an image first.");
            return;
        } 

        save_undo_state(); 

        int total_pixels = current_image->width * current_image->height;
        for (int i = 0; i < total_pixels; i++) {
            unsigned char r = current_image->data[i].r;
            unsigned char g = current_image->data[i].g;
            unsigned char b = current_image->data[i].b;

            unsigned char gray = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);

            current_image->data[i].r = gray;
            current_image->data[i].g = gray;
            current_image->data[i].b = gray;
        }

        update_display();
    }



    void apply_inversion() {

        if (current_image == NULL) {
            IupMessage("Error", "Please open an image first.");
            return; 
        }

        save_undo_state(); 

        int total_pixels = current_image->width * current_image->height;

        for (int i = 0; i < total_pixels; i++) {
            current_image->data[i].r = 255 - current_image->data[i].r;
            current_image->data[i].g = 255 - current_image->data[i].g;
            current_image->data[i].b = 255 - current_image->data[i].b;
        }

        update_display();
    }


    void apply_hflip() {
        if (current_image == NULL) {
            IupMessage("Error", "Please open an image first.");
            return;
        }

        save_undo_state();

        int width = current_image->width;
        int height = current_image->height;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width / 2; x++) {
                int left_idx = y * width + x;
                int right_idx = y * width + (width - 1 - x);

                
                Pixel temp = current_image->data[left_idx];
                current_image->data[left_idx] = current_image->data[right_idx];
                current_image->data[right_idx] = temp;
            }
        }

        update_display();
    }

    void apply_vflip() {
        if (current_image == NULL) {
            IupMessage("Error", "Please open an image first.");
            return;
        }

        save_undo_state();

        int width = current_image->width;
        int height = current_image->height;

        for (int y = 0; y < height / 2; y++) {
            for (int x = 0; x < width; x++) {
                int top_idx = y * width + x;
                int bottom_idx = (height - 1 - y) * width + x;

                Pixel temp = current_image->data[top_idx];
                current_image->data[top_idx] = current_image->data[bottom_idx];
                current_image->data[bottom_idx] = temp;
            }
        }

        update_display();
    }

    void apply_rotate90() {
        if (current_image == NULL) {
            IupMessage("Error", "Please open an image first.");
            return;
        }

        save_undo_state();

        int old_width = current_image->width;
        int old_height = current_image->height;

        int new_width = old_height;
        int new_height = old_width;

        Pixel *new_data = (Pixel*)malloc(new_width * new_height * sizeof(Pixel));

        for (int y = 0; y < old_height; y++) {
            for (int x = 0; x < old_width; x++) {
                int old_idx = y * old_width + x;
                
                int new_x = old_height - 1 - y;
                int new_y = x;
                int new_idx = new_y * new_width + new_x;

                new_data[new_idx] = current_image->data[old_idx];
            }
        }

        free(current_image->data);
        current_image->data = new_data;
        current_image->width = new_width;
        current_image->height = new_height;

        update_display();
    }


    void apply_brightness() {
        if (current_image == NULL) {
            IupMessage("Error", "Please open an image first.");
            return;
        }

        int adjustment = 0;

        if (!IupGetParam("Brightness Adjustment", NULL, NULL,
                        "Brightness amount (-255 to 255): %i\n",
                        &adjustment, NULL)) {
            return;
        }

        if (adjustment < -255 || adjustment > 255) {
            IupMessage("Error", "Brightness must be between -255 and 255.");
            return;
        }

        save_undo_state();

        int total_pixels = current_image->width * current_image->height;

        for (int i = 0; i < total_pixels; i++) {
            int r = current_image->data[i].r + adjustment;
            int g = current_image->data[i].g + adjustment;
            int b = current_image->data[i].b + adjustment;

            current_image->data[i].r = (unsigned char)(r < 0 ? 0 : (r > 255 ? 255 : r));
            current_image->data[i].g = (unsigned char)(g < 0 ? 0 : (g > 255 ? 255 : g));
            current_image->data[i].b = (unsigned char)(b < 0 ? 0 : (b > 255 ? 255 : b));
        }

        update_display();
    }


    void apply_crop() {
        if (current_image == NULL) {
            IupMessage("Error", "Please open an image first.");
            return;
        }

        int crop_x = 0;
        int crop_y = 0;
        int crop_w = current_image->width / 2;
        int crop_h = current_image->height / 2;

        if (!IupGetParam("Crop Region", NULL, NULL,
                        "Start X: %i\n"
                        "Start Y: %i\n"
                        "Width: %i\n"
                        "Height: %i\n",
                        &crop_x, &crop_y, &crop_w, &crop_h, NULL)) {
            return;
        }

        if (crop_x < 0 || crop_y < 0 || crop_w <= 0 || crop_h <= 0 ||
            crop_x + crop_w > current_image->width ||
            crop_y + crop_h > current_image->height) {
            IupMessage("Error", "Crop region is outside the image boundary.");
            return;
        }

        save_undo_state();

        Pixel *new_data = malloc((size_t)crop_w * crop_h * sizeof(Pixel));
        if (new_data == NULL) {
            IupMessage("Error", "Not enough memory for crop operation.");
            return;
        }

        for (int y = 0; y < crop_h; y++) {
            for (int x = 0; x < crop_w; x++) {
                new_data[y * crop_w + x] =
                    current_image->data[(crop_y + y) * current_image->width + (crop_x + x)];
            }
        }

        free(current_image->data);
        current_image->data = new_data;
        current_image->width = crop_w;
        current_image->height = crop_h;

        update_display();
    }

    void apply_blur() {
        if (current_image == NULL) {
            IupMessage("Error", "Please open an image first.");
            return;
        }

        save_undo_state();

        int width = current_image->width;
        int height = current_image->height;
        Pixel *new_data = (Pixel*)malloc(width * height * sizeof(Pixel));

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int sum_r = 0, sum_g = 0, sum_b = 0;
                int count = 0;

                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        int nx = x + kx;
                        int ny = y + ky;

                        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                            int idx = ny * width + nx;
                            sum_r += current_image->data[idx].r;
                            sum_g += current_image->data[idx].g;
                            sum_b += current_image->data[idx].b;
                            count++;
                        }
                    }
                }

                int out_idx = y * width + x;
                new_data[out_idx].r = (unsigned char)(sum_r / count);
                new_data[out_idx].g = (unsigned char)(sum_g / count);
                new_data[out_idx].b = (unsigned char)(sum_b / count);
            }
        }

        free(current_image->data);
        current_image->data = new_data;
        update_display();
    }

