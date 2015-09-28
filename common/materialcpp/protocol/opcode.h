#ifndef opcode_h__
#define opcode_h__
enum opcode
{   
    // guid 申请
    XS2GD_GUID_REQ = 1,
    GD2XS_GUID_ACK,
	
    // data 申请
	XS2DS_ENTITY_REQ,
	DS2XS_ENTITY_ACK,

    // data server 内部使用
    // 从 db load data
    DS2DS_ENTITY_LOAD_REQ,
    DS2DS_ENTITY_LOAD_ACK,

    // 序列化 data 进 sqlite
    DS2DS_ENTITY_SERIALIZE_REQ,
};


#endif // opcode_h__
