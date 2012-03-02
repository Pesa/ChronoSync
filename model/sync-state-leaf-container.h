/* -*- Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2012 University of California, Los Angeles
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Zhenkai Zhu <zhenkai@cs.ucla.edu>
 *         卞超轶 Chaoyi Bian <bcy@pku.edu.cn>
 *	   Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef SYNC_STATE_LEAF_CONTAINER
#define SYNC_STATE_LEAF_CONTAINER

#include <boost/multi_index_container.hpp>
// #include <boost/multi_index/tag.hpp>
// #include <boost/multi_index/ordered_index.hpp>
// #include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
// #include <boost/multi_index/random_access_index.hpp>
// #include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>

namespace mi = boost::multi_index;

namespace ns3 {
namespace Sync {

/**
 * \ingroup sync
 * @brief Container for SYNC leaves
 */
struct LeafContainer : public mi::multi_index_container<
    LeafPtr,
    mi::indexed_by<
      // For fast access to elements using NameInfo
      mi::hashed_unique<
        mi::const_mem_fun<Leaf, const NameInfo&, &Leaf::getInfo>,
        LeafHash (),
        LeafEqual () 
      >
    >
   >
{
};

} // Sync
} // ns3

#endif // SYNC_STATE_LEAF_CONTAINER
