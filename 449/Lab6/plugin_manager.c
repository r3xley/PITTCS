#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

static int (*InitializeFunction)();
static int (*RunFunction)();
static int (*CleanupFunction)();

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("enter correct args");
        return 1;
    }

    char pluginName[255];
    snprintf(pluginName, sizeof(pluginName), "./%s.so", argv[1]);

    void *plugin = dlopen(pluginName, RTLD_LAZY);
    if (plugin == NULL)
    {
        fprintf(stderr, "error plugin %s\n", dlerror());
        return 1;
    }

    InitializeFunction = dlsym(plugin, "initialize");
    RunFunction = dlsym(plugin, "run");
    CleanupFunction = dlsym(plugin, "cleanup");

    // print errors see if they appeardont assume that just call

    InitializeFunction();
    RunFunction();
    CleanupFunction();

    // dlclose(plugin);

    return 0;
}
