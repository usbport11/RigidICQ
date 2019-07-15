#ifndef md5H
#define md5H

/* Define the state of the MD5 Algorithm. */
typedef unsigned char mir_md5_byte_t; /* 8-bit byte */
typedef unsigned int mir_md5_word_t; /* 32-bit word */

typedef struct mir_md5_state_s {
	mir_md5_word_t count[2];  /* message length in bits, lsw first */
	mir_md5_word_t abcd[4];    /* digest buffer */
	mir_md5_byte_t buf[64];    /* accumulate block */
} mir_md5_state_t;

class MMD5Interface
{
public:
	int cbSize;
	void md5_init(mir_md5_state_t *pms);
	void md5_append(mir_md5_state_t *pms, const mir_md5_byte_t *data, int nbytes);
	void md5_finish(mir_md5_state_t *pms, mir_md5_byte_t digest[16]);
};

#endif