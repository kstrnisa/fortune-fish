/**
 * Fortune fish plugin for the XFCE panel.
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

#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4util/libxfce4util.h>
#include <libxfce4ui/libxfce4ui.h>

#include "fortune-fish.h"
#include "fortune-fish-settings.h"


typedef struct {
    XfcePanelPlugin *plugin;
    fortune_fish    *fish;
} fortune_fish_plugin;


static void fortune_fish_plugin_construct(XfcePanelPlugin *plugin);
XFCE_PANEL_PLUGIN_REGISTER(fortune_fish_plugin_construct);


static void fortune_fish_plugin_adjust(fortune_fish_plugin *fish_plugin) {

    fish_plugin->fish->orientation = xfce_panel_plugin_get_orientation(fish_plugin->plugin);
    fish_plugin->fish->size = xfce_panel_plugin_get_size(fish_plugin->plugin);

    if (fish_plugin->fish->orientation == GTK_ORIENTATION_HORIZONTAL) {
        gtk_widget_set_size_request(GTK_WIDGET(fish_plugin->plugin),
                -1, fish_plugin->fish->size);
    } else {
        gtk_widget_set_size_request(GTK_WIDGET(fish_plugin->plugin),
                fish_plugin->fish->size, -1);
    }
}


static void fortune_fish_plugin_settings_close(fortune_fish_plugin *fish_plugin,
            gint response, GtkWidget *dialog) {

    fortune_fish_settings_close(fish_plugin->fish);
    gtk_widget_destroy(dialog);

    xfce_panel_plugin_unblock_menu(fish_plugin->plugin);
}


static void fortune_fish_plugin_settings(fortune_fish_plugin *fish_plugin) {
    GtkWidget   *dialog, *content_area, *settings;

    xfce_panel_plugin_block_menu(fish_plugin->plugin);

    dialog = xfce_titled_dialog_new_with_buttons("Fortune Fish",
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(fish_plugin->plugin))),
            GTK_DIALOG_DESTROY_WITH_PARENT, "_Close", GTK_RESPONSE_OK, NULL);

    xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog), "Settings");
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
    gtk_window_set_icon_name(GTK_WINDOW(dialog), "fortune-fish");

    settings = fortune_fish_settings_new(fish_plugin->fish);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(content_area), settings);

    g_signal_connect_swapped(dialog, "response",
            G_CALLBACK(fortune_fish_plugin_settings_close), fish_plugin);

    gtk_widget_show_all(dialog);
}


static void fortune_fish_plugin_destroy(fortune_fish_plugin *fish_plugin) {

    fortune_fish_animation_destroy(fish_plugin->fish);
    fortune_fish_destroy(fish_plugin->fish);
    g_free(fish_plugin);
}


static fortune_fish_plugin *fortune_fish_plugin_new(XfcePanelPlugin *plugin) {
    fortune_fish_plugin *fish_plugin;

    /* Allocate memory for the plugin structure. */
    fish_plugin = (fortune_fish_plugin *)g_new0(fortune_fish_plugin, 1);

    /* Create new fish structure and initialize the panel animation. */
    fish_plugin->plugin = plugin;
    fish_plugin->fish = fortune_fish_new();
    fortune_fish_animation_init(fish_plugin->fish);

    /* Show settings in plugin menu. */
    xfce_panel_plugin_menu_show_configure(fish_plugin->plugin);
    g_signal_connect_swapped(fish_plugin->plugin, "configure-plugin",
            G_CALLBACK(fortune_fish_plugin_settings), fish_plugin);

    /* Connect notifications of orientation and size changes from the
     * XFCE panel. Needed for proper rendering of the animation image. */
    g_signal_connect_swapped(fish_plugin->plugin, "size-changed",
            G_CALLBACK(fortune_fish_plugin_adjust), fish_plugin);
    g_signal_connect_swapped(fish_plugin->plugin, "orientation-changed",
            G_CALLBACK(fortune_fish_plugin_adjust), fish_plugin);

    /* Free all plugin resources. */
    g_signal_connect_swapped(fish_plugin->plugin, "free-data",
            G_CALLBACK(fortune_fish_plugin_destroy), fish_plugin);

    return fish_plugin;
}


static void fortune_fish_plugin_construct(XfcePanelPlugin *plugin) {
    fortune_fish_plugin *fish_plugin;

    xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

    fish_plugin = fortune_fish_plugin_new(plugin);

    /* Add the animation to the panel plugin. */
    gtk_container_add(GTK_CONTAINER(plugin), fish_plugin->fish->ebox);
    xfce_panel_plugin_add_action_widget(plugin, fish_plugin->fish->ebox);
}
