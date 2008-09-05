/*
** $Id: libhashish.h 15 2007-08-23 15:17:56Z hgndgtl $
**
** Copyright (C) 2006 - Hagen Paul Pfeifer <hagen@jauu.net>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef _LIB_HASHISH_H
#define	_LIB_HASHISH_H

#ifdef _MSC_VER		/* typedefs for MSVC */
	typedef signed char             int8_t;
	typedef short int               int16_t;
	typedef int                     int32_t;
	typedef __int64                 int64_t;

	typedef unsigned char             uint8_t;
	typedef unsigned short int        uint16_t;
	typedef unsigned long             uint32_t;
	/* no uint64_t */

	#define vsnprintf _vsnprintf /* MSVC does not support these standard C99 funcs */
	#define snprintf _snprintf
#else
#include <inttypes.h> /* include stdint.h */
#endif
#include <stdio.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Error handling stuf */

#define	HI_ERR_SYSTEM       -1
#define	HI_ERR_NODATA       -2
#define	HI_ERR_INTERNAL     -3
#define	HI_ERR_NOKEY        -4
#define	HI_ERR_DUPKEY       -5
#define	HI_ERR_NOTIMPL      -6
#define	HI_ERR_RANGE        -7
#define HI_ERR_NOFUNC       -8

#define HI_ERR_SUCCESS       0
#define	HI_SUCCESS HI_ERR_SUCCESS



struct hashfunc_map_t {
	const unsigned int no;
	const char *name;
	uint32_t (*hashfunc)(const uint8_t*, uint32_t);
};

extern const struct hashfunc_map_t lhi_hashfunc_map[];


enum hash_alg {
	HI_HASH_DJB2 = 0,
	HI_HASH_DUMB1,
	HI_HASH_ELF,
	HI_HASH_GOULBURN,
	HI_HASH_HSIEH,
	HI_HASH_JENKINS2,
	HI_HASH_JENKINS3,
	HI_HASH_PHONG,
	HI_HASH_TOREK,
	HI_HASH_XOR,
	HI_HASH_KORZENDORFER1,
	HI_HASH_KORZENDORFER2,
	HI_HASH_SHA1,
	HI_HASH_MURMUR,
	__HI_HASH_MAX
};

#define	HI_HASH_DEFAULT HI_HASH_HSIEH

#define	HI_HASH_MAX (__HI_HASH_MAX - 1)

enum coll_eng {
	COLL_ENG_LIST = 1,
	COLL_ENG_LIST_HASH,
	COLL_ENG_LIST_MTF,
	COLL_ENG_LIST_MTF_HASH,
	COLL_ENG_ARRAY,
	COLL_ENG_ARRAY_HASH,
	COLL_ENG_ARRAY_DYN,
	COLL_ENG_ARRAY_DYN_HASH,
	COLL_ENG_RBTREE,
	__COLL_ENG_MAX
};

#define COLL_ENG_MAX (__COLL_ENG_MAX - 1)

/* This struct is passed to hi_init and determine
 * the hash libaray behaviour.
 *
 * NOTE: if you add additional field make sure you
 * adopt all dependcies functions like lhi_transform_hndl_2_hndl()!
 */
struct hi_init_set {
	uint32_t table_size;
	int rehash_auto; /* < signals if the table perform self rehashing */
	float rehash_threshold; /* < if self_resizing is true then this is the threshold */
	uint32_t coll_eng_array_size;
	enum coll_eng coll_eng;
	uint32_t (*hash_func)(const uint8_t*, uint32_t);
	uint32_t (*hash2_func)(const uint8_t*, uint32_t);
	int (*key_cmp)(const uint8_t *, const uint8_t *);
};

#define	DEFAULT_REHASHING_THRESHOLD (0.7f)

struct rb_root;
struct __hi_rb_tree {
	struct { void *rb_node; } root;
	pthread_rwlock_t *rwlock;
};

struct lhi_list_head
{
	struct lhi_list_head *	next;
	struct lhi_list_head *	prev;
};

 typedef struct __hi_bucket_obj {
     uint32_t                 key_len; /* key length in bytes */
     const void              *key;
     const void              *data;
     struct lhi_list_head    *hi_handle;
     struct lhi_list_head     list;
 } hi_bucket_obj_t;


 typedef struct __hi_bucket_hl_obj {
     uint32_t                 key_len; /* key length in bytes */
     const void              *key;
     uint32_t                 key_hash;
     const void               *data;
     struct lhi_list_head    *hi_handle;
     struct lhi_list_head     list;
 } hi_bucket_hl_obj_t;

 /* CHAINING_ARRAY elements */
 typedef struct __hi_bucket_a_obj {
     uint32_t                 key_len; /* key length in bytes */
     const void              *key;
     uint32_t                 key_hash;
     const void              *data;
	 int					  allocation; /* BA_NOT_ALLOCATED or BA_ALLOCATED */
 } hi_bucket_a_obj_t;

typedef struct __hi_handle {

	/* data from user -> hi_init_set */
	uint32_t table_size;  /* < the table size */
	enum coll_eng coll_eng; /* < the selected collision engine */
	int rehash_auto;      /* < flag to indicate self resizing or not */
	float rehash_threshold; /* < if self_resizing is true then this is the threshold */
	uint32_t coll_eng_array_size; /* < if collision engine is array then this indicates the size */
	uint32_t (*hash_func)(const uint8_t*, uint32_t); /* < the primary hash function */
	uint32_t (*hash2_func)(const uint8_t*, uint32_t); /* < *_HASH collision engines requires a second hash function */
	int (*key_cmp)(const uint8_t *, const uint8_t *); /* < the key compare function e.g. strcmp() */
	struct my_stuff * (*rb_search)(struct rb_root *, void *); /* < red black trees requires an addtional search function */

	/* statistic data */

	/* the current number elements in the particular bucket */
	uint32_t *bucket_size;
	/* the total amount of entries in the whole table */
	uint32_t no_objects;

	/* collision engine specific data */
	union {
		struct {
			struct lhi_list_head *bucket_table;
		} eng_list;
		struct {
			hi_bucket_a_obj_t **bucket_array;
			uint32_t    *bucket_array_slot_size;
			uint32_t    *bucket_array_slot_max;
		} eng_array;
		struct {
			struct __hi_rb_tree *trees;
		} eng_rbtree;
	};

	/* thread locking stuff */
	pthread_mutex_t *mutex_lock;

} hi_handle_t;

/* hashfunc.c */
uint32_t lhi_hash_dumb1(const uint8_t *, uint32_t);
uint32_t lhi_hash_hsieh(const uint8_t *, uint32_t);
uint32_t lhi_hash_jenkins2(const uint8_t *, uint32_t);
uint32_t lhi_hash_jenkins3(const uint8_t *, uint32_t);
uint32_t lhi_hash_goulburn(const uint8_t *, uint32_t);
uint32_t lhi_hash_phong(const uint8_t *, uint32_t);
uint32_t lhi_hash_torek(const uint8_t *, uint32_t);
uint32_t lhi_hash_elf(const uint8_t *, uint32_t);
uint32_t lhi_hash_djb2(const uint8_t *, uint32_t);
uint32_t lhi_hash_xor(const uint8_t *, uint32_t);
uint32_t lhi_hash_kr(const uint8_t *, uint32_t);
uint32_t lhi_hash_sdbm(const uint8_t *, uint32_t);
uint32_t lhi_hash_korzendorfer1(const uint8_t *, uint32_t);
uint32_t lhi_hash_korzendorfer2(const uint8_t *, uint32_t);
uint32_t lhi_hash_sha1(const uint8_t *, uint32_t);
uint32_t lhi_hash_murmur(const uint8_t *, uint32_t);

/* hi_fini.c */
int hi_fini(hi_handle_t *);

/* libhashish.c */
int hi_create(hi_handle_t **, struct hi_init_set *);
int hi_rehash(hi_handle_t *, uint32_t);

/* hi_iterator.c */
struct hi_operator;
typedef struct hi_operator hi_iterator_t;
int hi_iterator_create(hi_handle_t *t, hi_iterator_t **);
int hi_iterator_reset(hi_iterator_t *);
int hi_iterator_getnext(hi_iterator_t *, void **, void **, uint32_t *);
void hi_iterator_fini(hi_iterator_t *);


/* hi_set.c */
void hi_set_zero(struct hi_init_set *);
int hi_set_bucket_size(struct hi_init_set *, uint32_t);
int hi_set_hash_alg(struct hi_init_set *, enum hash_alg);
int hi_set_hash_func(struct hi_init_set *, uint32_t (*hash_func)(const uint8_t*, uint32_t));
int hi_set_hash2_alg(struct hi_init_set *, enum hash_alg);
int hi_set_hash2_func(struct hi_init_set *, uint32_t (*hash_func)(const uint8_t*, uint32_t));
int hi_set_coll_eng(struct hi_init_set *, enum coll_eng);
int hi_set_key_cmp_func(struct hi_init_set *, int (*cmp)(const uint8_t *, const uint8_t *));
void hi_set_rehash_auto(struct hi_init_set *, int);
void hi_set_rehash_threshold(struct hi_init_set *, float);
int hi_set_coll_eng_array_size(struct hi_init_set *, uint32_t);

/* xutils.c */
const char *hi_strerror(const int);

/* cmp_funcs.c */
int hi_cmp_str(const uint8_t *, const uint8_t *);
int hi_cmp_int32(const uint8_t *, const uint8_t *);

/* hi_operations */
int hi_insert(hi_handle_t *, const void *, uint32_t, const void *);
int hi_get(const hi_handle_t *, const void *, uint32_t, void **);
int hi_remove(hi_handle_t *, void *, uint32_t, void **);


/* helper function section */

static inline uint32_t hi_no_objects(const hi_handle_t *h)
{
	return h->no_objects;
}

static inline uint32_t hi_table_size(hi_handle_t *h)
{
	return h->table_size;
}

double hi_table_load_factor(hi_handle_t *);

/* string specific functions */
int hi_init_str(hi_handle_t **, const uint32_t);
int hi_insert_str(hi_handle_t *, const char *, const void *);
int hi_get_str(hi_handle_t *, const char *, void **);
int hi_remove_str(hi_handle_t *, const char *, void **);


/* BLOOM Filter Implementation */

typedef struct __hi_bloom_handle {
	uint8_t *filter_map;
	uint32_t m;
	uint32_t bit_collision;
	uint32_t n;
	unsigned k;
} hi_bloom_handle_t;

void hi_bloom_filter_add(hi_bloom_handle_t *, uint8_t *, uint32_t);
int hi_bloom_filter_check(hi_bloom_handle_t *, void *, uint32_t);
void hi_bloom_filter_add_str(hi_bloom_handle_t *, const char *);
int hi_bloom_filter_check_str(hi_bloom_handle_t *, const char *);

int hi_bloom_bit_get(hi_bloom_handle_t *, uint32_t);
int hi_bloom_print_hex_map(hi_bloom_handle_t *);

int hi_bloom_init_mk(hi_bloom_handle_t **, uint32_t, uint32_t);
void hi_fini_bloom_filter(hi_bloom_handle_t *);

double hi_bloom_current_false_positiv_probability(hi_bloom_handle_t *);
double hi_bloom_false_positiv_probability(uint32_t, uint32_t, uint32_t);




#ifdef __cplusplus
}
#endif

#endif /* _LIB_HASHISH_H */

/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
