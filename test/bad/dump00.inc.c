                    jwr_dump();
jwr_obj();          jwr_dump();
jwr_key("key1");    jwr_dump();
jwr_obj();          jwr_dump();
jwr_key("key2");    jwr_dump();
jwr_arr();          jwr_dump();
jwr_str("value1");  jwr_dump();
jwr_str("value2");  jwr_dump();
jwr_close();        jwr_dump();
jwr_close();        jwr_dump();
jwr_key("key3");    jwr_dump();
jwr_str("value3");  jwr_dump();
jwr_close();        jwr_dump();

// provoke error
jwr_null();
