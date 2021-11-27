

#include "csp_port.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <csp/csp.h>
#include <csp/arch/csp_queue.h>
#include <csp_autoconfig.h>

#include "csp_conn.h"

typedef enum {
	PORT_CLOSED = 0,
	PORT_OPEN = 1,
} csp_port_state_t;

typedef struct {
	csp_port_state_t state;
	csp_socket_t * socket;  // New connections are added to this socket's conn queue
} csp_port_t;

/* We rely on the .bss section to clear this, so there is no csp_port_init() function */
static csp_port_t ports[CSP_PORT_MAX_BIND + 2] = {0};

csp_socket_t * csp_port_get_socket(unsigned int port) {

	if (port > CSP_PORT_MAX_BIND) {
		return NULL;
	}

	/* Match dport to socket or local "catch all" port number */
	if (ports[port].state == PORT_OPEN) {
		return ports[port].socket;
	}

	if (ports[CSP_PORT_MAX_BIND + 1].state == PORT_OPEN) {
		return ports[CSP_PORT_MAX_BIND + 1].socket;
	}

	return NULL;
}

int csp_listen(csp_socket_t * socket, size_t backlog) {
	return CSP_ERR_NONE;
}

int csp_bind(csp_socket_t * socket, uint8_t port) {

	if (socket == NULL)
		return CSP_ERR_INVAL;

	if (port == CSP_ANY) {
		port = CSP_PORT_MAX_BIND + 1;
	} else if (port > CSP_PORT_MAX_BIND) {
		csp_dbg_errno = CSP_DBG_CONN_ERR_INVALID_BIND_PORT;
		return CSP_ERR_INVAL;
	}

	if (ports[port].state != PORT_CLOSED) {
		csp_dbg_errno = CSP_DBG_CONN_ERR_PORT_ALREADY_IN_USE;
		return CSP_ERR_USED;
	}

	/* Save listener */
	ports[port].socket = socket;
	ports[port].state = PORT_OPEN;

	return CSP_ERR_NONE;
}
