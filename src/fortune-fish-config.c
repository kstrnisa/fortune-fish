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
#include <glib/gstdio.h>
#include <fcntl.h>

#include "fortune-fish.h"
#include "fortune-fish-config.h"


#define FISH_CONFIG_FOLDER      "fortune-fish"
#define FISH_CONFIG_FILE        "fortune-fish.conf"

#define FISH_CONFIG_GROUP       "main"
#define FISH_CONFIG_NAME        "name"
#define FISH_CONFIG_CMD         "command"

#define FISH_DEFAULT_NAME       "Wanda"
#define FISH_DEFAULT_CMD        "fortune"


int fortune_fish_config_read(fortune_fish *fish) {
    GError *error;

    g_return_val_if_fail(fish != NULL, FALSE);
    g_return_val_if_fail(fish->config != NULL, FALSE);

    /* Load config from config file and if a certain setting is missing
     * provide a default value. */

    error = NULL;
    g_key_file_load_from_file(fish->config, fish->config_path,
            G_KEY_FILE_NONE, &error);
    if (error){
        g_warning("Failed to load config file (%s): %s",
                fish->config_path, error->message);
        g_free(error);
        return FALSE;
    }

    error = NULL;
    g_free(fish->name);
    fish->name = g_key_file_get_string(fish->config,
            FISH_CONFIG_GROUP, FISH_CONFIG_NAME, &error);
    if (error) {
        g_debug("\"%s\" setting not found in config: %s",
                FISH_CONFIG_NAME, error->message);
        fish->name = g_strdup(FISH_DEFAULT_NAME);
        g_free(error);
    }

    error = NULL;
    g_free(fish->command);
    fish->command = g_key_file_get_string(fish->config,
            FISH_CONFIG_GROUP, FISH_CONFIG_CMD, &error);
    if (error) {
        g_debug("\"%s\" setting not found in config: %s",
                FISH_CONFIG_CMD, error->message);
        fish->command = g_strdup(FISH_DEFAULT_CMD);
        g_free(error);
    }

    return TRUE;
}


int fortune_fish_config_write(fortune_fish *fish) {
    GError *error;

    g_return_val_if_fail(fish != NULL, FALSE);
    g_return_val_if_fail(fish->config != NULL, FALSE);

    /* Write all current settings to config file. */

    g_key_file_set_string(fish->config,
            FISH_CONFIG_GROUP, FISH_CONFIG_NAME, fish->name);

    g_key_file_set_string(fish->config,
            FISH_CONFIG_GROUP, FISH_CONFIG_CMD, fish->command);;

    error = NULL;
    g_key_file_save_to_file(fish->config, fish->config_path, &error);
    if (error){
        g_warning("Failed to save config to file (%s): %s",
                fish->config_path, error->message);
        g_free(error);
        return FALSE;
    }

    return TRUE;
}


int fortune_fish_config_init(fortune_fish *fish) {
    const char  *user_config_dir;
    char        *fish_config_dir;
    char        *fish_config_path;

    g_return_val_if_fail(fish != NULL, FALSE);

    /* This string belongs to glib and should't be freed. */
    user_config_dir = g_get_user_config_dir();

    /* If the folder containg the config file doesn't exist create it. */
    fish_config_dir = g_build_filename(user_config_dir,
            FISH_CONFIG_FOLDER, NULL);
    if (!g_file_test(fish_config_dir, G_FILE_TEST_EXISTS)) {
        g_debug("Config folder doesn't exist yet: %s\n", fish_config_dir);
        g_mkdir(fish_config_dir, S_IRWXU);
    }

    /* If the config file doesn't exist create an empty one. */
    fish_config_path = g_build_filename(fish_config_dir,
            FISH_CONFIG_FILE, NULL);
    if (!g_file_test(fish_config_path, G_FILE_TEST_EXISTS)) {
        g_debug("Config file doesn't exist yet: %s\n", fish_config_path);
        g_creat(fish_config_path, S_IRUSR | S_IWUSR);
    }
    g_free(fish->config_path);
    fish->config_path = g_strdup(fish_config_path);

    g_free(fish_config_dir);
    g_free(fish_config_path);

    if (!fish->config) {
        fish->config = g_key_file_new();
    }

    return fortune_fish_config_read(fish);
}


void fortune_fish_config_destroy(fortune_fish *fish) {

    g_return_if_fail(fish != NULL);

    g_free(fish->name);
    g_free(fish->command);
    g_free(fish->config_path);
    g_key_file_free(fish->config);

    fish->name = NULL;
    fish->command = NULL;
    fish->config_path = NULL;
    fish->config = NULL;
}
