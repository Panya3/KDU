/*******************************************************************************
*
*  TITLE:       HAMAKAZE.H
*
*  Main header for Hamakaze static library.
*
*******************************************************************************/

#pragma once

#include <Windows.h>
#include "shared/kdulog.h"

#if defined(__cplusplus)
extern "C" {
#endif

//
// Map driver from raw PE image memory buffer
//
BOOL WINAPI KDUMapDriverFromMemory(
    _In_ ULONG ProviderId,
    _In_ ULONG ShellVersion,
    _In_ PVOID DriverBuffer,
    _In_ SIZE_T DriverBufferSize,
    _In_opt_ LPCWSTR DriverObjectName,
    _In_opt_ LPCWSTR DriverRegistryPath
);

//
// DSE state manipulation
//
BOOL WINAPI KDUControlDSE(
    _In_ ULONG ProviderId,
    _In_ ULONG DSEValue
);

//
// Disable process protection for given PID
//
BOOL WINAPI KDUDisableProcessProtection(
    _In_ ULONG ProviderId,
    _In_ ULONG_PTR ProcessId
);

#if defined(__cplusplus)
}
#endif
