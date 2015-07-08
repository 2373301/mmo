#ifndef msg_guid_adl_h_adata_header_define
#define msg_guid_adl_h_adata_header_define

#include <adata.hpp>

namespace typed {namespace protocol {
  struct xs2gd_guid_req
  {
    uint32_t req_no;
    xs2gd_guid_req()
    :    req_no(0)
    {}
  };

  struct gd2xs_guid_ack
  {
    uint32_t seed;
    uint32_t no;
    uint64_t start_index;
    gd2xs_guid_ack()
    :    seed(0),
    no(0),
    start_index(0ULL)
    {}
  };

}}

namespace adata
{
  template<typename stream_ty>
  ADATA_INLINE void read( stream_ty& stream, ::typed::protocol::xs2gd_guid_req& value)
  {
    ::std::size_t offset = stream.read_length();
    uint64_t tag = 0;
    read(stream,tag);
    if(stream.error()){return;}
    int32_t len_tag = 0;
    read(stream,len_tag);
    if(stream.error()){return;}

    if(tag&1ULL)    {read(stream,value.req_no);{if(stream.error()){stream.trace_error("req_no",-1);return;}}}
    if(len_tag >= 0)
    {
      ::std::size_t read_len = stream.read_length() - offset;
      ::std::size_t len = (::std::size_t)len_tag;
      if(len > read_len) stream.skip_read(len - read_len);
    }
  }

  template <typename stream_ty>
  ADATA_INLINE void skip_read(stream_ty& stream, ::typed::protocol::xs2gd_guid_req* value)
  {
    (value);
    skip_read_compatible(stream);
  }

  ADATA_INLINE int32_t size_of(const ::typed::protocol::xs2gd_guid_req& value)
  {
    int32_t size = 0;
    uint64_t tag = 1ULL;
    {
      size += size_of(value.req_no);
    }
    size += size_of(tag);
    size += size_of(size + size_of(size));
    return size;
  }

  template<typename stream_ty>
  ADATA_INLINE void write(stream_ty& stream , const ::typed::protocol::xs2gd_guid_req&value)
  {
    uint64_t tag = 1ULL;
    write(stream,tag);
    if(stream.error()){return;}
    write(stream,size_of(value));
    if(stream.error()){return;}
    {write(stream,value.req_no);{if(stream.error()){stream.trace_error("req_no",-1);return;}}}
    return;
  }

  template<typename stream_ty>
  ADATA_INLINE void read( stream_ty& stream, ::typed::protocol::gd2xs_guid_ack& value)
  {
    ::std::size_t offset = stream.read_length();
    uint64_t tag = 0;
    read(stream,tag);
    if(stream.error()){return;}
    int32_t len_tag = 0;
    read(stream,len_tag);
    if(stream.error()){return;}

    if(tag&1ULL)    {read(stream,value.seed);{if(stream.error()){stream.trace_error("seed",-1);return;}}}
    if(tag&2ULL)    {read(stream,value.no);{if(stream.error()){stream.trace_error("no",-1);return;}}}
    if(tag&4ULL)    {read(stream,value.start_index);{if(stream.error()){stream.trace_error("start_index",-1);return;}}}
    if(len_tag >= 0)
    {
      ::std::size_t read_len = stream.read_length() - offset;
      ::std::size_t len = (::std::size_t)len_tag;
      if(len > read_len) stream.skip_read(len - read_len);
    }
  }

  template <typename stream_ty>
  ADATA_INLINE void skip_read(stream_ty& stream, ::typed::protocol::gd2xs_guid_ack* value)
  {
    (value);
    skip_read_compatible(stream);
  }

  ADATA_INLINE int32_t size_of(const ::typed::protocol::gd2xs_guid_ack& value)
  {
    int32_t size = 0;
    uint64_t tag = 7ULL;
    {
      size += size_of(value.seed);
    }
    {
      size += size_of(value.no);
    }
    {
      size += size_of(value.start_index);
    }
    size += size_of(tag);
    size += size_of(size + size_of(size));
    return size;
  }

  template<typename stream_ty>
  ADATA_INLINE void write(stream_ty& stream , const ::typed::protocol::gd2xs_guid_ack&value)
  {
    uint64_t tag = 7ULL;
    write(stream,tag);
    if(stream.error()){return;}
    write(stream,size_of(value));
    if(stream.error()){return;}
    {write(stream,value.seed);{if(stream.error()){stream.trace_error("seed",-1);return;}}}
    {write(stream,value.no);{if(stream.error()){stream.trace_error("no",-1);return;}}}
    {write(stream,value.start_index);{if(stream.error()){stream.trace_error("start_index",-1);return;}}}
    return;
  }

}

#endif
