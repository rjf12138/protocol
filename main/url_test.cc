#include "protocol.h"
#include "gtest/gtest.h"

using namespace ptl;

namespace my {
namespace project {
namespace {

class URL_Test : public ::testing::Test {
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

TEST_F(URL_Test, BasicUrlTest)
{
    URLParser paser;
    // 目前只支持全英文
    // 测试 raw 地址不携带任何参数
    ASSERT_EQ(paser.parser("raw://127.0.0.1:12138"), ParserError_Ok);
    ASSERT_EQ(paser.type_, ProtocolType_Raw);
    ASSERT_EQ(paser.addr_, "127.0.0.1");
    ASSERT_EQ(paser.port_, 12138);

    // 测试 http 地址解析，不携带任何参数, 不带端口默认是 80
    ASSERT_EQ(paser.parser("http://127.0.0.1"), ParserError_Ok);
    ASSERT_EQ(paser.type_, ProtocolType_Http);
    ASSERT_EQ(paser.addr_, "127.0.0.1");
    ASSERT_EQ(paser.port_, 80);
    ASSERT_EQ(paser.res_path_, "/");
    ASSERT_EQ(paser.param_.size(), 0);

    // 测试 http 地址解析，添加反斜杠(/), 不带端口默认是 80
    ASSERT_EQ(paser.parser("http://127.0.0.1/"), ParserError_Ok);
    ASSERT_EQ(paser.type_, ProtocolType_Http);
    ASSERT_EQ(paser.addr_, "127.0.0.1");
    ASSERT_EQ(paser.port_, 80);
    ASSERT_EQ(paser.res_path_, "/");
    ASSERT_EQ(paser.param_.size(), 0);

    // 测试 http 地址解析，添加端口：8088
    ASSERT_EQ(paser.parser("http://127.0.0.1:8088"), ParserError_Ok);
    ASSERT_EQ(paser.type_, ProtocolType_Http);
    ASSERT_EQ(paser.addr_, "127.0.0.1");
    ASSERT_EQ(paser.port_, 8088);
    ASSERT_EQ(paser.res_path_, "/");
    ASSERT_EQ(paser.param_.size(), 0);

    // 测试 http 地址解析，添加端口：8088， 添加反斜杠(/)
    ASSERT_EQ(paser.parser("http://127.0.0.1:8088/"), ParserError_Ok);
    ASSERT_EQ(paser.type_, ProtocolType_Http);
    ASSERT_EQ(paser.addr_, "127.0.0.1");
    ASSERT_EQ(paser.port_, 8088);
    ASSERT_EQ(paser.res_path_, "/");
    ASSERT_EQ(paser.param_.size(), 0);

    // 测试 http 地址解析，添加端口：8088， 添加资源地址
    ASSERT_EQ(paser.parser("http://127.0.0.1:8088/index.html"), ParserError_Ok);
    ASSERT_EQ(paser.type_, ProtocolType_Http);
    ASSERT_EQ(paser.addr_, "127.0.0.1");
    ASSERT_EQ(paser.port_, 8088);
    ASSERT_EQ(paser.res_path_, "/index.html");
    ASSERT_EQ(paser.param_.size(), 0);

    // 测试 http 地址解析，添加端口：8088， 添加资源地址
    ASSERT_EQ(paser.parser("http://127.0.0.1:8088/index.html?key1=value1"), ParserError_Ok);
    ASSERT_EQ(paser.type_, ProtocolType_Http);
    ASSERT_EQ(paser.addr_, "127.0.0.1");
    ASSERT_EQ(paser.port_, 8088);
    ASSERT_EQ(paser.res_path_, "/index.html");
    ASSERT_EQ(paser.param_.size(), 1);
    ASSERT_EQ(paser.param_["key1"], "value1");

    // 测试 http 地址解析，添加端口：8088， 添加资源地址
    ASSERT_EQ(paser.parser("http://127.0.0.1:8088/index.html?key1=value1&key2=value2"), ParserError_Ok);
    ASSERT_EQ(paser.type_, ProtocolType_Http);
    ASSERT_EQ(paser.addr_, "127.0.0.1");
    ASSERT_EQ(paser.port_, 8088);
    ASSERT_EQ(paser.res_path_, "/index.html");
    ASSERT_EQ(paser.param_.size(), 2);
    ASSERT_EQ(paser.param_["key1"], "value1");
    ASSERT_EQ(paser.param_["key2"], "value2");


    ////////////////////////// 测试 websocket 地址 //////////////////////////////////////////////////
    // 测试 ws 地址解析，不携带任何参数, 不带端口默认是 80
    ASSERT_EQ(paser.parser("ws://127.0.0.1"), ParserError_Ok);
    ASSERT_EQ(paser.type_, ProtocolType_Websocket);
    ASSERT_EQ(paser.addr_, "127.0.0.1");
    ASSERT_EQ(paser.port_, 80);
    ASSERT_EQ(paser.res_path_, "/");
    ASSERT_EQ(paser.param_.size(), 0);

    // 测试 ws 地址解析，添加反斜杠(/), 不带端口默认是 80
    ASSERT_EQ(paser.parser("ws://127.0.0.1/"), ParserError_Ok);
    ASSERT_EQ(paser.type_, ProtocolType_Websocket);
    ASSERT_EQ(paser.addr_, "127.0.0.1");
    ASSERT_EQ(paser.port_, 80);
    ASSERT_EQ(paser.res_path_, "/");
    ASSERT_EQ(paser.param_.size(), 0);

    // 测试 ws 地址解析，添加端口：8088
    ASSERT_EQ(paser.parser("ws://127.0.0.1:8088"), ParserError_Ok);
    ASSERT_EQ(paser.type_, ProtocolType_Websocket);
    ASSERT_EQ(paser.addr_, "127.0.0.1");
    ASSERT_EQ(paser.port_, 8088);
    ASSERT_EQ(paser.res_path_, "/");
    ASSERT_EQ(paser.param_.size(), 0);

    // 测试 ws 地址解析，添加端口：8088， 添加反斜杠(/)
    ASSERT_EQ(paser.parser("ws://127.0.0.1:8088/"), ParserError_Ok);
    ASSERT_EQ(paser.type_, ProtocolType_Websocket);
    ASSERT_EQ(paser.addr_, "127.0.0.1");
    ASSERT_EQ(paser.port_, 8088);
    ASSERT_EQ(paser.res_path_, "/");
    ASSERT_EQ(paser.param_.size(), 0);

    // 测试 ws 地址解析，添加端口：8088， 添加资源地址
    ASSERT_EQ(paser.parser("ws://127.0.0.1:8088/index.html"), ParserError_Ok);
    ASSERT_EQ(paser.type_, ProtocolType_Websocket);
    ASSERT_EQ(paser.addr_, "127.0.0.1");
    ASSERT_EQ(paser.port_, 8088);
    ASSERT_EQ(paser.res_path_, "/index.html");
    ASSERT_EQ(paser.param_.size(), 0);

    // 测试 ws 地址解析，添加端口：8088， 添加资源地址
    ASSERT_EQ(paser.parser("ws://127.0.0.1:8088/index.html?key1=value1"), ParserError_Ok);
    ASSERT_EQ(paser.type_, ProtocolType_Websocket);
    ASSERT_EQ(paser.addr_, "127.0.0.1");
    ASSERT_EQ(paser.port_, 8088);
    ASSERT_EQ(paser.res_path_, "/index.html");
    ASSERT_EQ(paser.param_.size(), 1);
    ASSERT_EQ(paser.param_["key1"], "value1");

    // 测试 ws 地址解析，添加端口：8088， 添加资源地址
    ASSERT_EQ(paser.parser("ws://127.0.0.1:8088/index.html?key1=value1&key2=value2"), ParserError_Ok);
    ASSERT_EQ(paser.type_, ProtocolType_Websocket);
    ASSERT_EQ(paser.addr_, "127.0.0.1");
    ASSERT_EQ(paser.port_, 8088);
    ASSERT_EQ(paser.res_path_, "/index.html");
    ASSERT_EQ(paser.param_.size(), 2);
    ASSERT_EQ(paser.param_["key1"], "value1");
    ASSERT_EQ(paser.param_["key2"], "value2");
}

}
}
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
