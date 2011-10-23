#include "memstream.h"
#include <string.h>
#include <stdio.h>

static uint8_t* g_buffer = NULL;
static size_t g_size = 0;

static size_t last_file_size = 0;

struct memstream
{
   memstream(uint8_t *buffer, size_t max_size) : m_buf(buffer), m_size(max_size), m_ptr(0)
   {}

   uint8_t *m_buf;
   size_t m_size;
   size_t m_ptr;
};

void memstream_set_buffer(uint8_t *buffer, size_t size)
{
   g_buffer = buffer;
   g_size = size;
}

size_t memstream_get_last_size()
{
   return last_file_size;
}

memstream_t *memstream_open()
{
   if (!g_buffer || !g_size)
      return NULL;

   memstream_t *stream = new memstream_t(g_buffer, g_size);
   g_buffer = NULL;
   g_size = 0;
   return stream;
}

void memstream_close(memstream_t *stream)
{
   last_file_size = stream->m_ptr;
   delete stream;
}

size_t memstream_read(memstream_t *stream, void *data, size_t bytes)
{
   size_t avail = stream->m_size - stream->m_ptr;
   if (bytes > avail)
      bytes = avail;

   memcpy(data, stream->m_buf + stream->m_ptr, bytes);
   stream->m_ptr += bytes;
   return bytes;
}

size_t memstream_write(memstream_t *stream, const void *data, size_t bytes)
{
   size_t avail = stream->m_size - stream->m_ptr;
   if (bytes > avail)
      bytes = avail;

   memcpy(stream->m_buf + stream->m_ptr, data, bytes);
   stream->m_ptr += bytes;
   return bytes;
}

int memstream_seek(memstream_t *stream, int offset, int whence)
{
   size_t ptr;
   if (whence == SEEK_SET)
      ptr = offset;
   else if (whence == SEEK_CUR)
      ptr = stream->m_ptr + offset;
   else if (whence == SEEK_END)
      ptr = stream->m_size + offset;
   else
      return -1;

   if (ptr <= stream->m_size)
   {
      stream->m_ptr = ptr;
      return 0;
   }
   else
      return -1;
}

size_t memstream_pos(memstream_t *stream)
{
   return stream->m_ptr;
}

char *memstream_gets(memstream_t *stream, char *buffer, size_t len)
{
   return NULL;
}

int memstream_getc(memstream_t *stream)
{
   if (stream->m_ptr >= stream->m_size)
      return EOF;
   else
      return stream->m_buf[stream->m_ptr++];
}
