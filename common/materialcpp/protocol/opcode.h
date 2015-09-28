#ifndef opcode_h__
#define opcode_h__
enum opcode
{   
    // guid ����
    XS2GD_GUID_REQ = 1,
    GD2XS_GUID_ACK,
	
    // data ����
	XS2DS_ENTITY_REQ,
	DS2XS_ENTITY_ACK,

    // data server �ڲ�ʹ��
    // �� db load data
    DS2DS_ENTITY_LOAD_REQ,
    DS2DS_ENTITY_LOAD_ACK,

    // ���л� data �� sqlite
    DS2DS_ENTITY_SERIALIZE_REQ,
};


#endif // opcode_h__
