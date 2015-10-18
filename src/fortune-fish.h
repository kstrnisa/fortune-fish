
#ifndef __FORTUNE_FISH_H__
#define __FORTUNE_FISH_H__


#include <gtk/gtk.h>


/**
 * @brief Private context for the plugin.
 *
 * The context consists of the information necessary for the XFCE panel, gui
 * elements, user settings and internal information.
 */
typedef struct {
    GtkWidget       *fortune_window;    /**< Fortune dialog window. */
    GtkTextBuffer   *fortune_buffer;    /**< Fortune command response buffer. */
    GtkWidget       *fortune_label;     /**< Label in forune window. */
    GtkWidget       *ebox;              /**< Event box that listens for mouse clicks on the fish. */
    GtkWidget       *image;             /**< Current fish image (changes with timer ticks). */
    GdkPixbuf       **frames;           /**< Array of fish animation pixbufs (eight pixbufs). */
    int             current_frame;      /**< Number of current frame (from 0 to 7). */
    int             size;               /**< Size in the relevant dimension (width for horizontal and height for vertical panel orientaion). */
    GtkOrientation  orientation;        /**< Orientation of the animation. */
    unsigned int    timer;              /**< Timer for periodic update of the image with the next element from frames. */

    /* Fortune fish configuration. */
    char            *config_path;
    GKeyFile        *config;
    char            *command;           /**< Command issued each time the fish is clicked. */
    GtkWidget       *command_setting;   /**< GtkEntry widget holding the fish command. */
    char            *name;              /**< Name of the fish. */
    GtkWidget       *name_setting;      /**< GtkEnty widget holding the fish name. */
} fortune_fish;


fortune_fish *fortune_fish_new();
void fortune_fish_destroy(fortune_fish *fish);

int fortune_fish_animation_init(fortune_fish *fish);
int fortune_fish_animation_refresh(fortune_fish *fish);
void fortune_fish_animation_destroy(fortune_fish *fish);

GtkWidget *fortune_fish_window_new(fortune_fish *fish);
int fortune_fish_window_show(fortune_fish *fish);
int fortune_fish_window_refresh(fortune_fish *fish);


#endif /* __FORTUNE_FISH_H__ */
