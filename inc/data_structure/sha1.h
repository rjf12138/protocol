#ifndef _SHA1_H_
#define _SHA1_H_

#include <stdint.h>
#include "buffer/byte_buffer.h"

namespace my_utils {
extern int sha1(my_utils::ByteBuffer &inbuf, my_utils::ByteBuffer &outbuf);

}
#endif