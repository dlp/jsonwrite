
jwr_obj();
for (int i = 0; i < 100; i++)
{
    char tmp[10];
    (void) snprintf(tmp, sizeof tmp, "key_%02d", i);
    jwr_key(tmp);
    jwr_int(i);
}
jwr_pop();
