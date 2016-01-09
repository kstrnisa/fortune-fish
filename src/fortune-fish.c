/**
 * Fortune fish.
 * Copyright (C) 2015 Klemen Strnisa
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <glib.h>

#include "fortune-fish.h"
#include "fortune-fish-config.h"


#define MOUSE_BUTTON_LEFT   1

#define FISH_UPDATE_TIMEOUT 300 /**< Update period for the animation (in milliseconds). */
#define FISH_NUMBER_FRAMES  8   /**< Number of frames in the animation. */
#define FISH_IMAGE_HEIGHT   22  /**< Fish animation image height (in pixels). */
#define FISH_IMAGE_WIDTH    36  /**< Fish animation image width (in pixels). */
#define FISH_IMAGE_BORDER   2   /**< Fish animation image border (in pixels). */
#define FISH_BUTTON_HEIGHT  40  /**< Fortune dialog button height (in pixels). */
#define FISH_BUTTON_WIDTH   100 /**< Fortune dialog button width (in pixels). */
#define FISH_WINDOW_HEIGHT  400 /**< Fortune dialog window height (in pixels). */
#define FISH_WINDOW_WIDTH   640 /**< Fortune dialog window width (in pixels). */
#define FISH_SPACING        5   /**< Fortune dialog inter-element spacing (in pixels). */

#define FISH_ANIMATION_             "fortune-fish.png"
#define FISH_ANIMATION_TRANSPARENT  "fortune-fish-transparent.png"


int fortune_fish_handle_button(fortune_fish *fish,
        GdkEvent *event, GtkWidget *plugin) {

    if (((GdkEventButton *)event)->button == MOUSE_BUTTON_LEFT) {
        fortune_fish_window_show(fish);
        return TRUE;
    }

    return FALSE;
}


int fortune_fish_window_refresh(fortune_fish *fish) {
    int     status, exit_status;
    char    *std_out, *std_err, *output;
    GError  *error;

    g_return_val_if_fail(fish != NULL, FALSE);
    g_return_val_if_fail(GTK_IS_WIDGET(fish->fortune_window), FALSE);

    status = TRUE;

    std_out = NULL;
    std_err = NULL;
    error = NULL;
    exit_status = 0;
    g_spawn_command_line_sync(fish->command,
            &std_out, &std_err, &exit_status, &error);

    /* If error is provided std_out, std_err and exit_status will be NULL. */
    if (error) {
        g_warning("Failed to execute fortune command: %s", error->message);
        status = FALSE;
    }

    if (exit_status) {
        g_spawn_check_exit_status(exit_status, &error);
        g_warning("Fortune command returned error: %s", error->message);
        status = FALSE;
    }

    if (status) {
        output = g_strdup_printf("\n%s", std_out);
        gtk_text_buffer_set_text(fish->fortune_buffer, output, -1);
        g_free(output);
    }

    g_free(std_out);
    g_free(std_err);
    g_free(error);

    return status;
}


int fortune_fish_window_show(fortune_fish *fish) {
    int status;

    g_return_val_if_fail(fish != NULL, FALSE);

    /* The fortune window is destroyed when closed. */
    if (!fish->fortune_window) {
        fish->fortune_window = fortune_fish_window_new(fish);
    }

    status = fortune_fish_window_refresh(fish);
    gtk_widget_show_all(fish->fortune_window);

    return status;
}


GtkWidget *fortune_fish_window_new(fortune_fish *fish) {
    GtkWidget   *window, *view, *scrolled, *grid, *label, *button;
    char        *text;

    g_return_val_if_fail(fish != NULL, FALSE);

    /* top level window */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 2 * FISH_SPACING);
    gtk_window_set_default_size(GTK_WINDOW(window),
            FISH_WINDOW_WIDTH, FISH_WINDOW_HEIGHT);
    gtk_window_set_icon_name(GTK_WINDOW(window), "fortune-fish");
    g_signal_connect(window, "destroy",
            G_CALLBACK(gtk_widget_destroyed), &fish->fortune_window);

    text = g_strdup_printf("%s the Fish", fish->name);
    gtk_window_set_title(GTK_WINDOW(window), text);
    g_free(text);

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 2 * FISH_SPACING);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 2 * FISH_SPACING);
    gtk_container_add(GTK_CONTAINER(window), grid);

    /* Label for the top of the window. */
    label = gtk_label_new("");
    text = g_strdup_printf("<big><big>%s the Fish says:</big></big>", fish->name);
    gtk_label_set_markup(GTK_LABEL(label), text);
    g_free(text);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 3, 1);

    /* Scrolled window for the text view if text is too long. */
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled),
            GTK_SHADOW_IN);
    gtk_widget_set_hexpand(scrolled, TRUE);
    gtk_widget_set_vexpand(scrolled, TRUE);
    gtk_grid_attach(GTK_GRID(grid), scrolled, 0, 1, 3, 1);

    /* Text view fot the fortune. */
    view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), FALSE);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(view), 2 * FISH_SPACING);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(view), 2 * FISH_SPACING);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD);
    fish->fortune_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(fish->fortune_buffer),
            "monospace_tag", "family", "Monospace", NULL);
    gtk_container_add(GTK_CONTAINER(scrolled), view);

    /* Empty label that moves buttons to the right. */
    label = gtk_label_new("");
    gtk_widget_set_hexpand(label, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);

    /* Next and Close buttons. */
    button = gtk_button_new_with_label("Speak Again");
    gtk_widget_set_size_request(button, FISH_BUTTON_WIDTH, FISH_BUTTON_HEIGHT);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 2, 1, 1);
    g_signal_connect_swapped(button, "clicked",
            G_CALLBACK(fortune_fish_window_refresh), fish);

    button = gtk_button_new_with_label("Close");
    gtk_widget_set_size_request(button, FISH_BUTTON_WIDTH, FISH_BUTTON_HEIGHT);;
    gtk_grid_attach(GTK_GRID(grid), button, 2, 2, 1, 1);
    g_signal_connect_swapped(button, "clicked",
            G_CALLBACK(gtk_widget_destroy), window);

    return window;
}


int fortune_fish_animation_refresh(fortune_fish *fish) {
    GdkPixbuf   *frame;
    float       width, height;

    g_return_val_if_fail(fish != NULL, FALSE);
    g_return_val_if_fail(fish->image != NULL, FALSE);

    /* Update frame counter. */
    fish->current_frame = (fish->current_frame + 1) % FISH_NUMBER_FRAMES;

    /* Scale image to size, depending on orientation. */
    if(fish->orientation == GTK_ORIENTATION_HORIZONTAL) {
        height = fish->size - FISH_IMAGE_BORDER;
        width = FISH_IMAGE_WIDTH * height/FISH_IMAGE_HEIGHT;
    } else {
        width = fish->size - FISH_IMAGE_BORDER;
        height = FISH_IMAGE_HEIGHT * width/FISH_IMAGE_WIDTH;
    }

    /* This gives a new instance of a pixbuf so we have to free it at the end. */
    frame = gdk_pixbuf_scale_simple(fish->frames[fish->current_frame],
            (int)width, (int)height, GDK_INTERP_BILINEAR);
    if (!frame) {
        g_warning("Failed to create animation frame.");
        return FALSE;
    }

    /* Set new image from the scaled pixbuf and free it. */
    gtk_image_set_from_pixbuf(GTK_IMAGE(fish->image), frame);
    g_object_unref(G_OBJECT(frame));

    return TRUE;
}


int fortune_fish_animation_init(fortune_fish *fish) {
    char        *fish_pixbuf_path;
    int         iter;
    GdkPixbuf   *fish_pixbuf;
    GError      *error;

    g_return_val_if_fail(fish != NULL, FALSE);

    /* Retrieve fish panel animation pixmap from file. */
    fish_pixbuf_path = g_build_filename(PIXMAPSDIR,
            FISH_ANIMATION_TRANSPARENT, NULL);
    if (!g_file_test(fish_pixbuf_path, G_FILE_TEST_EXISTS)) {
        g_warning("Failed to open animation pixmap %s", fish_pixbuf_path);
        return FALSE;
    }

    error = NULL;
    fish_pixbuf = gdk_pixbuf_new_from_file(fish_pixbuf_path, &error);
    g_free(fish_pixbuf_path);
    if (!fish_pixbuf) {
        g_warning("Failed to load animation: %s", error->message);
        g_free(error);
        return FALSE;
    }

    /* Split full pixbuf into separate frames. */
    fish->current_frame = 0;
    fish->frames = (GdkPixbuf **)g_malloc(FISH_NUMBER_FRAMES * sizeof(GdkPixbuf *));
    for(iter = 0; iter < FISH_NUMBER_FRAMES; iter++) {
        fish->frames[iter] = gdk_pixbuf_new_subpixbuf(fish_pixbuf,
                iter*FISH_IMAGE_WIDTH, 0, FISH_IMAGE_WIDTH, FISH_IMAGE_HEIGHT);
    }
    g_object_unref(G_OBJECT(fish_pixbuf));

    /* Create fish panel animation image and event box to catch mouse clicks. */
    fish->image = gtk_image_new();
    fish->ebox = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(fish->ebox), fish->image);
    gtk_widget_add_events(fish->ebox, GDK_BUTTON_PRESS_MASK);
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(fish->ebox), FALSE);
    gtk_widget_show_all(fish->ebox);

    /* Show fortune window on left mouse button click on animation. */
    g_signal_connect_swapped(fish->ebox, "button-press-event",
            G_CALLBACK(fortune_fish_handle_button), fish);

    /* Start animation. */
    fish->timer = g_timeout_add(FISH_UPDATE_TIMEOUT,
            (GSourceFunc)fortune_fish_animation_refresh, fish);

    return TRUE;
}


void fortune_fish_animation_destroy(fortune_fish *fish) {
    int iter;

    g_return_if_fail(fish != NULL);

    g_source_remove(fish->timer);
    gtk_widget_destroy(fish->ebox);

    for (iter = 0; iter < FISH_NUMBER_FRAMES; iter++) {
        g_object_unref(G_OBJECT(fish->frames[iter]));
    }
    g_free(fish->frames);
}


fortune_fish *fortune_fish_new() {
    fortune_fish *fish;

    fish = (fortune_fish *)g_new0(fortune_fish, 1);

    fortune_fish_config_init(fish);

    return fish;
}


void fortune_fish_destroy(fortune_fish *fish) {

    g_return_if_fail(fish != NULL);

    if (GTK_IS_WIDGET(fish->fortune_window)) {
        gtk_widget_destroy(fish->fortune_window);
    }

    fortune_fish_config_destroy(fish);
}
