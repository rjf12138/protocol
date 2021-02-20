### JSON 使用
1. 创建 json
    a.  WeJson js;

    b.  std::string sjs = "{\"key1\": \"value\"}";
        WeJson js1(sjs);
    
    c. ByteBuffer buff;
       buff.write_string(sjs);
       WeJson js1(buff);

2. 解析 json
    WeJson js;

    std::string sjs = "{\"key1\": \"value\"}";

    ByteBuffer buff;
    buff.write_string(sjs);

    a. js.parse(sjs);
    b. js.parse(buff);

3. 访问 json 值
    WeJson obj("{\"name\":\"Hello, World!\", \"tnull\": null, \"num\": 12.34, \"bool\": true, \"obj\": {\"obj_ele\": true}, \"arr\":[true, 12.34]}");

    JsonString jstr = obj["name"];
    JsonNull jnull = obj["tnull"];
    JsonBool jbool = obj["bool"];
    JsonNull jnum = obj["num"];
    JsonObject jobj = obj["obj"];
    JsonArray jarr = obj["arr"];

    JsonBool jbool = obj["obj"]["obj_ele"]; // 访问object下的元素
    JsonBool jbool1 = obj["arr"][0]; // 访问array下的元素
    
    // 转成 c++ 内置类型
    bool b = jbool;
    double num = jnum;
    std::string str = jstr;

4. 添加元素（// key 存在就覆盖掉原先的值， 不存在就创建。数组创建元素时与下标无关，直接添加到数组尾部）
    WeJson obj("{}"), arr("[]");

    // 方式1
    obj.add("str", "Hello"); // 对象是键值对
    obj.add("int", 123);
    obj.add("bool", false);
    obj.add("null", JsonNull());

    arr.add("Hello"); // 数组直接添加值
    arr.add(123);
    arr.add(false);
    arr.add(JsonNull());
    arr.add(obj);

    obj.add("arr", arr);

    // 方式2
    obj.["str"] = "Hello";
    obj.["int"] = 123;
    obj.["bool"] = false;
    obj.["null"] = JsonNull();

    arr[0] = "Hello"; // 数组直接添加值
    arr[1] = 123;
    arr[2] = false;
    arr[3] = JsonNull();
    arr[4] = obj;

    obj.add("arr", arr);

5. 输出成字符流
    1. obj.generate();  // 输出后时压缩的
    2. obj.format_json();// 输出后会格式化