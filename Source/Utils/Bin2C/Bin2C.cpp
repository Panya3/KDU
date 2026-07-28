/*******************************************************************************
*
*  (C) COPYRIGHT AUTHORS, 2026
*
*  TITLE:       BIN2C.CPP
*
*  VERSION:     1.00
*
*  DATE:        28 Jul 2026
*
*  BIN2C - Binary to C array converter utility.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
* TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
* PARTICULAR PURPOSE.
*
*******************************************************************************/

#include <Windows.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#include "../../Shared/ntos/ntos.h"
#include "../../Shared/minirtl/cmdline.h"
}
#endif

INT main(
    _In_ INT argc,
    _In_ CHAR* argv[]
)
{
    LPCSTR inputPath = NULL;
    LPCSTR headerPath = NULL;
    LPCSTR cPath = NULL;
    LPCSTR arrayName = NULL;
    LPCSTR sizeName = NULL;
    BOOL headerOnly = TRUE;

    for (INT i = 1; i < argc; ++i) {
        if ((_stricmp(argv[i], "-i") == 0 || _stricmp(argv[i], "--input") == 0) && i + 1 < argc) {
            inputPath = argv[++i];
        }
        else if ((_stricmp(argv[i], "-oh") == 0 || _stricmp(argv[i], "--out-header") == 0) && i + 1 < argc) {
            headerPath = argv[++i];
        }
        else if ((_stricmp(argv[i], "-oc") == 0 || _stricmp(argv[i], "--out-c") == 0) && i + 1 < argc) {
            cPath = argv[++i];
        }
        else if ((_stricmp(argv[i], "-a") == 0 || _stricmp(argv[i], "--array-name") == 0) && i + 1 < argc) {
            arrayName = argv[++i];
        }
        else if ((_stricmp(argv[i], "-s") == 0 || _stricmp(argv[i], "--size-name") == 0) && i + 1 < argc) {
            sizeName = argv[++i];
        }
        else if (_stricmp(argv[i], "-ho") == 0 || _stricmp(argv[i], "--header-only") == 0) {
            headerOnly = TRUE;
        }
        else if (argv[i][0] != '-' && inputPath == NULL) {
            inputPath = argv[i];
        }
    }

    if (!inputPath) {
        printf("BIN2C - Binary to C Array Converter v1.00\n");
        printf("Usage: bin2c -i <input_file> [-ho] [-oh <header_file>] [-oc <c_file>] [-a <array_name>] [-s <size_name>]\n");
        printf("Options:\n");
        printf("  -i,  --input <path>       Input binary file path\n");
        printf("  -ho, --header-only        Header-only mode (default: true)\n");
        printf("  -oh, --out-header <path>  Output header file path\n");
        printf("  -oc, --out-c <path>       Output C file path\n");
        printf("  -a,  --array-name <name>  Array symbol name\n");
        printf("  -s,  --size-name <name>   Size symbol name\n");
        printf("\nExample: bin2c --input kdu_db.bin\n");
        printf("         bin2c --input kdu_db.bin --header-only\n");
        printf("         bin2c -i data.bin --out-header data.h --out-c data.c --array-name g_Data --size-name g_DataSize\n");
        return -1;
    }

    char autoHeader[MAX_PATH];
    char autoC[MAX_PATH];
    char autoArray[MAX_PATH + 32];
    char autoSize[MAX_PATH + 32];

    // Extract base file name from path
    LPCSTR fileName = strrchr(inputPath, '\\');
    if (!fileName) fileName = strrchr(inputPath, '/');
    fileName = fileName ? fileName + 1 : inputPath;

    LPCSTR dot = strrchr(fileName, '.');
    SIZE_T baseLen = dot ? (SIZE_T)(dot - fileName) : strlen(fileName);
    if (baseLen >= MAX_PATH - 32) baseLen = MAX_PATH - 32;

    char baseName[MAX_PATH];
    strncpy_s(baseName, sizeof(baseName), fileName, baseLen);

    // Make base name C identifier friendly (replace non-alphanumeric with '_')
    for (SIZE_T i = 0; i < baseLen; ++i) {
        if (!isalnum((unsigned char)baseName[i])) {
            baseName[i] = '_';
        }
    }

    strncpy_s(autoHeader, sizeof(autoHeader), baseName, baseLen);
    strcat_s(autoHeader, sizeof(autoHeader), ".h");

    strncpy_s(autoC, sizeof(autoC), baseName, baseLen);
    strcat_s(autoC, sizeof(autoC), ".c");

    snprintf(autoArray, sizeof(autoArray), "g_%sData", baseName);
    snprintf(autoSize, sizeof(autoSize), "g_%sSize", baseName);

    if (!headerPath) headerPath = autoHeader;
    if (!cPath) cPath = autoC;
    if (!arrayName) arrayName = autoArray;
    if (!sizeName) sizeName = autoSize;

    HANDLE hInput = CreateFileA(inputPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hInput == INVALID_HANDLE_VALUE) {
        printf("Error: Cannot open input file %s (err %lu)\n", inputPath, GetLastError());
        return -1;
    }

    DWORD fileSize = GetFileSize(hInput, NULL);
    if (fileSize == INVALID_FILE_SIZE || fileSize == 0) {
        printf("Error: Invalid or empty file\n");
        CloseHandle(hInput);
        return -1;
    }

    BYTE* buffer = (BYTE*)HeapAlloc(GetProcessHeap(), 0, fileSize);
    if (!buffer) {
        printf("Error: Out of memory\n");
        CloseHandle(hInput);
        return -1;
    }

    DWORD bytesRead = 0;
    if (!ReadFile(hInput, buffer, fileSize, &bytesRead, NULL) || bytesRead != fileSize) {
        printf("Error: Failed reading input file\n");
        HeapFree(GetProcessHeap(), 0, buffer);
        CloseHandle(hInput);
        return -1;
    }
    CloseHandle(hInput);

    FILE* fHeader = fopen(headerPath, "w");
    if (!fHeader) {
        printf("Error: Cannot create header file %s\n", headerPath);
        HeapFree(GetProcessHeap(), 0, buffer);
        return -1;
    }

    fprintf(fHeader, "// Generated by BIN2C\n\n");
    fprintf(fHeader, "#pragma once\n\n");
    fprintf(fHeader, "#include <Windows.h>\n\n");
    fprintf(fHeader, "#if defined(__cplusplus)\nextern \"C\" {\n#endif\n\n");

    if (headerOnly) {
        fprintf(fHeader, "const unsigned char %s[%lu] = {\n", arrayName, fileSize);
        for (DWORD i = 0; i < fileSize; ++i) {
            if (i % 16 == 0) {
                fprintf(fHeader, "    ");
            }
            fprintf(fHeader, "0x%02X", buffer[i]);
            if (i + 1 < fileSize) {
                fprintf(fHeader, ", ");
            }
            if ((i + 1) % 16 == 0 || i + 1 == fileSize) {
                fprintf(fHeader, "\n");
            }
        }
        fprintf(fHeader, "};\n\n");
        fprintf(fHeader, "const ULONG %s = %lu;\n\n", sizeName, fileSize);
    }
    else {
        fprintf(fHeader, "extern const unsigned char %s[%lu];\n", arrayName, fileSize);
        fprintf(fHeader, "extern const ULONG %s;\n\n", sizeName);
    }

    fprintf(fHeader, "#if defined(__cplusplus)\n}\n#endif\n");
    fclose(fHeader);

    if (!headerOnly) {
        FILE* fC = fopen(cPath, "w");
        if (!fC) {
            printf("Error: Cannot create C file %s\n", cPath);
            HeapFree(GetProcessHeap(), 0, buffer);
            return -1;
        }

        fprintf(fC, "// Generated by BIN2C\n\n");
        fprintf(fC, "#include <Windows.h>\n\n");
        fprintf(fC, "#if defined(__cplusplus)\nextern \"C\" {\n#endif\n\n");
        fprintf(fC, "const unsigned char %s[%lu] = {\n", arrayName, fileSize);

        for (DWORD i = 0; i < fileSize; ++i) {
            if (i % 16 == 0) {
                fprintf(fC, "    ");
            }
            fprintf(fC, "0x%02X", buffer[i]);
            if (i + 1 < fileSize) {
                fprintf(fC, ", ");
            }
            if ((i + 1) % 16 == 0 || i + 1 == fileSize) {
                fprintf(fC, "\n");
            }
        }

        fprintf(fC, "};\n\n");
        fprintf(fC, "const ULONG %s = %lu;\n\n", sizeName, fileSize);
        fprintf(fC, "#if defined(__cplusplus)\n}\n#endif\n");

        fclose(fC);
    }

    HeapFree(GetProcessHeap(), 0, buffer);

    if (headerOnly) {
        printf("Successfully converted %s (%lu bytes) -> %s (header-only)\n", inputPath, fileSize, headerPath);
    }
    else {
        printf("Successfully converted %s (%lu bytes) -> %s, %s\n", inputPath, fileSize, headerPath, cPath);
    }
    return 0;
}
