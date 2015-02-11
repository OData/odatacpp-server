//---------------------------------------------------------------------
// <copyright file="odata_tests.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/core/odata_core.h"
#include "odata/edm/odata_edm.h"
#include "odata/common/utility.h"
#include "odata/common/uri.h"
#include "odata/common/asyncrt_utils.h"
#include "odata/common/json.h"
//#include "odata/common/http_client.h"

#include "unittestpp.h"
#include "os_utilities.h"

extern const char* test_model_string;
extern std::shared_ptr<::odata::edm::edm_model> g_test_model;
extern ::odata::utility::string_t g_service_root_url;

std::shared_ptr<::odata::edm::edm_model> get_test_model();