
jwr_arr();
jwr_int(42);
jwr_str("foo");
jwr_arr();
jwr_bool(1);
jwr_int(-456);
jwr_bool(0);

jwr_obj();
jwr_key("elem1");
jwr_null();
jwr_key("elem2");
jwr_str("val2");
jwr_key("elem3");

jwr_arr();
jwr_int(42);
jwr_str("foo");
jwr_close();

jwr_key("elem4");
jwr_obj();
jwr_key("nk1");
jwr_str("nv1");
jwr_key("nk2");
jwr_str("nv2");
jwr_close();

jwr_close();

jwr_close();
jwr_null();
jwr_close();

