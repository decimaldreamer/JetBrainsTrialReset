#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void deleteFile(const char *path) {
    if (DeleteFile(path)) {
        printf("Deleted: %s\n", path);
    } else {
        printf("Failed to delete: %s\n", path);
    }
}

void deleteRegistryKey(HKEY hKeyRoot, const char *subKey) {
    LONG result = RegDeleteKeyEx(hKeyRoot, subKey, KEY_WOW64_64KEY, 0);
    if (result == ERROR_SUCCESS) {
        printf("Deleted registry key: %s\n", subKey);
    } else {
        printf("Failed to delete registry key: %s (Error: %ld)\n", subKey, result);
    }
}

void removeJetBrainsFiles() {
    char appDataPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, appDataPath))) {
        char jetBrainsPath[MAX_PATH];
        snprintf(jetBrainsPath, MAX_PATH, "%s\\JetBrains", appDataPath);

        deleteFile("%APPDATA%\\JetBrains\\PermanentUserId");
        deleteFile("%APPDATA%\\JetBrains\\PermanentDeviceId");

        WIN32_FIND_DATA findFileData;
        HANDLE hFind;
        char searchPath[MAX_PATH];
        snprintf(searchPath, MAX_PATH, "%s\\JetBrains\\*\\*.key", appDataPath);

        hFind = FindFirstFile(searchPath, &findFileData);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                char filePath[MAX_PATH];
                snprintf(filePath, MAX_PATH, "%s\\JetBrains\\%s", appDataPath, findFileData.cFileName);
                deleteFile(filePath);
            } while (FindNextFile(hFind, &findFileData));
            FindClose(hFind);
        }
    } else {
        printf("Failed to retrieve APPDATA path.\n");
    }
}

int main() {
    if (!IsUserAnAdmin()) {
        printf("This program requires administrator privileges. Please run as administrator.\n");
        system("pause");
        return 1;
    }

    printf("Starting cleanup process...\n");

    removeJetBrainsFiles();

    printf("Clearing JavaSoft registry entries...\n");
    deleteRegistryKey(HKEY_CURRENT_USER, "Software\\JavaSoft");

    printf("Cleanup completed.\n");
    system("pause");

    return 0;
}
