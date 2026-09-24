#include "registry.h"

#include <stdio.h>
#include <string.h>

#define MAX_KERNEL_MODULES 32

static const kernel_module *modules[MAX_KERNEL_MODULES];
static int module_count = 0;

int kernel_register(const kernel_module *module) {
    if (module == NULL || module->name == NULL || module->run == NULL) {
        return -1;
    }

    for (int i = 0; i < module_count; ++i) {
        if (strcmp(modules[i]->name, module->name) == 0) {
            return -1;
        }
    }

    if (module_count >= MAX_KERNEL_MODULES) {
        return -1;
    }

    modules[module_count++] = module;
    return 0;
}

const kernel_module *kernel_find(const char *name) {
    if (name == NULL) {
        return NULL;
    }

    for (int i = 0; i < module_count; ++i) {
        if (strcmp(modules[i]->name, name) == 0) {
            return modules[i];
        }
    }

    return NULL;
}

int kernel_count(void) {
    return module_count;
}

const kernel_module *kernel_at(int index) {
    if (index < 0 || index >= module_count) {
        return NULL;
    }
    return modules[index];
}

void kernel_list_all(void) {
    printf("Available modules:\n");
    for (int i = 0; i < module_count; ++i) {
        const char *description = modules[i]->description != NULL
                                      ? modules[i]->description
                                      : "";
        printf("  %-16s %s\n", modules[i]->name, description);
    }
}
