#ifndef __PROTOCOL__
#define __PROTOCOL__

#include "basic_head.h"
#include "msg_record.h"
#include "buffer/byte_buffer.h"

namespace my_utils {

enum PaserResult {
    ProtocolParser_Failed = -1,
    ProtocolParse_OK = 1,
};

class ProtocolParser : public MsgRecord {
public:
    ProtocolParser(void);
    ~ProtocolParser(void);

    int parser(ByteBuffer &data);
    int generate(void);
private:
    ByteBuffer data_buffer_;
};

};

#endif