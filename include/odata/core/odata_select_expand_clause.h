//---------------------------------------------------------------------
// <copyright file="odata_select_expand_clause.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"

namespace odata { namespace core
{

class odata_select_item;
class odata_expand_item;

class odata_select_expand_clause
{
public:
	odata_select_expand_clause(
        std::vector<std::shared_ptr<::odata::core::odata_select_item>> &&select_items,
        std::vector<std::shared_ptr<::odata::core::odata_expand_item>> &&expand_items)
        : m_select_items(select_items),
        m_expand_items(expand_items) {}
	~odata_select_expand_clause() {}
    
    const std::vector<std::shared_ptr<::odata::core::odata_select_item>> &select_items() const
    { return m_select_items; }
    std::shared_ptr<::odata::core::odata_select_item> select_item_at(::size_t i) const
    { return m_select_items[i]; }
    
    const std::vector<std::shared_ptr<::odata::core::odata_expand_item>> &expand_items() const
    { return m_expand_items; }
    std::shared_ptr<::odata::core::odata_expand_item> expand_item_at(::size_t i) const
    { return m_expand_items[i]; }
    
private:
    std::vector<std::shared_ptr<::odata::core::odata_select_item>> m_select_items;
    std::vector<std::shared_ptr<::odata::core::odata_expand_item>> m_expand_items;
};

}}