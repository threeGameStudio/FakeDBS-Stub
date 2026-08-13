#include "fakedbs_common.h"

#include <string.h>

/* Recovered from main() at 0x00401486. */
int main(int argc, char **argv)
{
    int process_id = (int)GetCurrentProcessId();
    int i;

    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-break") == 0) {
            msgf("[%d] DBSUTIL: -break recieved\n", process_id);
            return 0;
        }
        if (strcmp(argv[i], "-restart-agent") == 0) {
            msgf("[%d] DBSUTIL: -restart-agent recieved\n", process_id);
            return 0;
        }
    }

    msgf("[%d] DBSUTIL: unknown command line '%s'\n",
         process_id, GetCommandLineA());
    return 0;
}
