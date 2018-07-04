#include "ringbuffer.h"
#include "trace.h"
#include "utils.h"

/* WARNINGI: Since console output is internally using this ringbuffer to implement
 * buffered writes, we cannot use any TRACE_*() or printf() style functions here,
 * as it would create infinite recursion! */

void rbuf_reset(volatile ringbuf * rb)
{
	unsigned long state;

	local_irq_save(state);
	rb->ird = 0;
	rb->iwr = 0;
	local_irq_restore(state);
}

uint8_t rbuf_read(volatile ringbuf * rb)
{
	unsigned long state;
	uint8_t val;

	local_irq_save(state);
	val = rb->buf[rb->ird];
	rb->ird = (rb->ird + 1) % RING_BUFLEN;
	local_irq_restore(state);

	return val;
}

uint8_t rbuf_peek(volatile ringbuf * rb)
{
	return rb->buf[rb->ird];
}

bool rbuf_is_empty(volatile ringbuf * rb)
{
	return rb->ird == rb->iwr;
}

static bool __rbuf_is_full(volatile ringbuf * rb)
{
	return rb->ird == (rb->iwr + 1) % RING_BUFLEN;
}

bool rbuf_is_full(volatile ringbuf * rb)
{
	unsigned long state;
	bool rc;

	local_irq_save(state);
	rc = rb->ird == (rb->iwr + 1) % RING_BUFLEN;
	local_irq_restore(state);

	return rc;
}

int rbuf_write(volatile ringbuf * rb, uint8_t item)
{
	unsigned long state;

	local_irq_save(state);
	if (!__rbuf_is_full(rb)) {
		rb->buf[rb->iwr] = item;
		rb->iwr = (rb->iwr + 1) % RING_BUFLEN;
		local_irq_restore(state);
		return 0;
	} else {
		local_irq_restore(state);
		return -1;
	}
}


