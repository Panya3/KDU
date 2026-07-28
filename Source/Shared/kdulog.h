/*******************************************************************************
*
*  TITLE:       KDULOG.H
*
*  Common logging interface for KDU library.
*
*******************************************************************************/

#pragma once

typedef enum _KDU_LOG_LEVEL {
    KDU_LOG_INFO = 0,
    KDU_LOG_WARNING,
    KDU_LOG_ERROR,
    KDU_LOG_DEBUG
} KDU_LOG_LEVEL;

#if defined(__cplusplus)
extern "C" {
#endif

//
// External logger function provided by the host application.
//
void KduLog(KDU_LOG_LEVEL level, const char* fmt, ...);

#if defined(__cplusplus)
}
#endif
