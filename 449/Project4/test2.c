#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dlfcn.h>

#define MAX_ARGS 20

// Define the struct for a loaded plugin

typedef struct PluginNode
{
    char name[20];
    void *plugin;
    int (*initialize)();
    int (*run)(char **);
    struct PluginNode *next;
} PluginNode;

// head of the plugins is NULL / empty
PluginNode *loaded_plugins = NULL;

void load_plugin(const char *plugin_name)
{
    char plugin_path[256];
    snprintf(plugin_path, sizeof(plugin_path), "./%s.so", plugin_name);
    printf("Loading plugin: %s\n", plugin_path);

    void *plugin = dlopen(plugin_path, RTLD_LAZY);
    if (plugin != NULL)
    {
        int (*initialize)() = dlsym(plugin, "initialize");
        int (*run)(char **) = dlsym(plugin, "run");

        char *error = dlerror();
        if (error != NULL)
        {
            fprintf(stderr, "Error: dlsym failed for plugin %s: %s\n", plugin_name, error);
            dlclose(plugin);
            return;
        }

        int init_result = initialize();
        if (init_result == 0)
        {
            // Create a new node for the loaded plugin
            PluginNode *new_p = malloc(sizeof(PluginNode));
            if (new_p != NULL)
            {
                strcpy(new_p->name, plugin_name);
                new_p->plugin = plugin;
                new_p->initialize = initialize;
                new_p->run = run;
                new_p->next = loaded_plugins;

                // Update the head of the linked list
                loaded_plugins = new_p;

                // Successful initialization
                printf("Plugin %s loaded and initialized\n", plugin_name);
            }
            else
            {
                fprintf(stderr, "Error: Memory allocation failed for plugin %s\n", plugin_name);
            }
        }
        else
        {
            // Initialization failed, print an error message
            fprintf(stderr, "Error: Plugin %s initialization failed!\n", plugin_name);
            dlclose(plugin);
        }
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
        dlclose(current->plugin);
        free(current);
        current = next;
    }
}

void execute_command(char *args[])
{
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
        else if (pid > 0)
        {
            // Parent process
            waitpid(pid, NULL, 0); // Wait for the child to complete
        }
        else
        {
            // Fork failed
            perror("Fork failed");
        }
    }
}

int main()
{
    char input[200];
    while (1)
    {
        printf("> ");
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            // Handle end-of-file (Ctrl+D) to exit the shell
            printf("\n");
            break;
        }

        // Remove trailing newline character
        input[strcspn(input, "\n")] = '\0';

        // Tokenize input into arguments
        char *args[MAX_ARGS];
        char *token = strtok(input, " ");
        int i = 0;
        while (token != NULL && i < MAX_ARGS - 1)
        {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // Null-terminate the argument list

        // Execute the command

        execute_command(args);
    }

    // Unload plugins before exiting
    unload_plugins();

    return 0;
}
