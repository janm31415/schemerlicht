#ifndef SCHEMERLICHT_STREAM_H
#define SCHEMERLICHT_STREAM_H

#include "schemerlicht.h"
#include "smemory.h"

/* end of stream */
#define END_OF_STREAM (cast(schemerlicht_memsize, -1))	

/*
* Struct that represents a stream of bytes, that you can read
* from and write to.
*
* Writing is guaranteed to succeed as long as there is enough
* memory left on the machine.
* Reading, in turn, can happen to only be executed partly,
* which depends on how much reading content there is in the stream.
*/
typedef struct schemerlicht_stream
  {
  char* data; // The data in memory
  schemerlicht_memsize position; // The current read/write position
  schemerlicht_memsize length; // The number of bytes written to the stream
  schemerlicht_memsize capacity; // The actual number of bytes allocated
  } schemerlicht_stream;

/*
* Initializes stream and reserves capac number of bytes.
*/
void schemerlicht_stream_init(schemerlicht_context* ctxt, schemerlicht_stream* stream, schemerlicht_memsize capac);

/*
* Clears all resources used by a stream.
* Make sure to call this on every stream when you are done with it.
*/
void schemerlicht_stream_close(schemerlicht_context* ctxt, schemerlicht_stream* stream);

/*
* Reserves a given capacity for the stream
*/
void schemerlicht_stream_reserve(schemerlicht_context* ctxt, schemerlicht_stream* stream, schemerlicht_memsize size);

/*
* Writes count bytes from buffer to stream at the current position.
* offset determines the offset of the input buffer.
* Returns the total number of bytes written to stream.
*
* The difference to stream_insert is, that this function overwrites the data
* of the stream it collides with from position to end.
*/
schemerlicht_memsize schemerlicht_stream_write(schemerlicht_context* ctxt, schemerlicht_stream* stream, const char* buffer, schemerlicht_memsize count, schemerlicht_memsize offset);

/*
* Writes a byte to stream.
*/
void schemerlicht_stream_write_byte(schemerlicht_context* ctxt, schemerlicht_stream* stream, char value);

/*
* Writes a string to stream.
*/
schemerlicht_memsize schemerlicht_stream_write_str(schemerlicht_context* ctxt, schemerlicht_stream* stream, const char* str);

/*
* Inserts count bytes from buffer into the stream at it's current position.
* offset determines the offset of the input buffer.
* Returns the total number of bytes written to stream.
*
* The difference to stream_write is, that this function does not overwrite the
* data is collides with, instead it actually inserts it into memory, which
* is obviously more expensive.
*/
schemerlicht_memsize schemerlicht_stream_insert(schemerlicht_context* ctxt, schemerlicht_stream* stream, const char* buffer, schemerlicht_memsize count, schemerlicht_memsize offset);

/*
* Inserts a byte into stream.
*/
void schemerlicht_stream_insert_byte(schemerlicht_context* ctxt, schemerlicht_stream* stream, char value);

/*
* Inserts a string into stream.
*/
schemerlicht_memsize schemerlicht_stream_insert_str(schemerlicht_context* ctxt, schemerlicht_stream* stream, const char* str);

/*
* Reads count bytes from stream into buffer.
* offset determines the offset of the output buffer.
* Returns the actual number of bytes read or END_OF_STREAM if the end was reached.
*/
schemerlicht_memsize schemerlicht_stream_read(schemerlicht_stream* stream, char* buffer, schemerlicht_memsize count, schemerlicht_memsize offset);

/*
* Reads a byte from stream.
* Returns the integer value of the byte or END_OF_STREAM
* if the end was reached.
*/
schemerlicht_memsize schemerlicht_stream_read_byte(schemerlicht_stream* stream);

/*
* Sets all bytes, position and the length to zero.
* The actual memory is kept allocated. Capacity stays the same.
* If your intention is to free the memory, use stream_close.
*/
void schemerlicht_stream_clear(schemerlicht_stream* stream);

/*
* Copies the whole content of one stream to another at the
* destinations position.
* Returns 1 if success or 0 if failure.
*/
schemerlicht_memsize schemerlicht_stream_copy(schemerlicht_context* ctxt, schemerlicht_stream* src, schemerlicht_stream* dest);

/*
* Sets the position of stream.
* Returns the new position of the stream or END_OF_STREAM on failure.
*/
schemerlicht_memsize schemerlicht_stream_set_pos(schemerlicht_stream* stream, schemerlicht_memsize pos);

/*
* Sets the position of stream.
* Returns the new position of the stream or END_OF_STREAM on failure.
*/
schemerlicht_memsize schemerlicht_stream_seek(schemerlicht_stream* stream, schemerlicht_memsize pos);

/*
* Sets the position of stream to 0.
* Returns the new position of the stream or END_OF_STREAM on failure.
*/
schemerlicht_memsize schemerlicht_stream_rewind(schemerlicht_stream* stream);

#endif //SCHEMERLICHT_STREAM_H