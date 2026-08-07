/***********************************************************************************
  MSU1 pack (.msu1) reader - zip container holding the MSU1 companion files.
  See msu1_pack.c for the format and seek-cost notes.
 ***********************************************************************************/

#ifndef _MSU1_PACK_H_
#define _MSU1_PACK_H_

#include <stdint.h>
#include <streams/file_stream.h>

struct msu1_pack_entry
{
	uint16_t	method;       /* 0 = stored, 8 = deflate */
	uint32_t	comp_size;
	uint32_t	uncomp_size;
	uint32_t	data_off;     /* offset of the entry's data in the pack */
};

struct msu1_pack_file
{
	RFILE			*file;
	struct msu1_pack_entry	 entry;

	/* Streaming inflate state, only used for method 8. */
	void		*inf;
	uint32_t	 out_pos;     /* uncompressed bytes produced so far */
	uint32_t	 in_left;     /* compressed bytes not yet fed */
	uint32_t	 in_have;     /* valid bytes in in_buf */
	uint32_t	 in_pos;      /* consumed bytes in in_buf */
	uint8_t		 eof;
	uint8_t		 in_buf[16384];
};

/* Open `pack_path` and mount the first entry whose name ends in `suffix`
   (case-insensitive). Returns FALSE if the pack or the entry is missing, or
   the entry uses a compression method other than store/deflate. */
int      msu1_pack_open  (struct msu1_pack_file *pf, const char *pack_path, const char *suffix);
void     msu1_pack_close (struct msu1_pack_file *pf);

/* Uncompressed size of the mounted entry. */
uint32_t msu1_pack_size  (const struct msu1_pack_file *pf);

/* Read `len` bytes at `offset` within the mounted entry. Returns bytes read.
   Stored entries seek freely; deflated entries restart the inflate on a
   backward seek. */
uint32_t msu1_pack_read  (struct msu1_pack_file *pf, uint32_t offset, uint8_t *out, uint32_t len);

#endif
