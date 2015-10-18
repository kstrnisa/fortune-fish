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
#include "fortune-fish-settings.h"


#define FISH_SPACING 5


static void fortune_fish_settings_read(fortune_fish *fish) {

    gtk_entry_set_text(GTK_ENTRY(fish->command_setting), fish->command);

    gtk_entry_set_text(GTK_ENTRY(fish->name_setting), fish->name);
}


static void fortune_fish_settings_save(fortune_fish *fish) {

    g_free(fish->command);
    fish->command = g_strdup(gtk_entry_get_text(GTK_ENTRY(fish->command_setting)));

    g_free(fish->name);
    fish->name = g_strdup(gtk_entry_get_text(GTK_ENTRY(fish->name_setting)));
}


int fortune_fish_settings_close(fortune_fish *fish) {
    int status;

    g_return_val_if_fail(fish != NULL, FALSE);

    fortune_fish_settings_save(fish);
    status = fortune_fish_config_write(fish);

    return status;
}


GtkWidget *fortune_fish_settings_new(fortune_fish *fish) {
    GtkWidget   *label, *grid;

    g_return_val_if_fail(fish != NULL, NULL);

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 2 * FISH_SPACING);
    gtk_grid_set_row_spacing(GTK_GRID(grid), FISH_SPACING);
    gtk_container_set_border_width(GTK_CONTAINER(grid), FISH_SPACING);

    label = gtk_label_new("");
    gtk_label_set_markup(GTK_LABEL(label), "<b>Fortune Fish</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    label = gtk_label_new("Fortune command:");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_margin_start(label, 2 * FISH_SPACING);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    fish->command_setting = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), fish->command_setting, 1, 1, 1, 1);

    label = gtk_label_new("Fish name:");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_margin_start(label, 2 * FISH_SPACING);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
    fish->name_setting = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), fish->name_setting, 1, 2, 1, 1);

    /* Set settings from struct members to widgets. */
    fortune_fish_settings_read(fish);

    return grid;
}
