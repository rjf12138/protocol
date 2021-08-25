
#include "protocol.h"
#include "gtest/gtest.h"

using namespace ptl;

namespace my {
namespace project {
namespace {

class WebsocketPtl_Test : public ::testing::Test {
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

TEST_F(WebsocketPtl_Test, Parser_Test)
{
    basic::ByteBuffer buffer, output_ptl;
    WebsocketPtl websck_ptl;

    std::vector<int> sizes = {0, 100, 254, 65533, 6553300};

    for (std::size_t count = 0; count < sizes.size(); ++count) {
        buffer.clear();
        for (int i = 0; i < count; ++i) {
            buffer.write_int8((rand() % 26) + 'a');
        }

        for (int i = WEBSOCKET_OPCODE_CONTINUATION_FRAME; i <= WEBSOCKET_OPCODE_RESERVED_FOR_CONTROL_FRAMES; ++i) {
            // 不加掩码
            websck_ptl.generate(output_ptl, buffer, i, false);
            
            websck_ptl.clear();
            
            websck_ptl.parse(output_ptl);
            ASSERT_EQ(i, websck_ptl.get_opcode());

            basic::ByteBuffer &content = websck_ptl.get_content();
            
            ASSERT_EQ(count, content.data_size());
            ASSERT_EQ(content, buffer);

            // 添加掩码
            websck_ptl.generate(output_ptl, buffer, i, true);
            
            websck_ptl.clear();
            
            websck_ptl.parse(output_ptl);
            ASSERT_EQ(i, websck_ptl.get_opcode());

            basic::ByteBuffer &content1 = websck_ptl.get_content();
            ASSERT_EQ(count, content1.data_size());
            if (content1 != buffer) {
                int k = 0;
            }
            ASSERT_EQ(content1, buffer);
        }
    }
}

TEST_F(WebsocketPtl_Test, Loop_test)
{
    basic::ByteBuffer buffer, output_ptl, msg_buffer;
    WebsocketPtl websck_ptl;

    std::vector<int> sizes = {0, 1, 50, 100, 254, 65533, 6553300};
    std::vector<char> content = {'z', 'a', 'b','c','d','e','f'};

    for (int s = 0; s < 10; ++s) {
         std::cout << "s: " << s << std::endl;
        for (int k = 0; k < 50; ++k) {
            std::cout << "k-generate: " << k << std::endl;
            for (std::size_t j = 0; j < sizes.size(); ++j) {
                buffer.clear();
                for (int i = 0; i < sizes[j]; ++i) {
                    buffer.write_int8(content[j]);
                }

                websck_ptl.generate(output_ptl, buffer, WEBSOCKET_OPCODE_TEXT_FRAME, false);
                msg_buffer += output_ptl;
            }
        }


        for (int k = 0; k < 50; ++k) {
            std::cout << "k-parse: " << k << std::endl;
            for (int i = 0; i < sizes.size(); ++i) {
                WebsocketParse_ErrorCode errcode = websck_ptl.parse(msg_buffer);
                ASSERT_EQ(errcode, WebsocketParse_OK);
                
                ASSERT_EQ(websck_ptl.get_content().data_size(), sizes[i]);
                for (int j = 0; j < websck_ptl.get_content().data_size(); ++j) {
                    ASSERT_EQ(websck_ptl.get_content()[j], content[i]);
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