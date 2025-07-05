#include "modEnv.h"
#include "envs.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static size_t countEnvs(char* const envp[]) {
    size_t count = 0;
    while (envp[count]) {
        count++;
    }
    return count;
}

static void setEnv(char** env_array, size_t* count, const char* key, size_t key_len) {
    for (size_t i = 0; i < *count; i++) {
        if (strncmp(env_array[i], key, key_len) == 0 && env_array[i][key_len] == '=') {
            free(env_array[i]);
            env_array[i] = env_array[*count - 1];
            (*count)--;
            break;
        }
    }
}

char** create_modified_envp(char* const envp[]) {
    size_t original_count = countEnvs(envp);
    char** new_env = malloc(sizeof(char*) * (original_count + 64));
    if (!new_env) {
        return NULL;
    }

    size_t current_count = 0;
    for (size_t i = 0; envp[i]; i++) {
        new_env[current_count++] = strdup(envp[i]);
    }

    for (ConditionalEnv* env = conditional_envs; env->env; env++) {
        if (!env->condition()) {
            continue;
        }

        size_t key_len = strlen(env->env);
        setEnv(new_env, &current_count, env->env, key_len);

        if (env->value) {
            size_t total_len = key_len + strlen(env->value) + 2;
            new_env[current_count] = malloc(total_len);
            snprintf(new_env[current_count], total_len, "%s=%s", env->env, env->value);
            current_count++;
        }
    }

    new_env[current_count] = NULL;
    return new_env;
}

void free_envp(char** envp) {
    for (size_t i = 0; envp[i]; i++) {
        free(envp[i]);
    }
    free(envp);
}

static bool is_appdir_set(void) {
    return getenv("APPDIR") != NULL;
}

static bool always(void) {
    return true;
}

ConditionalEnv conditional_envs[] = {
    {"BABL_PATH", NULL, is_appdir_set},
    {"GBM_BACKENDS_PATH", NULL, is_appdir_set},
    {"GCONV_PATH", NULL, is_appdir_set},
    {"GDK_PIXBUF_MODULEDIR", NULL, is_appdir_set},
    {"GDK_PIXBUF_MODULE_FILE", NULL, is_appdir_set},
    {"GEGL_PATH", NULL, is_appdir_set},
    {"GIO_MODULE_DIR", NULL, is_appdir_set},
    {"GI_TYPELIB_PATH", NULL, is_appdir_set},
    {"GSETTINGS_SCHEMA_DIR", NULL, is_appdir_set},
    {"GST_PLUGIN_PATH", NULL, is_appdir_set},
    {"GST_PLUGIN_SCANNER", NULL, is_appdir_set},
    {"GST_PLUGIN_SYSTEM_PATH", NULL, is_appdir_set},
    {"GST_PLUGIN_SYSTEM_PATH_1_0", NULL, is_appdir_set},
    {"GTK_DATA_PREFIX", NULL, is_appdir_set},
    {"GTK_EXE_PREFIX", NULL, is_appdir_set},
    {"GTK_IM_MODULE_FILE", NULL, is_appdir_set},
    {"GTK_PATH", NULL, is_appdir_set},
    {"LD_LIBRARY_PATH", NULL, is_appdir_set},
    {"LD_PRELOAD", NULL, is_appdir_set},
    {"LIBDECOR_PLUGIN_DIR", NULL, is_appdir_set},
    {"LIBGL_DRIVERS_PATH", NULL, is_appdir_set},
    {"LIBVA_DRIVERS_PATH", NULL, is_appdir_set},
    {"PERLLIB", NULL, is_appdir_set},
    {"PIPEWIRE_MODULE_DIR", NULL, is_appdir_set},
    {"PYTHONHOME", NULL, is_appdir_set},
    {"QT_PLUGIN_PATH", NULL, is_appdir_set},
    {"SPA_PLUGIN_DIR", NULL, is_appdir_set},
    {"TCL_LIBRARY", NULL, is_appdir_set},
    {"TK_LIBRARY", NULL, is_appdir_set},
    {"XTABLES_LIBDIR", NULL, is_appdir_set},
    {"APPDIR", NULL, always},
    {"_TEST", "HAPPY HAPPY HAPPY", always},
    {NULL, NULL, NULL}
};
