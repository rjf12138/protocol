#include "json/wejson.h"
#include "msg_record.h"
#include "system_utils.h"

using namespace my_utils;
using namespace system_utils;


int main(void)
{
   //LOG_GLOBAL_DEBUG("Start");
    ByteBuffer data;
    Stream stream, outstream;

    outstream.open("./config/data_dwjz_format.txt");
    stream.open("./config/data_dwjz.txt");
    stream.read(data, 8192000);

    WeJson json(data);
    data.clear();
    data.write_string(json.format_json());
    outstream.write(data, data.data_size());

    return 0;
}