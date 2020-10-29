#include "wejson.h"
#include "gtest/gtest.h"

using namespace my_util;

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
    std::cout << "test_number: " << val << std::endl;
    fprintf(stdout, "double: %lf, str_double: %s\n", val, str_val.c_str());
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
    try {
        ASSERT_EQ(test_parse_number(0, "03"), true);
    }catch (exception &e) {
        std::cout << "=======================================" << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << "=======================================" << std::endl;
    }
}

TEST_F(WeJson_Test, TYPE_TEST)
{
    
}

}
}
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}