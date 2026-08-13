#ifndef FAKEDBS_COMMON_H
#define FAKEDBS_COMMON_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdarg.h>
#include <stdio.h>

#define MAILSLOT_NAME "\\\\.\\mailslot\\FakeDBS_MS"
#define MESSAGE_BUFFER_SIZE 8192

/* RE_AGENT_HOOK(msgf, 0x004013c0)
 * Recovered from msgf() at 0x004013c0 in dbsbuild64.exe/dbsutil.exe.
 * This intentionally preserves the original behavior, including the lack of
 * truncation/error handling around _vsnprintf().
 */
static void msgf(char *format, ...)
{
    char buffer[MESSAGE_BUFFER_SIZE];
    va_list args;
    int length;
    DWORD written;
    HANDLE mailslot;

    va_start(args, format);
    length = _vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    mailslot = CreateFileA(
        MAILSLOT_NAME,
        GENERIC_WRITE,
        FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (mailslot != INVALID_HANDLE_VALUE) {
        WriteFile(mailslot, buffer, length, &written, NULL);
        CloseHandle(mailslot);
    }
}

#endif
