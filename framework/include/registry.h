#ifndef KERNEL_REGISTRY_H
#define KERNEL_REGISTRY_H

#include "kernel.h"

int kernel_register(const kernel_module *module);
const kernel_module *kernel_find(const char *name);
int kernel_count(void);
const kernel_module *kernel_at(int index);
void kernel_list_all(void);

#endif
