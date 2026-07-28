/*******************************************************************************
*
*  TITLE:       TAIGEI.H
*
*  Main header for Taigei static library.
*
*******************************************************************************/

#pragma once

#include <Windows.h>

#if defined(__cplusplus)
extern "C" {
#endif

int ExecutableMain();

#ifndef _WIN64
VOID UnlockCheatEngineDriver(_In_ PVOID ImageBase);
#endif

#if defined(__cplusplus)
}
#endif
