#ifndef REGISTRY_CHANGES_H
#define REGISTRY_CHANGES_H

#include <windows.h>   
#include <stdio.h>
#include <time.h>
// registryChanges

DWORD WINAPI registryChanges(HKEY root, const char* subkey);

#endif 