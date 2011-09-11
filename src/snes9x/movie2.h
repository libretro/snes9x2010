
enum MovieState
{
	MOVIE_STATE_NONE = 0,
	MOVIE_STATE_PLAY,
	MOVIE_STATE_RECORD
};

struct SMovie
{
	enum MovieState	State;

	FILE	*File;
	char	Filename[PATH_MAX + 1];
	char	ROMName[23];
	uint32	ROMCRC32;
	uint32	MovieId;
	uint32	Version;

	uint32	SaveStateOffset;
	uint32	ControllerDataOffset;

	uint8	ControllersMask;
	uint8	Opts;
	uint8	SyncFlags;
	uint32	MaxFrame;
	uint32	MaxSample;
	uint32	CurrentFrame;
	uint32	CurrentSample;
	uint32	BytesPerSample;
	uint32	RerecordCount;
	bool8	ReadOnly;
	uint8	PortType[2];
	int8	PortIDs[2][4];

	uint8	*InputBuffer;
	uint8	*InputBufferPtr;
	uint32	InputBufferSize;
};

static struct SMovie	Movie;
static bool8	prevMouseMaster, prevSuperScopeMaster, prevJustifierMaster, prevMultiPlayer5Master;
static uint8	prevPortType[2];
static int8		prevPortIDs[2][4];

#define BUFFER_GROWTH_SIZE    4096
#define MOVIE_INFO_END					"Movie end"

#define SKIPPED_POLLING_PORT_TYPE(x)	(((x) == CTL_NONE) || ((x) == CTL_JOYPAD) || ((x) == CTL_MP5))

#define Write8(v, ptr) { *ptr++ = v; }
#define Write16(v, ptr) { WRITE_WORD(ptr, v); ptr += 2; }
#define Write32(v, ptr) { WRITE_DWORD(ptr, v); ptr += 4; }

#ifndef __WIN32__
#include <unistd.h>
#endif


#if 0
static inline void truncate_movie (void)
{
	if (!Movie.File || !Settings.MovieTruncate)
		return;

	if (Movie.SaveStateOffset > Movie.ControllerDataOffset)
		return;

	int	ignore;
	ignore = ftruncate(fileno(Movie.File), Movie.ControllerDataOffset + Movie.BytesPerSample * (Movie.MaxSample + 1));
}
#endif

static inline uint16 Read16 (uint8 *&ptr)
{
	uint16	v = READ_WORD(ptr);
	ptr += 2;
	return (v);
}
