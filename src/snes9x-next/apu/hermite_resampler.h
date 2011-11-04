/* Simple resampler based on bsnes's ruby audio library */
/* Simple byte-based ring buffer. Licensed under public domain (C) BearOso. */

#ifndef __HERMITE_RESAMPLER_H
#define __HERMITE_RESAMPLER_H

#include <string.h>

#undef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#undef CLAMP
#undef SHORT_CLAMP
#define CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define SHORT_CLAMP(n) ((short) CLAMP((n), -32768, 32767))

class HermiteResampler
{
	protected:

	double r_step;
	float r_frac;
	int    chandata[2][4];

	//from ring buffer
	int size;
	int buffer_size;
	int start;
	unsigned char *buffer;

	static inline float hermite (float mu1, float a, float b, float c, float d)
	{
		float mu2 = mu1 + mu1;
		float mu3 = mu1 + mu1 + mu1;

		float m0  = (c - a) * 0.5;
		float m1 = (d - b) * 0.5;

		float a0 = +2 * mu3 - 3 * mu2 + 1;
		float a1 =      mu3 - 2 * mu2 + mu1;
		float a2 =      mu3 -     mu2;
		float a3 = -2 * mu3 + 3 * mu2;

		return (a0 * b) + (a1 * m0) + (a2 * m1) + (a3 * c);
	}

	public:
	HermiteResampler (int num_samples)
	{
		//from ring buffer
		buffer_size = num_samples << 1;
		buffer = new unsigned char[buffer_size];
		memset (buffer, 0, buffer_size);

		size = 0;
		start = 0;

		//from hermite resampler
		clear ();
	}

	~HermiteResampler ()
	{
		delete[] buffer;
	}

	void time_ratio (double ratio)
	{
		r_step = ratio;
		clear ();
	}

	void clear (void)
	{
		//from ring buffer
		start = 0;
		size = 0;
		memset (buffer, 0, buffer_size);

		//from hermite
		r_frac = 1.0;
		chandata[0][0] = chandata[0][1] = chandata[0][2] = chandata[0][3] = 0;
		chandata[1][0] = chandata[1][1] = chandata[1][2] = chandata[1][3] = 0;
	}

	void read (short *data, int num_samples)
	{
		int i_position = start >> 1;
		short *internal_buffer = (short *) buffer;
		int o_position = 0;
		int consumed = 0;

		while (o_position < num_samples)
		{
			int s_left = internal_buffer[i_position];
			int s_right = internal_buffer[i_position + 1];
			int max_samples = buffer_size >> 1;

			while (r_frac <= 1.0 && o_position < num_samples)
			{
				data[o_position]     = SHORT_CLAMP (hermite (r_frac, chandata[0][0], chandata[0][1], chandata[0][2], chandata[0][3]));
				data[o_position + 1] = SHORT_CLAMP (hermite (r_frac, chandata[1][0], chandata[1][1], chandata[1][2], chandata[1][3]));

				o_position += 2;

				r_frac += r_step;
			}

			if (r_frac > 1.0)
			{
				r_frac -= 1.0;

				chandata[0][0] = chandata[0][1];
				chandata[0][1] = chandata[0][2];
				chandata[0][2] = chandata[0][3];
				chandata[0][3] = s_left;

				chandata[1][0] = chandata[1][1];
				chandata[1][1] = chandata[1][2];
				chandata[1][2] = chandata[1][3];
				chandata[1][3] = s_right;                    

				i_position += 2;
				if (i_position >= max_samples)
					i_position -= max_samples;
				consumed += 2;
			}
		}

		size -= consumed << 1;
		start += consumed << 1;
		if (start >= buffer_size)
			start -= buffer_size;
	}

	inline int avail (void)
	{
		return (int) floor (((size >> 2) - r_frac) / r_step) * 2;
	}

	bool ring_buffer_push (unsigned char *src, int bytes)
	{
		if (space_empty () < bytes)
			return false;

		int end = (start + size) % buffer_size;
		int first_write_size = MIN (bytes, buffer_size - end);

		memcpy (buffer + end, src, first_write_size);

		if (bytes > first_write_size)
			memcpy (buffer, src + first_write_size, bytes - first_write_size);

		size += bytes;

		return true;
	}

	inline bool push (short *src, int num_samples)
	{
		if (max_write () < num_samples)
			return false;

		!num_samples || ring_buffer_push((unsigned char *) src, num_samples << 1);

		return true;
	}

	inline int space_empty (void)
	{
		return buffer_size - size;
	}

	inline int space_filled (void)
	{
		return size;
	}

	inline int max_write (void)
	{
		return space_empty () >> 1;
	}

	void ring_buffer_resize (int size)
	{
		delete[] buffer;
		buffer_size = size;
		buffer = new unsigned char[buffer_size];
		memset (buffer, 0, this->buffer_size);

		size = 0;
		start = 0;
	}

	inline void resize (int num_samples)
	{
		ring_buffer_resize (num_samples << 1);
	}
};

#endif /* __HERMITE_RESAMPLER_H */
