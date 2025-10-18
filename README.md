# registryChanges
registryChanges mainly uses RegNotifyChangeKeyValue and an event HANDLE from the winapi to asynchonrchly monitor a registrey subkey.

# monitorDirChanges
monitorDirChanges prints out to console any changes in a specified directory. 
it is a build on someones existing code. i added some checks' optimaztion and a time limit. addtioanlly i added a init function to use it easily.
in order to monitor it uses ReadDirectoryChangesW winapi function and an Overlapped struct. the overlapped struct is used for asynchrouns actions.
