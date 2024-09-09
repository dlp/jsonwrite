#ifndef _JSONWRITE_H_
#define _JSONWRITE_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


/* opaque type */
struct jwr;

void jwr_init_r(struct jwr *jwr, char *buf, size_t size);
size_t jwr_finish_r(struct jwr *jwr);
void jwr_str_r(struct jwr *jwr, const char *s);
void jwr_int_r(struct jwr *jwr, int64_t val);
void jwr_null_r(struct jwr *jwr);
void jwr_bool_r(struct jwr *jwr, bool val);
void jwr_arr_r(struct jwr *jwr);
void jwr_obj_r(struct jwr *jwr);
void jwr_key_r(struct jwr *jwr, const char *key);
void jwr_pop_r(struct jwr *jwr);


void jwr_init(char *buf, size_t size);
size_t jwr_finish(void);
void jwr_str(const char *s);
void jwr_int(int64_t val);
void jwr_null(void);
void jwr_bool(bool val);
void jwr_arr(void);
void jwr_obj(void);
void jwr_key(const char *key);
void jwr_pop(void);


#endif /* _JSONWRITE_H_ */
