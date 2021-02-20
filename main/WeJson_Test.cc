#include "json/wejson.h"
#include "gtest/gtest.h"

using namespace my_utils;
//////////////////////////////////////////////////
void random_str(int setlen, ByteBuffer &buff1, ByteBuffer &buff2)
{
    buff1.clear();
    buff2.clear();

    int length = rand() % setlen +1;
    buff1.write_int8('"');

    for (int i = 0; i < length - 1; ++i) {
        int8_t ch = rand() % 256;
        
        if (ch == 0) {
            buff1.write_int8('0');
            buff2.write_int8('0');
        } else if (ch == '"') {
            buff1.write_int8('\\');
            buff2.write_int8('\\');
            buff1.write_int8('"');
            buff2.write_int8('"');
        } else if (ch == '\\') {
            buff1.write_int8('\\');
            buff2.write_int8('\\');
            buff1.write_int8('\\');
            buff2.write_int8('\\');
        } else {
            buff1.write_int8(ch);
            buff2.write_int8(ch);
        }
    }
    buff1.write_int8('"');

    return ;
}

bool str_compare(ByteBuffer &buff1, ByteBuffer &buff2)
{
    string str;
    buff2.read_string(str);
    JsonString js_str_t1(str);
    bool ret = (str.length() == js_str_t1.generate().length());
    if (ret == true) {
        ret = (str == js_str_t1.generate());
    }

    if (ret == false) {
        LOG_GLOBAL_DEBUG("Test_Str1: %s", str.c_str());
        return false;
    }

    JsonString js_str_t2(str.c_str());
    ret = (str.length() == js_str_t2.generate().length());
    if (ret == true) {
        ret = (str == js_str_t2.generate());
    }
    
    if (ret == false) {
        LOG_GLOBAL_DEBUG("Test_Str2: %s", str.c_str());
        return false;
    }

    JsonString js_str_t3;
    
    ByteBuffer_Iterator bbegin = buff1.begin();
    ByteBuffer_Iterator bend = buff1.end();

    js_str_t3.parse(bbegin, bend);
    ret = (str.length() == js_str_t3.generate().length());
    if (ret == true) {
        ret = (str == js_str_t3.generate());
    }
    
    if (ret == false) {
        LOG_GLOBAL_DEBUG("Test_Str3: %s", str.c_str());
        return false;
    }

    return ret;
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
        std::cout << "===============异常测试=================" << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << "=======================================" << std::endl;
    }
}

TEST_F(WeJson_Test, STRING_TEST)
{
    ByteBuffer buff1, buff2;
    for (int i = 0;i < 190; ++i) {
        random_str(11560, buff1, buff2);
        ASSERT_EQ(str_compare(buff1, buff2), true);
    }
}

TEST_F(WeJson_Test, BooleanTest)
{
    ByteBuffer buff1;
    JsonBool jb1(true), jb2(false);

    ASSERT_EQ(jb1.generate(), "true");
    ASSERT_EQ(jb2.generate(), "false");

    buff1.write_string("true");
    auto bbegin = buff1.begin();
    auto bend = buff1.end();

    jb1.parse(bbegin, bend);
    ASSERT_EQ(jb1.generate(), "true");

    buff1.clear();
    buff1.write_string("false");
    bbegin = buff1.begin();
    bend = buff1.end();

    jb1.parse(bbegin, bend);
    ASSERT_EQ(jb1.generate(), "false");
}

TEST_F(WeJson_Test, NullTest)
{
    ByteBuffer buff1;
    JsonNull jn1;

    buff1.write_string("null");
    auto bbegin = buff1.begin();
    auto bend = buff1.end();

    jn1.parse(bbegin, bend);
    ASSERT_EQ(jn1.generate(), "null");
}

TEST_F(WeJson_Test, ObjectTest)
{
    try {
        WeJson js("");
    } catch (exception &e) {
        std::cout << "===============异常测试=================" << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << "=======================================" << std::endl;
    }

    WeJson js("{}"), obj("{\"name\":\"Hello, World!\", \"tnull\": null, \"num\": 12.34, \"bool\": true}"), arr("[true, \"Hello\", null, 12.45]");
    ASSERT_EQ((js.begin() == js.end()), true);

    js["st,r"] = "te\nst,val   ue";
    js["bo ol"] = false;
    js["double"] = 12.3455;
    js["int"] = 12300;

    cout << obj.format_json() <<endl;

    js["null"] = JsonNull();
    ASSERT_EQ(js["st,r"], "te\nst,val   ue");
    
    cout << js.format_json() <<endl;

    arr.add(obj);
    obj.add("arr", arr);
    arr.add(obj);
    obj.add("arr2", arr);
    js["test-obj"] = obj;
    js["test-arr"] = arr;

    ASSERT_EQ(js["test-obj"]["num"], 12.34);
    ASSERT_EQ(js["test-obj"]["bool"], true);
    ASSERT_EQ(js["test-obj"]["name"], "Hello, World!");
    ASSERT_EQ(js["test-obj"]["tnull"], "null");

    ASSERT_EQ(js["test-obj"]["arr"][0], true);
    ASSERT_EQ(js["test-obj"]["arr"][1], "Hello");
    ASSERT_EQ(js["test-obj"]["arr"][2], JsonNull());
    ASSERT_EQ(js["test-obj"]["arr"][3], 12.45);
    ASSERT_EQ(js["test-obj"]["arr"][4]["name"], "Hello, World!");

    JsonNull jnval = js["test-obj"]["tnull"];
    ASSERT_EQ(jnval.generate(), "null");

    JsonNumber jnumval = js["test-obj"]["num"]; // 不能直接将double类型的json值赋给double
    ASSERT_EQ(jnumval, 12.34);
    double numval = jnumval;
    ASSERT_EQ(numval, 12.34);

    JsonString jstrval = js["test-obj"]["name"];
    ASSERT_EQ(jstrval, "Hello, World!");
    string strval = jstrval;
    ASSERT_EQ(strval, "Hello, World!");

    JsonBool jbval = js["test-obj"]["bool"];
    ASSERT_EQ(jbval, true);
    bool bval = jbval;
    ASSERT_EQ(bval, true);

    WeJson tmp1(js.generate()), tmp2(js.format_json());
    tmp2["test-obj"]["num"] = 12.34;
    ASSERT_EQ(tmp1, tmp2);
    ASSERT_EQ(tmp1.generate(), tmp2.generate());
    // cout << js.format_json() << endl << js.generate() << endl;

    WeJson cpy_js;
    cpy_js = js;
    cout << cpy_js.format_json() << endl;
    
    ASSERT_EQ(cpy_js, js);
    cpy_js["test-obj"]["num"] = 12.3;
    ASSERT_NE(cpy_js["test-obj"]["num"], 12.34);
    ASSERT_EQ(cpy_js["test-obj"]["num"], 12.3);
}

}
}
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}