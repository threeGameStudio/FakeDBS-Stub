#include "fakedbs_common.h"

#include <direct.h>
#include <stdlib.h>
#include <string.h>

#define SCRIPT_LINE_SIZE 4096
#define DIRECTORY_SIZE 256

/* RE_AGENT_HOOK(extract_dir, 0x00401486)
 * Recovered from extract_dir() at 0x00401486.
 */
static void extract_dir(char *path, char *directory)
{
    char *backslash = strrchr(path, '\\');
    char *slash = strrchr(path, '/');
    size_t length;

    /* The original compares these pointers directly, including NULL. */
    if (backslash > slash) {
        length = (size_t)(backslash - path) + 1;
        memcpy(directory, path, length);
        directory[length] = '\0';
    } else if (slash > backslash) {
        length = (size_t)(slash - path) + 1;
        memcpy(directory, path, length);
        directory[length] = '\0';
    } else {
        memcpy(directory, ".", 2);
    }
}

/* RE_AGENT_HOOK(main, 0x0040154d)
 * Recovered from main() at 0x0040154d.
 */
int main(int argc, char **argv)
{
    char line[SCRIPT_LINE_SIZE];
    char directory[DIRECTORY_SIZE];
    char *project = "<NOTSET>";
    char *script_name = NULL;
    char *newline;
    FILE *script;
    int process_id;
    int i;

    process_id = (int)GetCurrentProcessId();
    msgf("[%d] STARTED\n", process_id);

    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-p") == 0) {
            project = argv[++i];
        } else if (strcmp(argv[i], "-s") == 0) {
            script_name = argv[++i];
        } else {
            msgf("[%d] Skipping option '%s'", process_id, argv[i]);
        }
    }

    msgf("[%d] BUILDING PROJECT %s\n", process_id, project);

    if (script_name == NULL) {
        msgf("[%d] Script name not specified!\n", process_id);
        msgf("[%d] EXITING\n", process_id);
        return 0;
    }

    script = fopen(script_name, "r");
    if (script == NULL) {
        msgf("[%d] Cannot open script file '%s'\n", process_id, script_name);
        msgf("[%d] EXITING\n", process_id);
        return 0;
    }

    extract_dir(script_name, directory);
    _chdir(directory);
    msgf("[%d] Executing script '%s'...\n", process_id, script_name);

    while (fgets(line, sizeof(line), script) != NULL) {
        newline = strrchr(line, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }
        msgf("[%d] %s\n", process_id, line);
        system(line);
    }

    fclose(script);
    msgf("[%d] DONE.", process_id);
    return 0;
}
