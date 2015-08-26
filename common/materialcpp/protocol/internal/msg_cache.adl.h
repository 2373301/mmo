#ifndef msg_cache_adl_h_adata_header_define
#define msg_cache_adl_h_adata_header_define

#include <adata.hpp>

namespace p {
  struct entity_req_type
  {
    uint8_t get;
    uint8_t set;
    entity_req_type()
    :    get(0),
    set(1)
    {}
  };

  struct xs2ds_entity_req
  {
    uint8_t req_type;
    uint64_t req_guid;
    ::std::string data;
    xs2ds_entity_req()
    :    req_type(0),
    req_guid(0ULL)
    {}
  };

  struct ds2xs_entity_ack
  {
    uint8_t req_type;
    uint8_t result;
    uint64_t req_guid;
    ::std::string data;
    ds2xs_entity_ack()
    :    req_type(0),
    result(0),
    req_guid(0ULL)
    {}
  };

}

namespace adata
{
  template<typename stream_ty>
  ADATA_INLINE void read( stream_ty& stream, ::p::entity_req_type& value)
  {
    ::std::size_t offset = stream.read_length();
    uint64_t tag = 0;
    read(stream,tag);
    if(stream.error()){return;}
    int32_t len_tag = 0;
    read(stream,len_tag);
    if(stream.error()){return;}

    if(tag&1ULL)    {read(stream,value.get);{if(stream.error()){stream.trace_error("get",-1);return;}}}
    if(tag&2ULL)    {read(stream,value.set);{if(stream.error()){stream.trace_error("set",-1);return;}}}
    if(len_tag >= 0)
    {
      ::std::size_t read_len = stream.read_length() - offset;
      ::std::size_t len = (::std::size_t)len_tag;
      if(len > read_len) stream.skip_read(len - read_len);
    }
  }

  template <typename stream_ty>
  ADATA_INLINE void skip_read(stream_ty& stream, ::p::entity_req_type* value)
  {
    (value);
    skip_read_compatible(stream);
  }

  ADATA_INLINE int32_t size_of(const ::p::entity_req_type& value)
  {
    int32_t size = 0;
    uint64_t tag = 3ULL;
    {
      size += size_of(value.get);
    }
    {
      size += size_of(value.set);
    }
    size += size_of(tag);
    size += size_of(size + size_of(size));
    return size;
  }

  template<typename stream_ty>
  ADATA_INLINE void write(stream_ty& stream , const ::p::entity_req_type&value)
  {
    uint64_t tag = 3ULL;
    write(stream,tag);
    if(stream.error()){return;}
    write(stream,size_of(value));
    if(stream.error()){return;}
    {write(stream,value.get);{if(stream.error()){stream.trace_error("get",-1);return;}}}
    {write(stream,value.set);{if(stream.error()){stream.trace_error("set",-1);return;}}}
    return;
  }

  template<typename stream_ty>
  ADATA_INLINE void read( stream_ty& stream, ::p::xs2ds_entity_req& value)
  {
    ::std::size_t offset = stream.read_length();
    uint64_t tag = 0;
    read(stream,tag);
    if(stream.error()){return;}
    int32_t len_tag = 0;
    read(stream,len_tag);
    if(stream.error()){return;}

    if(tag&1ULL)    {read(stream,value.req_guid);{if(stream.error()){stream.trace_error("req_guid",-1);return;}}}
    if(tag&2ULL)    {read(stream,value.req_type);{if(stream.error()){stream.trace_error("req_type",-1);return;}}}
    if(tag&4ULL)
    {
      uint32_t len = check_read_size(stream);
      {if(stream.error()){stream.trace_error("data",-1);return;}}
      value.data.resize(len);
      stream.read((char *)value.data.data(),len);
      {if(stream.error()){stream.trace_error("data",-1);return;}}
    }
    if(len_tag >= 0)
    {
      ::std::size_t read_len = stream.read_length() - offset;
      ::std::size_t len = (::std::size_t)len_tag;
      if(len > read_len) stream.skip_read(len - read_len);
    }
  }

  template <typename stream_ty>
  ADATA_INLINE void skip_read(stream_ty& stream, ::p::xs2ds_entity_req* value)
  {
    (value);
    skip_read_compatible(stream);
  }

  ADATA_INLINE int32_t size_of(const ::p::xs2ds_entity_req& value)
  {
    int32_t size = 0;
    uint64_t tag = 3ULL;
    if(!value.data.empty()){tag|=4ULL;}
    {
      size += size_of(value.req_guid);
    }
    {
      size += size_of(value.req_type);
    }
    if(tag&4ULL)
    {
      int32_t len = (int32_t)(value.data).size();
      size += size_of(len);
      size += len;
    }
    size += size_of(tag);
    size += size_of(size + size_of(size));
    return size;
  }

  template<typename stream_ty>
  ADATA_INLINE void write(stream_ty& stream , const ::p::xs2ds_entity_req&value)
  {
    uint64_t tag = 3ULL;
    if(!value.data.empty()){tag|=4ULL;}
    write(stream,tag);
    if(stream.error()){return;}
    write(stream,size_of(value));
    if(stream.error()){return;}
    {write(stream,value.req_guid);{if(stream.error()){stream.trace_error("req_guid",-1);return;}}}
    {write(stream,value.req_type);{if(stream.error()){stream.trace_error("req_type",-1);return;}}}
    if(tag&4ULL)
    {
      uint32_t len = (uint32_t)(value.data).size();
      write(stream,len);
      stream.write((value.data).data(),len);
      {if(stream.error()){stream.trace_error("data",-1);return;}}
    }
    return;
  }

  template<typename stream_ty>
  ADATA_INLINE void read( stream_ty& stream, ::p::ds2xs_entity_ack& value)
  {
    ::std::size_t offset = stream.read_length();
    uint64_t tag = 0;
    read(stream,tag);
    if(stream.error()){return;}
    int32_t len_tag = 0;
    read(stream,len_tag);
    if(stream.error()){return;}

    if(tag&1ULL)    {read(stream,value.req_guid);{if(stream.error()){stream.trace_error("req_guid",-1);return;}}}
    if(tag&2ULL)    {read(stream,value.req_type);{if(stream.error()){stream.trace_error("req_type",-1);return;}}}
    if(tag&4ULL)    {read(stream,value.result);{if(stream.error()){stream.trace_error("result",-1);return;}}}
    if(tag&8ULL)
    {
      uint32_t len = check_read_size(stream);
      {if(stream.error()){stream.trace_error("data",-1);return;}}
      value.data.resize(len);
      stream.read((char *)value.data.data(),len);
      {if(stream.error()){stream.trace_error("data",-1);return;}}
    }
    if(len_tag >= 0)
    {
      ::std::size_t read_len = stream.read_length() - offset;
      ::std::size_t len = (::std::size_t)len_tag;
      if(len > read_len) stream.skip_read(len - read_len);
    }
  }

  template <typename stream_ty>
  ADATA_INLINE void skip_read(stream_ty& stream, ::p::ds2xs_entity_ack* value)
  {
    (value);
    skip_read_compatible(stream);
  }

  ADATA_INLINE int32_t size_of(const ::p::ds2xs_entity_ack& value)
  {
    int32_t size = 0;
    uint64_t tag = 7ULL;
    if(!value.data.empty()){tag|=8ULL;}
    {
      size += size_of(value.req_guid);
    }
    {
      size += size_of(value.req_type);
    }
    {
      size += size_of(value.result);
    }
    if(tag&8ULL)
    {
      int32_t len = (int32_t)(value.data).size();
      size += size_of(len);
      size += len;
    }
    size += size_of(tag);
    size += size_of(size + size_of(size));
    return size;
  }

  template<typename stream_ty>
  ADATA_INLINE void write(stream_ty& stream , const ::p::ds2xs_entity_ack&value)
  {
    uint64_t tag = 7ULL;
    if(!value.data.empty()){tag|=8ULL;}
    write(stream,tag);
    if(stream.error()){return;}
    write(stream,size_of(value));
    if(stream.error()){return;}
    {write(stream,value.req_guid);{if(stream.error()){stream.trace_error("req_guid",-1);return;}}}
    {write(stream,value.req_type);{if(stream.error()){stream.trace_error("req_type",-1);return;}}}
    {write(stream,value.result);{if(stream.error()){stream.trace_error("result",-1);return;}}}
    if(tag&8ULL)
    {
      uint32_t len = (uint32_t)(value.data).size();
      write(stream,len);
      stream.write((value.data).data(),len);
      {if(stream.error()){stream.trace_error("data",-1);return;}}
    }
    return;
  }

}

#endif
