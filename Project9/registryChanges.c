#include "registryChanges.h"

// gets a root HKEY (for example HKEY_LOCAL_MACHINE) "A handle to an open registry key" ( mdsn)
// subkey is "The name of the registry subkey to be opened" (from mdsn)


LSTATUS isMonitorKeyResult(HANDLE hEvent, HKEY hKey, BOOL bWatchSubtree)
{
    // documention from mdsn to RegNotifyChangeKeyValue
    //LSTATUS RegNotifyChangeKeyValue(
    //    [in]           HKEY   hKey,
    //    [in]           BOOL   bWatchSubtree,
    //    [in]           DWORD  dwNotifyFilter,
    //    [in, optional] HANDLE hEvent,
    //    [in]           BOOL   fAsynchronous
    //);
    LSTATUS monitorKeyResult = RegNotifyChangeKeyValue(
        hKey,
        bWatchSubtree, // watch the subtree
        REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_ATTRIBUTES |
        REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_CHANGE_SECURITY |
        REG_NOTIFY_THREAD_AGNOSTIC,
        hEvent,     // inserts the mointioring into the hEvent HANDLE object
        TRUE // true since hEvEnt is async
    );


    if (monitorKeyResult != ERROR_SUCCESS)
    {
        printf("RegNotifyChangeKeyValue failed");
        return monitorKeyResult;
    }

    return ERROR_SUCCESS;
}


DWORD WINAPI registryChanges(HKEY root, const wchar_t* subkey, double secToWait, BOOL bWatchSubtree)
{
    double monitoringTime = secToWait;
    char changeInfoBuf[512];
    double timePassed;
    HKEY hKey = NULL;
    LSTATUS openKeyResult;
    LSTATUS monitorKeyResult;
    DWORD eventStatus;
    HANDLE hEvent;
    time_t currentTime, startTime;

    // mdsn documation for RegOpenKeyExW:
    //LSTATUS RegOpenKeyExW(
    //    [in]           HKEY    hKey,
    //    [in, optional] LPCWSTR lpSubKey,
    //    [in]           DWORD   ulOptions,
    //    [in]           REGSAM  samDesired,
    //    [out]          PHKEY   phkResult
    //);

    openKeyResult = RegOpenKeyExW(
        root,
        subkey, 
        0,
        KEY_NOTIFY,
        &hKey
    );

    if (openKeyResult != ERROR_SUCCESS)
    {
        printf("subkey or rootkey entered are invalid\n");
        return 1;
    }



    // creates an event
    hEvent = CreateEventW(NULL, FALSE, 0, NULL);

    if (!hEvent)
    {
        printf("eror creating event\n");
        RegCloseKey(hKey);
        return 1;
    }


    monitorKeyResult = isMonitorKeyResult(hEvent, hKey, bWatchSubtree);
    if (monitorKeyResult != ERROR_SUCCESS)
    {
        CloseHandle(hEvent);
        RegCloseKey(hKey);
        return monitorKeyResult;
    }
    printf("press ctrl c to stop\n");

    // start the time count and enter loop
    time(&startTime);
    while (1)
    {
        // calculatoe cuureent time
        time(&currentTime);

        // wait for event to occur ( a change in the directory)
        eventStatus = WaitForSingleObject(hEvent, 1000);
        if (eventStatus == WAIT_OBJECT_0)
        {
            // since subkey parameter is passed as a wide char pointer printf cant print it.
           // the solution it to use microsoft WideCharToMultiByte
            //int WideCharToMultiByte(
            //    [in]            UINT                               CodePage,
            //    [in]            DWORD                              dwFlags,
            //    [in]            _In_NLS_string_(cchWideChar)LPCWCH lpWideCharStr,
            //    [in]            int                                cchWideChar,
            //    [out, optional] LPSTR                              lpMultiByteStr,
            //    [in]            int                                cbMultiByte,
            //    [in, optional]  LPCCH                              lpDefaultChar,
            //    [out, optional] LPBOOL                             lpUsedDefaultChar
            //);
            WideCharToMultiByte(CP_UTF8, 0, subkey, -1, changeInfoBuf, sizeof(changeInfoBuf), NULL, NULL);
            printf("Registrey was changed under, %s \n", changeInfoBuf);
            // refesh event so no infinitly print the same thing
            monitorKeyResult = isMonitorKeyResult(hEvent, hKey, bWatchSubtree);
            if (monitorKeyResult != ERROR_SUCCESS)
            {
                CloseHandle(hEvent);
                RegCloseKey(hKey);
                return monitorKeyResult;
            }
        }

        if (eventStatus == WAIT_FAILED)
        {
            printf("wait fail");
            break;
        }
        // iff time passed is bigger than time entered exit
        timePassed = difftime(currentTime, startTime);
        if (timePassed >= monitoringTime) 
        {
            break;
        }


    }
    CloseHandle(hEvent);
    RegCloseKey(hKey);
    return ERROR_SUCCESS;
}

    