#ifndef typedef_h__
#define typedef_h__
#include <gce/actor/all.hpp>
#include <gce/adata/cpp/adata.hpp>
#include "opcode.h"
#include "internal/msg_cache.adl.h"
#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>

struct expired_item
{
    uint64_t transcation_id;
    boost::shared_ptr<p::xs2ds_entity_req> data;
};

#endif // typedef_h__
