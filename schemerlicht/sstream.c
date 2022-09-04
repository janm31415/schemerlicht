#include "sstream.h"

#include <string.h>
#include <stdlib.h>

#define write_space_left(stream) \
  (stream->capacity - stream->position)

#define read_space_left(stream) \
  (stream->length - stream->position)

#define capacity_left(stream) \
  (stream->capacity - stream->length)

static void schemerlicht_stream_resize(schemerlicht_stream* stream, schemerlicht_memsize size);

void schemerlicht_stream_init(schemerlicht_context* ctxt, schemerlicht_stream* stream, schemerlicht_memsize capac)
  {
  stream->data = cast(char*, schemerlicht_malloc(ctxt, capac));  
  stream->length = 0;
  stream->position = 0;
  }

void schemerlicht_stream_close(schemerlicht_context* ctxt, schemerlicht_stream* stream)
  {
  schemerlicht_free(ctxt, stream->data, stream->capacity);  
  stream->data = NULL;
  stream->capacity = 0;
  stream->length = 0;
  stream->position = 0;
  }

void schemerlicht_stream_reserve(schemerlicht_context* ctxt, schemerlicht_stream* stream, schemerlicht_memsize size)
  {
  if (stream->capacity < size)
    {
    stream->data = schemerlicht_realloc(ctxt, stream->data, stream->capacity, size);
    stream->capacity = size;
    } 
  }

schemerlicht_memsize schemerlicht_stream_write(schemerlicht_context* ctxt, schemerlicht_stream* stream, const char* buffer, schemerlicht_memsize count, schemerlicht_memsize offset)
  {
  schemerlicht_memsize space_left = write_space_left(stream);
  if (space_left < count) 
    {
    schemerlicht_stream_reserve(ctxt, stream, stream->capacity + count-space_left);    
    }

  char* dest = &(stream->data[stream->position]);
  const char* source = &buffer[offset];
  memcpy(dest, source, count);
  stream->position += count;
  stream->length += count;
  return count;  
  }

void schemerlicht_stream_write_byte(schemerlicht_context* ctxt, schemerlicht_stream* stream, char value)
  {
  schemerlicht_stream_write(ctxt, stream, &value, 1, 0);
  }

schemerlicht_memsize schemerlicht_stream_write_str(schemerlicht_context* ctxt, schemerlicht_stream* stream, const char* str)
  {
  schemerlicht_memsize len = cast(schemerlicht_memsize, strlen(str) + 1);
  return schemerlicht_stream_write(ctxt, stream, str, len, 0);
  }

schemerlicht_memsize schemerlicht_stream_insert(schemerlicht_context* ctxt, schemerlicht_stream* stream, const char* buffer, schemerlicht_memsize count, schemerlicht_memsize offset)
  {
  schemerlicht_memsize space_left = capacity_left(stream);
  if (space_left < count) 
    {
    schemerlicht_stream_reserve(ctxt, stream, stream->capacity + count - space_left);    
    }

  char* src = &(stream->data[stream->position]);
  char* move_dest = &(stream->data[stream->position + count]);
  schemerlicht_memsize read_spc = read_space_left(stream);

  // Move data
  memmove(move_dest, src, read_spc);

  // Insert input buffer at pos
  memcpy(src, &buffer[offset], count);

  // Update position and length
  stream->position += count;
  stream->length += count;
  return count;
  }

void schemerlicht_stream_insert_byte(schemerlicht_context* ctxt, schemerlicht_stream* stream, char value)
  {
  schemerlicht_stream_insert(ctxt, stream, &value, 1, 0);
  }

schemerlicht_memsize schemerlicht_stream_insert_str(schemerlicht_context* ctxt, schemerlicht_stream* stream, const char* str)
  {  
  schemerlicht_memsize len = cast(schemerlicht_memsize, strlen(str) + 1);
  return schemerlicht_stream_insert(ctxt, stream, str, len, 0);
  }

schemerlicht_memsize schemerlicht_stream_read(schemerlicht_stream* stream, char* buffer, schemerlicht_memsize count, schemerlicht_memsize offset)
  {
  schemerlicht_memsize space_left = read_space_left(stream);
  if (space_left == 0) 
    {
    return END_OF_STREAM;
    }
  if (space_left > count) 
    {
    space_left = count;
    }

  char* dest = &buffer[offset];
  char* source = &(stream->data[stream->position]);

  memcpy(dest, source, space_left);
  stream->position += space_left;

  return space_left;
  }

schemerlicht_memsize schemerlicht_stream_read_byte(schemerlicht_stream* stream)
  {
  schemerlicht_memsize space_left = read_space_left(stream);
  if (space_left == 0) 
    {
    return END_OF_STREAM;
    }

  return cast(schemerlicht_memsize, stream->data[stream->position++]);
  }

void schemerlicht_stream_clear(schemerlicht_stream* stream)
  {
  memset(stream->data, 0, stream->capacity);
  stream->length = 0;
  stream->position = 0;
  }

schemerlicht_memsize schemerlicht_stream_copy(schemerlicht_context* ctxt, schemerlicht_stream* src, schemerlicht_stream* dest)
  {
  return cast(schemerlicht_memsize, schemerlicht_stream_write(ctxt, dest, src->data, src->length, 0) == src->length);
  }

schemerlicht_memsize schemerlicht_stream_set_pos(schemerlicht_stream* stream, schemerlicht_memsize pos)
  {
  if (pos < 0 || pos > stream->length) 
    {
    return END_OF_STREAM;
    }

  stream->position = pos;
  return stream->position;
  }

schemerlicht_memsize schemerlicht_stream_seek(schemerlicht_stream* stream, schemerlicht_memsize pos)
  {
  return schemerlicht_stream_set_pos(stream, pos);
  }

schemerlicht_memsize schemerlicht_stream_rewind(schemerlicht_stream* stream)
  {
  return schemerlicht_stream_set_pos(stream, 0);
  }