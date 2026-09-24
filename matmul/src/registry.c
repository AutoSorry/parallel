#include "matmul.h"

#include <stdio.h>
#include <string.h>

#define MAX_MODULES 32

static const matmul_module *modules[MAX_MODULES];
static int module_count = 0;

int matmul_register(const matmul_module *module) {
    if (module == NULL || module->name == NULL || module->fn == NULL) {
        return -1;
    }

    for (int i = 0; i < module_count; ++i) {
        if (strcmp(modules[i]->name, module->name) == 0) {
            return -1;
        }
    }

    if (module_count >= MAX_MODULES) {
        return -1;
    }

    modules[module_count++] = module;
    return 0;
}

const matmul_module *matmul_get(const char *name) {
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

int matmul_count(void) {
    return module_count;
}

const matmul_module *matmul_at(int index) {
    if (index < 0 || index >= module_count) {
        return NULL;
    }
    return modules[index];
}

void matmul_list_all(void) {
    printf("Available modules:\n");
    for (int i = 0; i < module_count; ++i) {
        const char *desc = modules[i]->description ? modules[i]->description : "";
        printf("  %-16s %s\n", modules[i]->name, desc);
    }
}
