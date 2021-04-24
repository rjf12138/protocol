#include "buffer/byte_buffer.h"
namespace my_utils {

// base64 编码和解码
int encode_base64(ByteBuffer &inbuf, ByteBuffer &outbuf);
int decode_base64(ByteBuffer &inbuf, ByteBuffer &outbuf);

}