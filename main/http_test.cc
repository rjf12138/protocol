#include "protocol.h"
#include "gtest/gtest.h"

using namespace my_protocol;

namespace my {
namespace project {
namespace {


class Http_Test : public ::testing::Test {
protected:
    void SetUp() override {
        // Code here will be called immediately after the constructor (right
        // before each test).
    }

    void TearDown() override {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }
};

std::string http_request = 
"GET / HTTP/1.1\r\n\
Host: hackr.jp\r\n\
User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:13.0) Gecko/2010010\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*; q=0\r\n\
Accept-Language: ja,en-us;q=0.7,en;q=0.3\r\n\
Accept-Encoding: gzip, deflate\r\n\
DNT: 1\r\n\
Connection: keep-alive\r\n\
If-Modified-Since: Fri, 31 Aug 2007 02:02:20 GMT\r\n\
If-None-Match: \"45bae1-16a-46d776ac\"\r\n\
Content-Length: 12\r\n\
Cache-Control: max-age=0\r\n\r\n\
Hello, Http!";

std::string http_respone = 
"HTTP/1.1 304 Not Modified\r\n\
Date: Thu, 07 Jun 2012 07:21:36 GMT\r\n\
Server: Apache\r\n\
Connection: close\r\n\
Content-Length: 11\r\n\
Etag: \"45bae1-16a-46d776ac\"\r\n\r\n\
Hello, Http!";

TEST_F(Http_Test, Basic_Test)
{
    HttpPtl http;
    ProtocolParser ptl;
    ptl.get_protocol_buff().write_string(http_respone);
    ptl.get_http_packet(http);

    string str;
    ByteBuffer out;
    http.generate(out);
    out.read_string(str);

    cout << endl << str << std::endl;
}

}
}
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}