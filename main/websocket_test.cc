
#include "protocol.h"
#include "gtest/gtest.h"

using namespace protocol;

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

    std::vector<int> sizes = {0, 100, 254, 65533, 655330};

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
            std::cout << "buffer_size: " << buffer.data_size() << std::endl;
            ASSERT_EQ(count, content1.data_size());
            if (content1 != buffer) {
                int k = 0;
            }
            ASSERT_EQ(content1, buffer);
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