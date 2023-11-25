#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>

// Define a struct to store plugin
typedef struct PluginNode
{
    char name[20];
    void *handle;
    int (*initialize)();
    int (*run)(char **);
    struct PluginNode *next;
} PluginNode;

PluginNode *loaded_plugins = NULL;

void load_plugin(const char *plugin_name)
{
    void *handle = dlopen(plugin_name, RTLD_NOW);
    if (handle != NULL)
    {
        int (*initialize)() = dlsym(handle, "initialize");
        int (*run)(char **) = dlsym(handle, "run");

        char *error = dlerror();
        if (error != NULL)
        {
            fprintf(stderr, "Error: dlsym failed for plugin %s: %s\n", plugin_name, error);
            dlclose(handle);
            return;
        }

        if (initialize != NULL && run != NULL)
        {
            // Create a new node for the loaded plugin
            PluginNode *new_plugin = malloc(sizeof(PluginNode));
            if (new_plugin != NULL)
            {
                strcpy(new_plugin->name, plugin_name);
                new_plugin->handle = handle;
                new_plugin->initialize = initialize;
                new_plugin->run = run;
                new_plugin->next = loaded_plugins;

                // Update the head of the linked list
                loaded_plugins = new_plugin;

                // Successful initialization
                printf("Plugin %s initialized\n", plugin_name);
                return;
            }
            else
            {
                fprintf(stderr, "Error: Memory allocation failed for plugin %s\n", plugin_name);
            }
        }
        else
        {
            fprintf(stderr, "Error: Missing required symbols (initialize and run) in plugin %s\n", plugin_name);
        }

        // If we reach here, initialization failed
        dlclose(handle);
    }
    else
    {
        fprintf(stderr, "Error: Unable to load plugin %s: %s\n", plugin_name, dlerror());
    }
}

void unload_plugins()
{
    // Free the memory associated with each loaded plugin
    PluginNode *current = loaded_plugins;
    while (current != NULL)
    {
        PluginNode *next = current->next;
        dlclose(current->handle);
        free(current);
        current = next;
    }
}

void execute_command(char *args[])
{
    // Implementation remains the same
    // ...
    if (args[0] == NULL)
    {
        // Empty command, do nothing
        return;
    }

    if (strcmp(args[0], "exit") == 0)
    {
        // Exit the shell
        exit(0);
    }
    else if (strcmp(args[0], "load") == 0)
    {
        // Load a plugin
        if (args[1] != NULL)
        {
            load_plugin(args[1]);
        }
    }
    else
    {
        // Check if the command matches a loaded plugin
        PluginNode *current = loaded_plugins;
        while (current != NULL)
        {
            if (strcmp(args[0], current->name) == 0)
            {
                // Invoke the run function of the loaded plugin
                current->run(args);
                return;
            }
            current = current->next;
        }

        // If the command is not a built-in or a plugin, execute it
        pid_t pid = fork();
        if (pid == 0)
        {
            // Child process
            execvp(args[0], args);
            // execvp only returns on error
            perror("Error");
            exit(1);
        }
        else
        {
            // Parent process
            wait(NULL);
        }
    }
}

int main()
{
    char input[200];
    while (1)
    {
        printf("> ");
        fgets(input, sizeof(input), stdin);

        // Remove trailing newline character
        input[strcspn(input, "\n")] = '\0';

        // Tokenize input into arguments
        char *args[20];
        char *token = strtok(input, " ");
        int i = 0;
        while (token != NULL && i < 19)
        {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // Execute the command
        execute_command(args);
    }

    // Unload plugins before exiting
    unload_plugins();

    return 0;
}

/////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>

#define MAX_COMMAND_LENGTH 200
#define MAX_PROGRAM_NAME_LENGTH 20
#define MAX_ARGUMENTS 20
#define MAX_PLUGINS 10

typedef struct plugin
{
    char *name;
    void *handle;
    int (*initialize)();
    int (*run)(char **argv);
} plugin_t;

plugin_t plugins[MAX_PLUGINS];
int num_plugins = 0;

void load_plugin(const char *plugin_name)
{
    void *handle = dlopen(plugin_name, RTLD_NOW);
    if (handle != NULL)
    {
        int (*initialize)() = dlsym(handle, "initialize");
        int (*run)(char **) = dlsym(handle, "run");

        char *error = dlerror();
        if (error != NULL)
        {
            fprintf(stderr, "Error: dlsym failed for plugin %s: %s\n", plugin_name, error);
            dlclose(handle);
            return;
        }

        if (initialize != NULL && run != NULL)
        {
            // Create a new node for the loaded plugin
            PluginNode *new_plugin = malloc(sizeof(PluginNode));
            if (new_plugin != NULL)
            {
                strcpy(new_plugin->name, plugin_name);
                new_plugin->handle = handle;
                new_plugin->initialize = initialize;
                new_plugin->run = run;
                new_plugin->next = loaded_plugins;

                // Update the head of the linked list
                loaded_plugins = new_plugin;

                // Successful initialization
                printf("Plugin %s initialized\n", plugin_name);
                return;
            }
            else
            {
                fprintf(stderr, "Error: Memory allocation failed for plugin %s\n", plugin_name);
            }
        }
        else
        {
            fprintf(stderr, "Error: Missing required symbols (initialize and run) in plugin %s\n", plugin_name);
        }

        // If we reach here, initialization failed
        dlclose(handle);
    }
    else
    {
        fprintf(stderr, "Error: Unable to load plugin %s: %s\n", plugin_name, dlerror());
    }
}

int unload_plugin(char *plugin_name)
{
    int i;

    for (i = 0; i < num_plugins; i++)
    {
        if (strcmp(plugins[i].name, plugin_name) == 0)
        {
            dlclose(plugins[i].handle);
            free(plugins[i].name);
            plugins[i] = plugins[num_plugins - 1];
            num_plugins--;
            return 0;
        }
    }

    return -1;
}

int execute_command(char *command)
{
    char *argv[MAX_ARGUMENTS + 1];
    int num_args = 0;

    char *token = strtok(command, " ");
    while (token)
    {
        argv[num_args++] = token;
        token = strtok(NULL, " ");
    }

    argv[num_args] = NULL;

    if (strcmp(argv[0], "exit") == 0)
    {
        return 1;
    }
    else if (strcmp(argv[0], "load") == 0)
    {
        if (load_plugin(argv[1]) != 0)
        {
            return -1;
        }
    }
    else if (strcmp(argv[0], "unload") == 0)
    {
        if (unload_plugin(argv[1]) != 0)
        {
            return -1;
        }
    }
    else
    {
        int i;

        for (i = 0; i < num_plugins; i++)
        {
            if (strcmp(plugins[i].name, argv[0]) == 0)
            {
                return plugins[i].run(argv);
            }
        }

        if (fork() == 0)
        {
            execvp(argv[0], argv);
            _exit(-1);
        }
        else
        {
            wait(NULL);
        }
    }

    return 0;
}

int main()
{
    while (1)
    {
        printf("> ");
        char command[MAX_COMMAND_LENGTH];
        fgets(command, MAX_COMMAND_LENGTH, stdin);

        if (execute_command(command) != 0)
        {
            break;
        }
    }

    return 0;
}