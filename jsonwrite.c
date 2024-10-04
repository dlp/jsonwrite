
#define _GNU_SOURCE
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsonwrite.h"

/* having character helped debugging */
#define JWR_ARR     'A'
#define JWR_OBJ     'O'
#define JWR_KEY     'K'

/* this flag turns the characters to lowercase */
#define JWR_NXT     0x20U

#define JWR_STK_MAX  32

//#define JWR_PRETTY
#define JWR_COMPACT

struct jwr {
    char *buf;
    size_t size;
    size_t pos;
#ifdef JWR_PRETTY
    int depth;
#endif /* JWR_PRETTY */
    char *tos;
    char stk[JWR_STK_MAX];
};

#ifdef DEBUG
void jwr_dump_r(struct jwr *jwr)
{
    fprintf(stderr, "-- DUMP--\n%s\n", jwr->buf);

    fputc('[', stderr);
    for (char *s = jwr->stk; s <= jwr->tos; s++) fputc(*s, stderr);
    fputc(']', stderr);
    fputc('\n', stderr);
}
#endif /* DEBUG */

static void jwr_raw(struct jwr *jwr, const char *raw, size_t len)
{
    assert(jwr->pos + len < jwr->size);
    (void) memcpy(&jwr->buf[jwr->pos], raw, len);
    jwr->pos += len;
}

static void jwr_char(struct jwr *jwr, char c)
{
    assert(jwr->pos + 1 < jwr->size);
    jwr->buf[jwr->pos++] = c;
}

#ifdef JWR_PRETTY
static void jwr_indent(struct jwr *jwr)
{
    jwr_char(jwr, '\n');
    for (int i = 0; i < jwr->depth * 2; i++)
    {
        jwr_char(jwr, ' ');
    }
}
#endif /* JWR_PRETTY */

static void jwr_push(struct jwr *jwr, char type)
{
    assert(jwr->tos < (jwr->stk + JWR_STK_MAX));
    *(++jwr->tos) = type;
}

static bool jwr_tos_is(struct jwr *jwr, char type)
{
    return (*jwr->tos & ~JWR_NXT) == type;
}

/**
 * If the tos is a key, pop it off.
 * This function is called after every item, and ensures that
 * a key has only one value.
 */
static void jwr_popkey(struct jwr *jwr)
{
    if (jwr_tos_is(jwr, JWR_KEY))
    {
        assert ((*jwr->tos & JWR_NXT) == JWR_NXT);
        *(jwr->tos--) = '\0';
    }
}

/**
 * If this is a follow-up item in a list, write a comma as sepataror.
 * This is done via a NXT flag in the tos item, and if it is set
 * after the first item, it's presence will cause a comma to be written.
 * Multiple items on top-level are not allowed.
 */
static void jwr_sep(struct jwr *jwr)
{
    if (jwr->tos > jwr->stk)
    {
        if ((*jwr->tos & JWR_NXT) == JWR_NXT)
        {
            jwr_char(jwr, ',');
#ifdef JWR_PRETTY
            jwr_indent(jwr);
#else
#  ifndef JWR_COMPACT
            jwr_char(jwr, ' ');
#  endif /* JWR_COMPACT */
#endif /* JWR_PRETTY */
        }
    }
    else
    {
        /* we are top level, one leaf was already written */
        assert((*jwr->tos & JWR_NXT) != JWR_NXT);
    }
    /* set flag for the next time */
    *jwr->tos |= JWR_NXT;
}

/**
 * Write a quoted string
 */
static void jwr_qstr(struct jwr *jwr, const char *s)
{
    jwr_char(jwr, '"');
    jwr_raw(jwr, s, strnlen(s, jwr->size));
    jwr_char(jwr, '"');
}

/*****************************************************************************
 * Public functions
 *****************************************************************************/

/**
 * Initialize json writing: assign a buffer
 *
 * \param buf   buffer which to write to
 * \param size  size of the buffer
 */
void jwr_init_r(struct jwr *jwr, char *buf, size_t size)
{
    jwr->buf = buf;
    jwr->size = size;
    jwr->pos = 0;
#ifdef JWR_PRETTY
    jwr->depth = 0;
#endif /* JWR_PRETTY */
    jwr->tos = jwr->stk;

    (void) memset(jwr->stk, 0, sizeof jwr->stk);
}

/**
 * Finish the json writing to the buffer.
 *
 * \return the size of the written json string
 */
size_t jwr_finish_r(struct jwr *jwr)
{
    assert(jwr->tos == jwr->stk && (*jwr->tos & ~JWR_NXT) == '\0');
#ifdef JWR_PRETTY
    assert(jwr->depth == 0);
#endif /* JWR_PRETTY */
    jwr->buf[jwr->pos] = '\0';
    return jwr->pos;
}

/**
 * Write a string
 */
void jwr_str_r(struct jwr *jwr, const char *s)
{
    assert(!jwr_tos_is(jwr, JWR_OBJ));
    jwr_sep(jwr);
    jwr_qstr(jwr, s);
    jwr_popkey(jwr);
}

/**
 * Write an integer value
 */
void jwr_int_r (struct jwr *jwr, int64_t val)
{
    int cnt;

    assert (!jwr_tos_is (jwr, JWR_OBJ));
    jwr_sep (jwr);

    /* writes at most (size - pos) bytes including '\0' terminator */
    cnt = snprintf (&jwr->buf[jwr->pos], jwr->size - jwr->pos, "%"PRIi64, val);

    /* return value is the number of bytes written (or would have been written)
        * excluding the '\0' terminator. Hence, a value equal or greater than the
        * limit indicates that it was truncated (did not fit)
        */
    assert (cnt > 0 && cnt < (int)(jwr->size - jwr->pos));
    jwr->pos += (size_t) cnt;

    jwr_popkey (jwr);
}

/**
 * Write the literal "null"
 */
void jwr_null_r(struct jwr *jwr)
{
    assert(!jwr_tos_is(jwr, JWR_OBJ));
    jwr_sep(jwr);
    jwr_raw(jwr, "null", 4);
    jwr_popkey(jwr);
}

/**
 * Write a boolean literal
 */
void jwr_bool_r(struct jwr *jwr, bool val)
{
    assert(!jwr_tos_is(jwr, JWR_OBJ));
    jwr_sep(jwr);
    if (val) {
        jwr_raw(jwr, "true", 4);
    } else {
        jwr_raw(jwr, "false", 5);
    }
    jwr_popkey(jwr);
}

/**
 * Start an array
 */
void jwr_arr_r(struct jwr *jwr)
{
    jwr_sep(jwr);
    jwr_push(jwr, JWR_ARR);
    jwr_char(jwr, '[');
#ifdef JWR_PRETTY
    jwr->depth++;
    jwr_indent(jwr);
#endif /* JWR_PRETTY */
}

/**
 * Start a json object
 */
void jwr_obj_r(struct jwr *jwr)
{
    jwr_sep(jwr);
    jwr_push(jwr, JWR_OBJ);
    jwr_char(jwr, '{');
#ifdef JWR_PRETTY
    jwr->depth++;
    jwr_indent(jwr);
#endif /* JWR_PRETTY */
}

/**
 * Write a key, only string allowed.
 * Keys must have a json object as parent.
 */
void jwr_key_r(struct jwr *jwr, const char *key)
{
    jwr_sep(jwr);
    assert(jwr_tos_is(jwr, JWR_OBJ));
    jwr_push(jwr, JWR_KEY);
    jwr_qstr(jwr, key);
    jwr_char(jwr, ':');
#ifndef JWR_COMPACT
    jwr_char(jwr, ' ');
#endif /* JWR_COMPACT */
}

/**
 * Close the array/object
 */
void jwr_close_r(struct jwr *jwr)
{
    char type = *jwr->tos & ~JWR_NXT;

    char c = (type == JWR_ARR) ? ']' :
             (type == JWR_OBJ) ? '}' :
                            -1 ;
    *(jwr->tos--) = '\0';
    assert(c != -1);
#ifdef JWR_PRETTY
    jwr->depth--;
    jwr_indent(jwr);
#endif /* JWR_PRETTY */
    jwr_char(jwr, c);
    jwr_popkey(jwr);
}


/*****************************************************************************
 * Non-reentrant short forms
 *****************************************************************************/

/* global struct - for non-reentrant, more compact use */
static struct jwr g_jwr;

void jwr_init(char *buf, size_t size) { jwr_init_r(&g_jwr, buf, size); }
size_t jwr_finish(void) { return jwr_finish_r(&g_jwr); }
void jwr_str(const char *s) { jwr_str_r(&g_jwr, s); }
void jwr_int(int64_t val) { jwr_int_r(&g_jwr, val); }
void jwr_null(void) { jwr_null_r(&g_jwr); }
void jwr_bool(bool val) { jwr_bool_r(&g_jwr, val); }
void jwr_arr(void) { jwr_arr_r(&g_jwr); }
void jwr_obj(void) { jwr_obj_r(&g_jwr); }
void jwr_key(const char *key) { jwr_key_r(&g_jwr, key); }
void jwr_close(void) { jwr_close_r(&g_jwr); }

#ifdef DEBUG
void jwr_dump(void) { jwr_dump_r(&g_jwr); }
#endif /* DEBUG */
