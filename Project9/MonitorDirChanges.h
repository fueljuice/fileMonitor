#ifndef MONITOR_DIR_H
#define MONITOR_DIR_H

#include <stdint.h>
#include <windows.h>
#include <time.h>
#include <stdio.h>


DWORD WINAPI monitorDirChanges(int timeToMonitor);

int initDirMonitor();



#endif