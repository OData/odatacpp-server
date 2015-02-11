//---------------------------------------------------------------------
// <copyright file="xxpublic.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#ifndef _MS_WINDOWS
#if defined(_WIN32) || defined(__cplusplus_winrt)
#define _MS_WINDOWS
#endif
#endif // _MS_WINDOWS


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