#include "protocol.h"
#include "gtest/gtest.h"

using namespace ptl;

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
Content-Length: 12\r\n\
Etag: \"45bae1-16a-46d776ac\"\r\n\r\n\
Hello, Http!";

std::string tranfer_encode = 
"HTTP/1.1 200 OK\r\n\
Date: Thu, 07 Jun 2012 07:21:36 GMT\r\n\
Server: Apache\r\n\
Connection: close\r\n\
Tranfer-Encoding: chunked\r\n\
Etag: \"45bae1-16a-46d776ac\"\r\n\r\n";

TEST_F(Http_Test, Basic_Test)
{
    std::vector<int>    head_code = {HTTP_STATUS_OK, HTTP_STATUS_NoContent, HTTP_STATUS_Found, HTTP_STATUS_NotFound};
    std::vector<std::string> head_phrase = {"OK", "No Content", "Found", "Not Found"};
    std::vector<std::string> head_method = {HTTP_METHOD_GET, HTTP_METHOD_POST, HTTP_METHOD_PUT, HTTP_METHOD_DELETE, HTTP_METHOD_HEAD, HTTP_METHOD_OPTION};
    std::vector<std::string> head_option = {HTTP_HEADER_ContentEncoding, HTTP_HEADER_ContentLanguage, HTTP_HEADER_Allow, HTTP_HEADER_Host, HTTP_HEADER_IfNoneMatch};
    std::vector<std::string> head_value = {"rjfzip", "zh_CN", "allow", "RjfWebServer;kkk", "!@#$%^&*()_+"};
    std::vector<std::string> head_url = {"/file/index.html", "/", "/lkdsf?fjksld&&sdjkfjls"};

    HttpPtl ptl1, ptl2;
    basic::ByteBuffer content("Hello, Http!"), ptl_stream;

    // 测试http请求
    ASSERT_EQ(head_value.size(), head_option.size());
    for (std::size_t j = 0; j < head_method.size(); ++j) {
        ptl1.set_request(head_method[j], head_url[0]);
        for (std::size_t i = 0; i < head_option.size(); ++i) {
            ptl1.set_header_option(head_option[i], head_value[i]);
        }

        ptl1.set_header_option(HTTP_HEADER_ContentLength, std::to_string(content.data_size()));
        ptl1.set_content(content);

        ptl_stream.clear();
        ptl1.generate(ptl_stream);
        ASSERT_EQ(ptl2.parse(ptl_stream), HttpParse_OK);

        ASSERT_EQ(ptl2.get_phrase(), HTTP_VERSION);
        ASSERT_EQ(ptl2.get_url(), head_url[0]);
        ASSERT_EQ(ptl2.get_content(), content);
        ASSERT_EQ(ptl2.get_method(), head_method[j]);
        for (std::size_t i = 0; i < head_option.size(); ++i) {
            ASSERT_EQ(ptl2.get_header_option(head_option[i]), head_value[i]);
        }

        ptl1.clear();
        ptl2.clear();
    }

    // 测试http响应
    for (std::size_t j = 0; j < head_code.size(); ++j) {
        ptl1.set_response(head_code[j], head_phrase[j % 2]);
        for (std::size_t i = 0; i < head_option.size(); ++i) {
            ptl1.set_header_option(head_option[i], head_value[i]);
        }

        ptl1.set_header_option(HTTP_HEADER_ContentLength, std::to_string(content.data_size()));
        ptl1.set_content(content);
        ptl1.generate(ptl_stream);

        ASSERT_EQ(ptl2.parse(ptl_stream), HttpParse_OK);

        ASSERT_EQ(ptl2.get_content(), content);
        ASSERT_EQ(ptl2.get_status_code(), head_code[j]);
        ASSERT_EQ(ptl2.get_phrase(), head_phrase[j % 2]);
        for (std::size_t i = 0; i < head_option.size(); ++i) {
            ASSERT_EQ(ptl2.get_header_option(head_option[i]), head_value[i]);
        }
        ptl1.clear();
        ptl2.clear();
    }

    // 测试特殊HTTP请求
    ptl1.clear();
    basic::ByteBuffer buff(std::string("GET / HTTP/1.1\r\nContent-Length:0\r\n\r\n"));
    ptl1.parse(buff);
    ASSERT_EQ(ptl1.get_method(), "GET");
    ASSERT_EQ(ptl1.get_url(), "/");
    ASSERT_EQ(ptl2.get_content().data_size(), 0);

    // 测试特殊HTTP回复
    ptl1.clear();
    buff.write_string(std::string("HTTP/1.1 404 OK\r\nContent-Length:0\r\n\r\n"));
    ptl1.parse(buff);
    ASSERT_EQ(ptl1.get_status_code(), 404);
    ASSERT_EQ(ptl1.get_content().data_size(), 0);
}

// 测试缓存中存在多条HTTP消息，然后逐条消息进行解析
TEST_F(Http_Test, Loop_Test)
{
    std::vector<int>    head_code = {HTTP_STATUS_OK, HTTP_STATUS_NoContent, HTTP_STATUS_Found, HTTP_STATUS_NotFound};
    std::vector<std::string> head_phrase = {"OK", "NoContent", "Found", "NotFound"};
    std::vector<std::string> head_method = {HTTP_METHOD_GET, HTTP_METHOD_POST, HTTP_METHOD_PUT, HTTP_METHOD_DELETE, HTTP_METHOD_HEAD, HTTP_METHOD_OPTION};
    std::vector<std::string> head_option = {HTTP_HEADER_ContentEncoding, HTTP_HEADER_ContentLanguage, HTTP_HEADER_Allow, HTTP_HEADER_Host, HTTP_HEADER_IfNoneMatch};
    std::vector<std::string> head_value = {"rjfzip", "zh_CN", "allow", "RjfWebServer;kkk", "!@#$%^&*()_+"};
    std::vector<std::string> head_url = {"/file/index.html"};

    HttpPtl ptl1, ptl2;
    basic::ByteBuffer content("Hello, world!"), ptl_stream;
    int count_1 = 5, count_2 = 300;
    for (int k = 0; k < count_1; ++k) {
        for (int s = 0; s < count_2; ++s) {  // 设置多个http请求消息
            for (std::size_t j = 0; j < head_method.size(); ++j) {
                ptl1.set_request(head_method[j], head_url[0]);
                for (std::size_t i = 0; i < head_option.size(); ++i) {
                    ptl1.set_header_option(head_option[i], head_value[i]);
                }
                ptl1.set_content(content);

                basic::ByteBuffer tmp_buf;
                ptl1.generate(tmp_buf);
                ptl_stream += tmp_buf;
                ptl1.clear();
            }
        }

        for (int s = 0; s < count_2; ++s) { // 解析多个http请求消息
            for (std::size_t j = 0; j < head_method.size(); ++j) {
                ASSERT_EQ(ptl2.parse(ptl_stream), HttpParse_OK);

                ASSERT_EQ(ptl2.get_url(), head_url[0]);
                ASSERT_EQ(ptl2.get_content(), content);
                ASSERT_EQ(ptl2.get_method(), head_method[j]);
                for (std::size_t i = 0; i < head_option.size(); ++i) {
                    ASSERT_EQ(ptl2.get_header_option(head_option[i]), head_value[i]);
                }
                ptl2.clear();
            }
        }

        for (int s = 0; s < count_2; ++s) { // 设置多个http回复消息
            for (std::size_t j = 0; j < head_code.size(); ++j) {
                ptl1.set_response(head_code[j], head_phrase[0]);
                for (std::size_t i = 0; i < head_option.size(); ++i) {
                    ptl1.set_header_option(head_option[i], head_value[i]);
                }

                ptl1.set_header_option(HTTP_HEADER_ContentLength, std::to_string(content.data_size()));
                ptl1.set_content(content);

                basic::ByteBuffer tmp_buf;
                ptl1.generate(tmp_buf);
                ptl_stream += tmp_buf;
                ptl1.clear();
            }
        }

        for (int s = 0; s < count_2; ++s) { // 解析多个http回复消息
            for (std::size_t j = 0; j < head_code.size(); ++j) {
                ASSERT_EQ(ptl2.parse(ptl_stream), HttpParse_OK);

                ASSERT_EQ(ptl2.get_content(), content);
                ASSERT_EQ(ptl2.get_status_code(), head_code[j]);
                for (std::size_t i = 0; i < head_option.size(); ++i) {
                    ASSERT_EQ(ptl2.get_header_option(head_option[i]), head_value[i]);
                }
                ptl2.clear();
            }
        }
    }
}

// 数据不够时测试
TEST_F(Http_Test, DataNotEnough_Test)
{
    HttpPtl ptl;
    basic::ByteBuffer buffer;
    for (std::size_t i = 0; i < http_request.length() - 1; ++i) {
        buffer.write_int8(http_request[i]);
        //std::cout << buffer.str() << std::endl;
        ASSERT_EQ(ptl.parse(buffer), HttpParse_ContentNotEnough);
    }
    buffer.write_int8(http_request[http_request.length() - 1]);
    ASSERT_EQ(ptl.parse(buffer), HttpParse_OK);

    for (std::size_t i = 0; i < http_respone.length() - 1; ++i) {
        buffer.write_int8(http_respone[i]);
        //std::cout << buffer.str() << std::endl;
        ASSERT_EQ(ptl.parse(buffer), HttpParse_ContentNotEnough);
    }
    buffer.write_int8(http_respone[http_respone.length() - 1]);
    ASSERT_EQ(ptl.parse(buffer), HttpParse_OK);
}

basic::ByteBuffer get_chunked_data(basic::ByteBuffer data, int num)
{
    basic::ByteBuffer data_buffer;
    if (data.data_size() > 0) {
        for (int i = 0; i < num; ++i) {
            char buffer[32] = {0};
            snprintf(buffer, 32, "%lx", data.data_size());
            data_buffer.write_bytes(buffer, strlen(buffer));
            data_buffer.write_bytes("\r\n", 2);
            data_buffer += data;
            data_buffer.write_bytes("\r\n", 2);
        }
    }
    data_buffer.write_bytes("0\r\n\r\n",5);
    return data_buffer;
}

// 测试 Tranfer-encode 
TEST_F(Http_Test, TranferEncodeSingleTest)
{
    HttpPtl http_ptl;
    basic::ByteBuffer buffer;
    std::string data_str;

    // 单个块解析
    for (int i = 0; i < 100; ++i) {
        buffer += tranfer_encode;
        buffer += get_chunked_data(data_str, 1);

        http_ptl.parse(buffer);
        std::vector<basic::ByteBuffer> &datas_refer = http_ptl.get_tranfer_encode_datas();
        if (i == 0) {
            ASSERT_EQ(datas_refer.size(), 0);
        } else {
            ASSERT_EQ(datas_refer.size(), 1);
            ASSERT_EQ(datas_refer[0], data_str);
        }
        datas_refer.clear();
        buffer.clear();
        data_str += 'a';
    }
}

// 测试 Tranfer-encode 
TEST_F(Http_Test, TranferEncodeMutilTest)
{
    HttpPtl http_ptl;
    basic::ByteBuffer buffer;
    std::string data_str;

    // 单个块解析
    for (int j = 0; j < 256; ++j) { // 对每个字符进行测试
        for (int i = 0; i < 20; ++i) {
            buffer += tranfer_encode;
            buffer += get_chunked_data(data_str, i);
            http_ptl.parse(buffer);
            std::vector<basic::ByteBuffer> &datas_refer = http_ptl.get_tranfer_encode_datas();
            ASSERT_EQ(datas_refer.size(), i);
            for (int k = 0; k < i; ++k) {
                ASSERT_EQ(datas_refer[k], data_str);
            }
            
            datas_refer.clear();
            buffer.clear();
            data_str += j;
        }
    }
}

// 测试数据不够时情况
TEST_F(Http_Test, TranferEncodeDataNotEnough_Test)
{
    HttpPtl http_ptl;
    basic::ByteBuffer buffer;
    basic::ByteBuffer data_str;

    // 单个块解析
    for (int j = 0; j < 256; ++j) { // 对每个字符进行测试
        std::cout << "j: " << j << std::endl;
        for (int i = 0; i < 20; ++i) {
            std::cout << "i: " << i << std::endl;
            buffer += tranfer_encode;
            buffer += get_chunked_data(data_str, i);

            basic::ByteBuffer tmp_buffer;
            for (std::size_t s = 0; s < buffer.data_size() - 1; ++s) {
                std::cout << "data_size: " << buffer.data_size() << " s: " << s << " value: " << buffer[s] << std::endl;
                tmp_buffer.write_int8(buffer[s]);
                ASSERT_EQ(http_ptl.parse(tmp_buffer), HttpParse_ContentNotEnough);
            }
            tmp_buffer.write_int8(buffer[buffer.data_size() - 1]);
            ASSERT_EQ(http_ptl.parse(tmp_buffer), HttpParse_OK);

            std::vector<basic::ByteBuffer> &datas_refer = http_ptl.get_tranfer_encode_datas();
            ASSERT_EQ(datas_refer.size(), i);
            for (int k = 0; k < i; ++k) {
                ASSERT_EQ(datas_refer[k], data_str);
            }
            
            datas_refer.clear();
            buffer.clear();
            data_str += j;
        }
    }
}

}
}
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}