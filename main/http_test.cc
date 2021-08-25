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
Content-Length: 11\r\n\
Etag: \"45bae1-16a-46d776ac\"\r\n\r\n\
Hello, Http!";

TEST_F(Http_Test, Basic_Test)
{
    std::vector<int>    head_code = {HTTP_STATUS_OK, HTTP_STATUS_NoContent, HTTP_STATUS_Found, HTTP_STATUS_NotFound};
    std::vector<std::string> head_phrase = {"OK", "NoContent", "Found", "NotFound"};
    std::vector<std::string> head_method = {HTTP_METHOD_GET, HTTP_METHOD_POST, HTTP_METHOD_PUT, HTTP_METHOD_DELETE, HTTP_METHOD_HEAD, HTTP_METHOD_OPTION};
    std::vector<std::string> head_option = {HTTP_HEADER_ContentEncoding, HTTP_HEADER_ContentLanguage, HTTP_HEADER_Allow, HTTP_HEADER_Host, HTTP_HEADER_IfNoneMatch};
    std::vector<std::string> head_value = {"rjfzip", "zh_CN", "allow", "RjfWebServer;kkk", "!@#$%^&*()_+"};
    std::vector<std::string> head_url = {"/file/index.html"};

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
        ASSERT_EQ(ptl2.parser(ptl_stream), HttpParse_OK);

        basic::ByteBuffer data;
        ptl2.get_content(data);
        ASSERT_EQ(data, content);
        ASSERT_EQ(ptl2.get_method(), head_method[j]);
        for (std::size_t i = 0; i < head_option.size(); ++i) {
            ASSERT_EQ(ptl2.get_header_option(head_option[i]), head_value[i]);
        }

        // 每次解析后，解析的数据会被清空，需要重新设置
        ptl1.set_request(head_method[j], head_url[0]);
        for (std::size_t i = 0; i < head_option.size(); ++i) {
            ptl1.set_header_option(head_option[i], head_value[i]);
        }

        ptl1.set_header_option(HTTP_HEADER_ContentLength, std::to_string(content.data_size()));
        ptl1.set_content(content);

        ptl_stream.clear();
        ptl1.generate(ptl_stream);

        auto begin_iter = ptl_stream.begin();
        ptl_stream.insert_front(begin_iter, basic::ByteBuffer("fjlkakjflsdfjlairuwoerjwnv"));
        ASSERT_EQ(ptl2.parser(ptl_stream), HttpParse_OK);
        
        ptl2.get_content(data);
        ASSERT_EQ(data, content);
        ASSERT_EQ(ptl2.get_method(), head_method[j]);
        for (std::size_t i = 0; i < head_option.size(); ++i) {
            ASSERT_EQ(ptl2.get_header_option(head_option[i]), head_value[i]);
        }
    }

    // 测试http响应
    for (std::size_t j = 0; j < head_code.size(); ++j) {
        ptl1.set_response(head_code[j], head_phrase[0]);
        for (std::size_t i = 0; i < head_option.size(); ++i) {
            ptl1.set_header_option(head_option[i], head_value[i]);
        }

        ptl1.set_header_option(HTTP_HEADER_ContentLength, std::to_string(content.data_size()));
        ptl1.set_content(content);
        ptl1.generate(ptl_stream);

        ASSERT_EQ(ptl2.parser(ptl_stream), HttpParse_OK);

        basic::ByteBuffer data;
        ptl2.get_content(data);
        ASSERT_EQ(data, content);
        ASSERT_EQ(ptl2.get_status_code(), head_code[j]);
        for (std::size_t i = 0; i < head_option.size(); ++i) {
            ASSERT_EQ(ptl2.get_header_option(head_option[i]), head_value[i]);
        }

        // 每次解析后，解析的数据会被清空，需要重新设置
        ptl1.set_response(head_code[j], head_phrase[0]);
        for (std::size_t i = 0; i < head_option.size(); ++i) {
            ptl1.set_header_option(head_option[i], head_value[i]);
        }

        ptl1.set_header_option(HTTP_HEADER_ContentLength, std::to_string(content.data_size()));
        ptl1.set_content(content);
        ptl1.generate(ptl_stream);

        auto begin_iter = ptl_stream.begin();
        ptl_stream.insert_front(begin_iter, basic::ByteBuffer("fjlkakjflsdfjlairuwoerjwnv"));
        ASSERT_EQ(ptl2.parser(ptl_stream), HttpParse_OK);
        
        ptl2.get_content(data);
        ASSERT_EQ(data, content);
        ASSERT_EQ(ptl2.get_status_code(), head_code[j]);
        for (std::size_t i = 0; i < head_option.size(); ++i) {
            ASSERT_EQ(ptl2.get_header_option(head_option[i]), head_value[i]);
        }
    }
}

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
    int count_1 = 10, count_2 = 90;
    for (int k = 0; k < count_1; ++k) {
        time_t start = time(NULL);
        for (int s = 0; s < count_2; ++s) {
            for (std::size_t j = 0; j < head_method.size(); ++j) {
                ptl1.set_request(head_method[j], head_url[0]);
                for (std::size_t i = 0; i < head_option.size(); ++i) {
                    ptl1.set_header_option(head_option[i], head_value[i]);
                }

                ptl1.set_header_option(HTTP_HEADER_ContentLength, std::to_string(content.data_size()));
                ptl1.set_content(content);

                basic::ByteBuffer tmp_buf;
                ptl1.generate(tmp_buf);
                ptl_stream += tmp_buf;
            }
        }

        for (int s = 0; s < count_2; ++s) {
            for (std::size_t j = 0; j < head_method.size(); ++j) {
                ASSERT_EQ(ptl2.parser(ptl_stream), HttpParse_OK);

                basic::ByteBuffer data;
                ptl2.get_content(data);
                ASSERT_EQ(data, content);
                ASSERT_EQ(ptl2.get_method(), head_method[j]);
                for (std::size_t i = 0; i < head_option.size(); ++i) {
                    ASSERT_EQ(ptl2.get_header_option(head_option[i]), head_value[i]);
                }
            }
        }
        for (int s = 0; s < count_2; ++s) {
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
            }
        }

        for (int s = 0; s < count_2; ++s) {
            for (std::size_t j = 0; j < head_code.size(); ++j) {
                ASSERT_EQ(ptl2.parser(ptl_stream), HttpParse_OK);

                basic::ByteBuffer data;
                ptl2.get_content(data);
                ASSERT_EQ(data, content);
                ASSERT_EQ(ptl2.get_status_code(), head_code[j]);
                for (std::size_t i = 0; i < head_option.size(); ++i) {
                    ASSERT_EQ(ptl2.get_header_option(head_option[i]), head_value[i]);
                }
            }
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