#include "json/wejson.h"
#include "gtest/gtest.h"

using namespace my_utils;

//////////////////////////////////////////////////
char* g_str = NULL;
void release_random_str(void)
{
    if (g_str != NULL) {
        delete[] g_str;
        g_str = NULL;
    }
}

char* random_str(int setlen, int& outlen)
{
    release_random_str();

    int length = rand() % setlen;
    g_str = new char[length];

    for (int i = 0; i < length - 1; ++i) {
        char ch = rand() % 256;
        g_str[i] = ch;
        if (g_str[i] == 0) {
            g_str[i] += 1;
        }
        if (g_str[i] == '"') {
            if (i < length - 1) {
                g_str[i++] = '\\';
                g_str[i] = '"';
            }
            else {
                g_str[i] = '0';
            }
        }
    }
    outlen = length;

    return g_str;
}

//////////////////////////////////////////////////

namespace my {
namespace project {
namespace {

#define MUTITHREAD 1

class WeJson_Test : public ::testing::Test {
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

bool test_number(double val, string str_val)
{
//    fprintf(stdout, "double: %lf, str_double: %s\n", val, str_val.c_str());
    JsonNumber json_number(val);
    ostringstream ostr;
    ostr << json_number;
    if (stod(ostr.str().c_str()) == stod(str_val.c_str())) {
        return true;
    }
    fprintf(stdout, "double: %lf, str_double: %s, convert_double: %s\n", val, str_val.c_str(), ostr.str().c_str());
    return false;
}

bool test_parse_number(double val, string str_val)
{
    ByteBuffer buff;
    buff.write_string(str_val, str_val.length());
    JsonNumber json_number;
    auto str_number_begin = buff.begin();
    auto str_number_end = buff.end();
    json_number.parse(str_number_begin, str_number_end);

    if (stod(json_number.generate().c_str()) == stod(str_val.c_str())) {
        return true;
    }

    fprintf(stdout, "double: %lf, str_double: %s, read_double: %s\n", val, str_val.c_str(), json_number.generate().c_str());
    return false;
}

TEST_F(WeJson_Test, NUMBER_TEST)
{
    JsonNumber json_number_test_1(1.1214);
    JsonNumber json_number_test_2(12);
    JsonNumber json_number_test_3(json_number_test_1);
    JsonNumber json_number_test_4(json_number_test_2);

    ASSERT_EQ(json_number_test_1.generate(), "1.1214");
    ASSERT_EQ(json_number_test_2.generate(), "12");
    ASSERT_EQ(json_number_test_1.generate(), json_number_test_3.generate());
    ASSERT_EQ(json_number_test_2.generate(), json_number_test_4.generate());

    ASSERT_EQ(test_number(-10000.001, "-10000.001"), true);
    ASSERT_EQ(test_number(-10000, "-10000"), true);
    ASSERT_EQ(test_number(-10000.0000, "-10000"), true);
    ASSERT_EQ(test_number(10000.0000, "10000"), true);
    ASSERT_EQ(test_number(10000.0000, "10000"), true);
    ASSERT_EQ(test_number(10000.00001, "10000.00001"), true);
    ASSERT_EQ(test_number(0.00001, "0.00001"), true);
    ASSERT_EQ(test_number(0, "0"), true);
    
    ASSERT_EQ(test_parse_number(-10000.001, "-10000.001"), true);
    ASSERT_EQ(test_parse_number(-10000, "-10000"), true);
    ASSERT_EQ(test_parse_number(-10000.0000, "-10000"), true);
    ASSERT_EQ(test_parse_number(10000.0000, "10000"), true);
    ASSERT_EQ(test_parse_number(10000.0000, "10000"), true);
    ASSERT_EQ(test_parse_number(10000.00001, "10000.00001"), true);
    ASSERT_EQ(test_parse_number(10000.0000, "+10000"), true);
    ASSERT_EQ(test_parse_number(10000.00001, "+10000.00001"), true);
    ASSERT_EQ(test_parse_number(0.00001, "0.00001"), true);
    ASSERT_EQ(test_parse_number(0.00001, "+0.00001"), true);
    ASSERT_EQ(test_parse_number(-0.00001, "-0.00001"), true);

    ASSERT_EQ(test_number(-0., "0"), true);
    ASSERT_EQ(test_number(+0000., "0"), true);
    ASSERT_EQ(test_number(+0000.00000000, "0"), true);

    // double 小数点后默认保留6位，最后一位会以第七位进一
    ASSERT_EQ(test_number(-0.123456789, "-0.123457"), true);
    ASSERT_EQ(test_number(+000.123456778, "+0.123457"), true);
    ASSERT_EQ(test_number(+0000.1234567890000, "+0.123457"), true);

    ASSERT_EQ(test_number(-0.123456789, "-0.123457"), true);
    try {
        ASSERT_EQ(test_parse_number(0, "03"), true);
    }catch (exception &e) {
        std::cout << "=======================================" << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << "=======================================" << std::endl;
    }
}

TEST_F(WeJson_Test, STRING_TEST)
{
    int len;
    for (int i = 0;i < 9000; ++i) {
        char *ptr = random_str(16, len);
        string str = ptr;
        // 测试 string 和 const char* 构造函数是否一致
        JsonString js_str_1(str);
        JsonString js_str_2(ptr);
        ASSERT_EQ(js_str_1.generate(), js_str_2.generate());

        // 测试字符串解析， json解析的是双引号内的字符串，所以要将原来的
        // 字符串加上双引号
        ByteBuffer buff;
        JsonString js_str_3;

        str = '"';
        str +=  ptr;
        str += '"';
        LOG_GLOBAL_DEBUG("Len: %d", len);
        buff.write_string(str);
        ByteBuffer_Iterator bbegin = buff.begin();
        ByteBuffer_Iterator bend = buff.end();
        if (len == 13 && i == 997) {
            std::cout << i << std::endl;
        }
        js_str_3.parse(bbegin, bend);

        if (js_str_1.generate() != js_str_3.generate()) {
            std::cout << "" << std::endl;
        }
        ASSERT_EQ(js_str_1.generate(), js_str_3.generate());
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