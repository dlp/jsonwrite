                    jwr_dump();
jwr_obj();          jwr_dump();
jwr_key("key");     jwr_dump();
jwr_obj();          jwr_dump();
jwr_key("key");     jwr_dump();
jwr_obj();          jwr_dump();
jwr_key("key");     jwr_dump();
jwr_str("value");   jwr_dump();
jwr_close();        jwr_dump();
jwr_close();        jwr_dump();
jwr_key("key2");    jwr_dump();
jwr_str("value");   jwr_dump();
jwr_close();        jwr_dump();

// provoke error
jwr_null();
