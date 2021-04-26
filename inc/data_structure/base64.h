#include "buffer/byte_buffer.h"
namespace my_utils {

// base64 编码和解码
extern int encode_base64(ByteBuffer &inbuf, ByteBuffer &outbuf);
extern int decode_base64(ByteBuffer &inbuf, ByteBuffer &outbuf);

}