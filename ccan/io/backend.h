/* Licensed under LGPLv2.1+ - see LICENSE file for details */
#ifndef CCAN_IO_BACKEND_H
#define CCAN_IO_BACKEND_H
#include <stdbool.h>
#include <poll.h>
#include "io_plan.h"

struct fd {
	int fd;
	bool listener;
	size_t backend_info;
};

/* Listeners create connections. */
struct io_listener {
	struct fd fd;

	const tal_t *ctx;

	/* These are for connections we create. */
	struct io_plan *(*init)(struct io_conn *conn, void *arg);
	void *arg;
};

/* One connection per client. */
struct io_conn {
	struct fd fd;

	/* always or closing list. */
	struct io_conn *list;

	void (*finish)(struct io_conn *, void *arg);
	void *finish_arg;

	struct io_plan plan[2];
};

extern void *io_loop_return;

bool add_listener(struct io_listener *l);
bool add_conn(struct io_conn *c);
bool add_duplex(struct io_conn *c);
void del_listener(struct io_listener *l);
void backend_new_closing(struct io_conn *conn);
void backend_new_always(struct io_conn *conn);
void backend_new_plan(struct io_conn *conn);

void backend_plan_done(struct io_conn *conn);

void backend_wake(const void *wait);
void backend_del_conn(struct io_conn *conn);

void io_ready(struct io_conn *conn, int pollflags);
void io_do_always(struct io_conn *conn);
void io_do_wakeup(struct io_conn *conn, struct io_plan *plan);
void *do_io_loop(struct io_conn **ready);
#endif /* CCAN_IO_BACKEND_H */
