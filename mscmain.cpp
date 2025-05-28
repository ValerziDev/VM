#include <iostream>
#include <string>
#include <windows.h>
#include <algorithm>
#include <shlobj.h>
#include <fstream>
#include <conio.h>

using namespace std;

string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return tolower(c); });
    return s;
}

bool folderExists(const string& path) {
    DWORD ftyp = GetFileAttributesA(path.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
        return true;
    return false;
}

bool folderHasFiles(const string& path) {
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind;
    string searchPath = path + "\\*.*";

    hFind = FindFirstFileA(searchPath.c_str(), &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return false;
    }

    bool hasFiles = false;
    do {
        string fileName = findFileData.cFileName;
        if (fileName != "." && fileName != ".." && !(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            hasFiles = true;
            break;
        }
    } while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);
    return hasFiles;
}

bool createFolder(const string& path) {
    if (CreateDirectoryA(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
        return true;
    }
    return false;
}

bool copyFiles(const string& sourceDir, const string& destDir) {
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind;
    string searchPath = sourceDir + "\\*.*";

    hFind = FindFirstFileA(searchPath.c_str(), &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        cout << "No files found to copy in " << sourceDir << endl;
        return false;
    }

    bool success = true;

    do {
        string fileName = findFileData.cFileName;
        if (fileName == "." || fileName == "..") continue;
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;

        string sourceFile = sourceDir + "\\" + fileName;
        string destFile = destDir + "\\" + fileName;

        if (!CopyFileA(sourceFile.c_str(), destFile.c_str(), FALSE)) {
            cout << "Failed to copy " << fileName << endl;
            success = false;
        }
    } while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);
    return success;
}

//etsii MSC save path

string getMySummerCarSavePath() {
    char path[MAX_PATH];
    if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path) != S_OK) {
        cerr << "Failed to get AppData path" << endl;
        return "";
    }

    string fullPath(path);
    fullPath += "\\..\\LocalLow\\Amistech\\My Summer Car";
    return fullPath;
}

//tarkistaa valenox kansion olemassa olon






//tarkistaa version

int main() {
    string basePath = "C:\\Valenox";
    

   

    //save slot koodit

    while (true) {
        cout << "\nSave slots status:\n";
        for (int i = 1; i <= 5; ++i) {
            string slotFolder = basePath + "\\Slot" + to_string(i);
            cout << "Slot " << i << ": ";
            if (folderExists(slotFolder) && folderHasFiles(slotFolder)) {
                cout << "In Use\n";
            }
            else {
                cout << "Empty\n";
            }
        }

        cout << "\nSelect your mode:\nModes: Loadsave, Deletesave, Changesave, CreateSlots, Implement, Exit\n";
        

        string selectmode;
        bool clearInput = false;

        
        while (true) {
            if (GetAsyncKeyState(VK_RSHIFT) & 0x8000) {
                clearInput = true;
                cout << "[Input cleared]\n";
                break;
            }
            if (_kbhit()) break;
            Sleep(50);
        }

        if (clearInput) {
            continue;
        }

        cout << "> ";
        getline(cin, selectmode);
        selectmode = toLower(selectmode);

        if (selectmode == "exit") {
            cout << "Exiting..." << endl;
            break;
        }

        //tekee slotit

        if (selectmode == "createslots") {
            if (!folderExists(basePath)) {
                if (createFolder(basePath))
                    cout << "Created base directory: " << basePath << endl;
                else {
                    cout << "Failed to create base directory: " << basePath << endl;
                    continue;
                }
            }

            for (int i = 1; i <= 5; ++i) {
                string slotFolder = basePath + "\\Slot" + to_string(i);
                if (!folderExists(slotFolder)) {
                    if (createFolder(slotFolder))
                        cout << "Created folder: " << slotFolder << endl;
                    else
                        cout << "Failed to create folder: " << slotFolder << endl;
                }
                else {
                    cout << "Folder already exists: " << slotFolder << endl;
                }
            }
        }
        else if (selectmode == "loadsave") {
            cout << "Select slot number (1-5): ";
            string slotStr;
            getline(cin, slotStr);
            int slotNum = stoi(slotStr);

            if (slotNum < 1 || slotNum > 5) {
                cout << "Invalid slot number." << endl;
                continue;
            }

            string slotFolder = basePath + "\\Slot" + to_string(slotNum);
            if (!folderExists(slotFolder) || !folderHasFiles(slotFolder)) {
                cout << "Selected slot is empty or doesn't exist." << endl;
                continue;
            }

            string mscPath = getMySummerCarSavePath();
            if (mscPath.empty()) {
                cout << "Failed to locate My Summer Car save folder." << endl;
                continue;
            }

            cout << "Copying files from Slot " << slotNum << " to MSC save folder...\n";
            if (copyFiles(slotFolder, mscPath)) {
                cout << "Save loaded successfully!" << endl;
            }
            else {
                cout << "Some files failed to copy." << endl;
            }
        }
        else if (selectmode == "implement") {
            cout << "Select slot number (1-5) to save current MSC files into: ";
            string slotStr;
            getline(cin, slotStr);
            int slotNum = stoi(slotStr);

            if (slotNum < 1 || slotNum > 5) {
                cout << "Invalid slot number." << endl;
                continue;
            }

            string slotFolder = basePath + "\\Slot" + to_string(slotNum);
            if (!folderExists(slotFolder)) {
                cout << "Slot folder doesn't exist. Creating it now...\n";
                if (!createFolder(slotFolder)) {
                    cout << "Failed to create slot folder." << endl;
                    continue;
                }
            }

            string mscPath = getMySummerCarSavePath();
            if (mscPath.empty()) {
                cout << "Failed to locate My Summer Car save folder." << endl;
                continue;
            }

            if (!folderHasFiles(mscPath)) {
                cout << "No save files found in MSC folder to save." << endl;
                continue;
            }

            cout << "Copying files from MSC save folder to Slot " << slotNum << "...\n";
            if (copyFiles(mscPath, slotFolder)) {
                cout << "Save implemented successfully!" << endl;
            }
            else {
                cout << "Some files failed to copy." << endl;
            }
        }
        else if (selectmode == "deletesave") {
            cout << "Select slot number (1-5) to delete: ";
            string slotStr;
            getline(cin, slotStr);
            int slotNum = stoi(slotStr);

            if (slotNum < 1 || slotNum > 5) {
                cout << "Invalid slot number." << endl;
                continue;
            }

            string slotFolder = basePath + "\\Slot" + to_string(slotNum);
            if (!folderExists(slotFolder)) {
                cout << "Slot folder doesn't exist or is already empty." << endl;
                continue;
            }

            cout << "Are you sure you want to delete all files in Slot " << slotNum << "? (yes/no): ";
            string confirm;
            getline(cin, confirm);
            confirm = toLower(confirm);

            if (confirm == "yes" || confirm == "y") {
                WIN32_FIND_DATAA findFileData;
                HANDLE hFind;
                string searchPath = slotFolder + "\\*.*";

                hFind = FindFirstFileA(searchPath.c_str(), &findFileData);
                if (hFind == INVALID_HANDLE_VALUE) {
                    cout << "No files to delete in this slot." << endl;
                }
                else {
                    do {
                        string fileName = findFileData.cFileName;
                        if (fileName != "." && fileName != "..") {
                            string filePath = slotFolder + "\\" + fileName;
                            if (!DeleteFileA(filePath.c_str())) {
                                cout << "Failed to delete " << fileName << endl;
                            }
                        }
                    } while (FindNextFileA(hFind, &findFileData) != 0);
                    FindClose(hFind);
                    cout << "All files deleted from Slot " << slotNum << "." << endl;
                }
            }
            else {
                cout << "Deletion cancelled." << endl;
            }
        }
        else if (selectmode == "changesave") {
            cout << "Changesave mode selected (not implemented yet)." << endl;
        }
        else {
            cout << "Invalid mode selected." << endl;
        }

        cout << "\nPress Enter to continue...";
        cin.get();
        system("cls");
    }

    return 0;
}
