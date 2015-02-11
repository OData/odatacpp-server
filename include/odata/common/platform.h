//---------------------------------------------------------------------
// <copyright file="platform.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#ifndef _MS_WINDOWS
#if defined(_WIN32) || defined(__cplusplus_winrt)
#define _MS_WINDOWS
#endif
#endif // _MS_WINDOWS

#ifdef _MSC_VER
#pragma warning(disable:4146 4521 4522 4566 4996)
#endif

#ifndef ODATACPP_API
#ifdef ODATALIB_EXPORTS
#define ODATACPP_API __declspec(dllexport)
#else
#define ODATACPP_API __declspec(dllimport)
#endif
#endif

// for guids, used in comm.h
#ifdef MS_TARGET_APPLE
#include "odata/common/compat/apple_compat.h"
#else
#ifdef _MS_WINDOWS
#include <Guiddef.h>
#include "odata/common/compat/windows_compat.h"
#else
#include "boost/uuid/uuid.hpp"
#endif
#endif

#define UNREACHABLE __assume(0)

#ifdef _MS_WINDOWS
#include "odata/common/compat/windows_compat.h"
// use the debug version of the CRT if _DEBUG is defined
#ifdef _DEBUG
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#define NOMINMAX
#endif

#include <windows.h>
#include <objbase.h>

// Windows Header Files:
#if !defined(__cplusplus_winrt)
#include <winhttp.h>

#endif // #if !defined(__cplusplus_winrt)
#else // LINUX or APPLE
#ifdef __APPLE__
#include <odata/common/compat/apple_compat.h>
#else
#include <odata/common/compat/linux_compat.h>
#define FAILED(x) ((x) != 0)
#endif
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <cstdint>
#include <string>
#include <sstream>
#include <thread>
#include <atomic>
#include <signal.h>
#include "pthread.h"
#include "boost/locale.hpp"
#include "boost/thread/condition_variable.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include "boost/bind/bind.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#endif // _MS_WINDOWS
