#include <stdlib.h>
#include <stdio.h>
#include <iup.h>
#include "allheaderfile.h"

    int btn_open_cb(Ihandle *self) {
        (void)self;
        bmp_open();
        return IUP_DEFAULT;
    }

    int btn_save_cb(Ihandle *self) {
        (void)self;
        apply_save();
        return IUP_DEFAULT;
    }

    int btn_undo_cb(Ihandle *self) {
        (void)self;
        apply_undo();
        return IUP_DEFAULT;
    }


    int btn_grayscale_cb(Ihandle *self) {
        (void)self;
        apply_grayscale(); 
        return IUP_DEFAULT;
    }

    int btn_brightness_cb(Ihandle *self) {
        (void)self;
        apply_brightness();
        return IUP_DEFAULT;
    }

    int btn_invert_cb(Ihandle *self) {
        (void)self;
        apply_inversion(); 
        return IUP_DEFAULT;
    }

    int btn_hflip_cb(Ihandle *self) {
        (void)self;
        apply_hflip();
        return IUP_DEFAULT;
    }

    int btn_vflip_cb(Ihandle *self) {
        (void)self;
        apply_vflip();
        return IUP_DEFAULT;
    }

    int btn_rotate_cb(Ihandle *self) {
        (void)self;
        apply_rotate90();
        return IUP_DEFAULT;
    }

    int btn_crop_cb(Ihandle *self) {
        (void)self;
        apply_crop();
        return IUP_DEFAULT;
    }

    int btn_blur_cb(Ihandle *self) {
        (void)self;
        apply_blur();
        return IUP_DEFAULT;
    }

    int main(int argc, char **argv) {
        Ihandle *dlg, *vbox_menu, *hbox_main, *image_display;
        Ihandle *btn_open, *btn_save, *btn_undo;
        Ihandle *btn_gray, *btn_bright, *btn_invert;
        Ihandle *btn_hflip, *btn_vflip, *btn_rotate, *btn_crop, *btn_blur;

        IupOpen(&argc, &argv);

        btn_open = IupButton("Open", NULL);
        btn_save = IupButton("Save", NULL);
        btn_undo = IupButton("Undo", NULL);

        btn_gray   = IupButton("Grayscale", NULL);
        btn_bright = IupButton("Brightness", NULL);
        btn_invert = IupButton("Invert Image", NULL);
        btn_hflip  = IupButton("Horizontal Flip", NULL);
        btn_vflip  = IupButton("Vertical Flip", NULL);
        btn_rotate = IupButton("Rotate 90", NULL);
        btn_crop   = IupButton("Crop", NULL);
        btn_blur   = IupButton("Blur", NULL);

        IupSetCallback(btn_open, "ACTION", (Icallback)btn_open_cb);
        IupSetCallback(btn_save, "ACTION", (Icallback)btn_save_cb);
        IupSetCallback(btn_undo, "ACTION", (Icallback)btn_undo_cb);
        IupSetCallback(btn_gray, "ACTION", (Icallback)btn_grayscale_cb);
        IupSetCallback(btn_bright, "ACTION", (Icallback)btn_brightness_cb);
        IupSetCallback(btn_invert, "ACTION", (Icallback)btn_invert_cb);
        IupSetCallback(btn_hflip, "ACTION", (Icallback)btn_hflip_cb);
        IupSetCallback(btn_vflip, "ACTION", (Icallback)btn_vflip_cb);
        IupSetCallback(btn_rotate, "ACTION", (Icallback)btn_rotate_cb);
        IupSetCallback(btn_crop, "ACTION", (Icallback)btn_crop_cb);
        IupSetCallback(btn_blur, "ACTION", (Icallback)btn_blur_cb);

        vbox_menu = IupVbox(
            btn_open, btn_save, btn_undo, 
            IupLabel("--- Filters ---"),
            btn_gray, btn_bright, btn_invert, btn_blur,
            IupLabel("--- Action ---"),
            btn_hflip, btn_vflip, btn_rotate, btn_crop,
            NULL
        );
        IupSetAttribute(vbox_menu, "GAP", "5");
        IupSetAttribute(vbox_menu, "MARGIN", "10x10");

        static unsigned char placeholder_pixels[3] = {255, 255, 255};
        displayed_iup_image = IupImageRGB(1, 1, placeholder_pixels);

        image_display = IupLabel(NULL);
        IupSetAttributeHandle(image_display, "IMAGE", displayed_iup_image);
        IupSetAttribute(image_display, "SIZE", "400x400");
        IupSetAttribute(image_display, "ALIGNMENT", "ACENTER:ACENTER");
        IupSetHandle("MY_DISPLAY_LABEL", image_display);

        hbox_main = IupHbox(vbox_menu, image_display, NULL);

        dlg = IupDialog(hbox_main);
        IupSetAttribute(dlg, "TITLE", "Image Manipulation Software");
        IupSetAttribute(dlg, "SIZE", "HALFXHALF");

        IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
        IupMainLoop();

        IupClose();
        return EXIT_SUCCESS;
    }
