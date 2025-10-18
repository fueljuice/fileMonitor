#include "registryChanges.h"
int main()
{
    int a = registryChanges(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 100, FALSE);
    printf("return value is:, %d", a);


}
