#include <stdint.h>
#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>

LPCWSTR PATH = L"C:\\Users\\zohar\\Desktop\\Monitoring\\";

DWORD WINAPI monitorDirChanges(LPVOID lpParam)
{
    DWORD MonitoringTime = *(PINT)lpParam;

    HANDLE file = CreateFile(PATH,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL);
    if (file == INVALID_HANDLE_VALUE) {
        wprintf(L"Failed to open directory. Error: %d\n", GetLastError());
        return 1;
    }

    OVERLAPPED overlapped = { 0 };
    overlapped.hEvent = CreateEvent(NULL, FALSE, 0, NULL);
    if (overlapped.hEvent == NULL) {
        wprintf(L"Failed to create event. Error: %d\n", GetLastError());
        CloseHandle(file);
        return 1;
    }

    uint8_t change_buf[1024];
    BOOL success = ReadDirectoryChangesW(
        file, change_buf, 1024, TRUE,
        FILE_NOTIFY_CHANGE_FILE_NAME |
        FILE_NOTIFY_CHANGE_DIR_NAME |
        FILE_NOTIFY_CHANGE_LAST_WRITE,
        NULL, &overlapped, NULL);
    if (!success) {
        wprintf(L"Failed to read directory changes. Error: %d\n", GetLastError());
        CloseHandle(overlapped.hEvent);
        CloseHandle(file);
        return 1;
    }

    time_t start_time, current_time;
    time(&start_time);
    while (TRUE) {
        time(&current_time);

        DWORD result = WaitForSingleObject(overlapped.hEvent, 0);
        if (result == WAIT_OBJECT_0) {
            DWORD bytes_transferred;
            GetOverlappedResult(file, &overlapped, &bytes_transferred, FALSE);

            FILE_NOTIFY_INFORMATION* event = (FILE_NOTIFY_INFORMATION*)change_buf;

            for (;;) {
                DWORD name_len = event->FileNameLength / sizeof(wchar_t);

                switch (event->Action) {
                case FILE_ACTION_ADDED: {
                    wprintf(L"       Added: %.*s\n", name_len, event->FileName);
                } break;

                case FILE_ACTION_REMOVED: {
                    wprintf(L"     Removed: %.*s\n", name_len, event->FileName);
                } break;

                case FILE_ACTION_MODIFIED: {
                    wprintf(L"    Modified: %.*s\n", name_len, event->FileName);
                } break;

                case FILE_ACTION_RENAMED_OLD_NAME: {
                    wprintf(L"Renamed from: %.*s\n", name_len, event->FileName);
                } break;

                case FILE_ACTION_RENAMED_NEW_NAME: {
                    wprintf(L"          to: %.*s\n", name_len, event->FileName);
                } break;

                default: {
                    printf("Unknown action!\n");
                } break;
                }
                // Are there more events to handle?
                if (event->NextEntryOffset) {
                    *((uint8_t**)&event) += event->NextEntryOffset;
                }
                else
                    break;
            }

            // Queue the next event
            success = ReadDirectoryChangesW(
                file, change_buf, 1024, TRUE,
                FILE_NOTIFY_CHANGE_FILE_NAME |
                FILE_NOTIFY_CHANGE_DIR_NAME |
                FILE_NOTIFY_CHANGE_LAST_WRITE,
                NULL, &overlapped, NULL);
            if (!success) {
                wprintf(L"Failed to read directory changes. Error: %d\n", GetLastError());
                break;
            }

        }
        double TimePassed = difftime(current_time, start_time);
        if (TimePassed >= MonitoringTime) {
            return 0;
        }
    }

    CloseHandle(overlapped.hEvent);
    CloseHandle(file);
    return 0;
}

DWORD WINAPI registeryChanges(LPVOID lpParam)
{
    OVERLAPPED overlapped = { 0 };
    overlapped.hEvent = CreateEvent(NULL, FALSE, 0, NULL);

    LSTATUS status =  RegNotifyChangeKeyValue(
        HKEY_LOCAL_MACHINE,
        TRUE,
        REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_ATTRIBUTES |
        REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_CHANGE_SECURITY | 
        REG_NOTIFY_THREAD_AGNOSTIC,
        &overlapped,
        TRUE
    );

    while (TRUE)
    {
		DWORD eventStatus = WaitForSingleObject(overlapped.hEvent, 0);
        if (eventStatus == WAIT_OBJECT_0) {



        }






    }



	return 0;
}

int main()
{
    printf("En1sadasdasdasdter the time in abc seconds to monitor the directory:\n ");
    INT monitoringTime = 0;
    scanf_s("%d", &monitoringTime);

    HANDLE DirMonitor = CreateThread(
        NULL,
        0,
        monitorDirChanges,
        &monitoringTime,
        0,
        NULL
    );

    if (DirMonitor == NULL) {
        printf("Failed to create thread. Error: %d\n", GetLastError());
        return 1;
    }

    // Wait for the thread to complete
    WaitForSingleObject(DirMonitor, INFINITE);
    CloseHandle(DirMonitor);

    return 0;
}