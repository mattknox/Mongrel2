/**
 *
 * Copyright (c) 2010, Zed A. Shaw and Mongrel2 Project Contributors.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 * 
 *     * Neither the name of the Mongrel2 Project, Zed A. Shaw, nor the names
 *       of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <task/task.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <dbg.h>
#include <proxy.h>
#include <assert.h>
#include <stdlib.h>
#include <mem/halloc.h>

// TODO: make these configurable
enum
{
    STACK = 32768,
    BUFFER_SIZE = 2 * 1024 
};


void rwtask(void*);


ProxyConnect *ProxyConnect_create(int client_fd, char *buffer, size_t size)
{
    ProxyConnect *conn = h_malloc(sizeof(ProxyConnect));
    check_mem(conn);
    conn->client_fd = client_fd;
    conn->proxy_fd = 0;
    conn->buffer = buffer;
    conn->size = size;

    return conn;
error:
    return NULL;
}

void ProxyConnect_destroy(ProxyConnect *conn) 
{
    if(conn) {
        h_free(conn);
    }
}


void Proxy_destroy(Proxy *proxy)
{
    if(proxy) {
        if(proxy->server) bdestroy(proxy->server);
        h_free(proxy);
    }
}

Proxy *Proxy_create(bstring server, int port)
{
    Proxy *proxy = h_calloc(sizeof(Proxy), 1);
    check_mem(proxy);
    
    proxy->server = server;
    proxy->port = port;

    return proxy;

error:
    Proxy_destroy(proxy);
    return NULL;
}

ProxyConnect *Proxy_connect_backend(Proxy *proxy, int fd)
{
    ProxyConnect *to_proxy = NULL;

    to_proxy = ProxyConnect_create(fd, NULL, 0);
    to_proxy->waiter = h_calloc(sizeof(Rendez), 1);
    hattach(to_proxy, to_proxy->waiter);

    check(to_proxy, "Could not create the connection to backend %s:%d",
            bdata(proxy->server), proxy->port);

    debug("Connecting to %s:%d", bdata(proxy->server), proxy->port);

    // TODO: create release style macros that compile these away taskstates
    taskstate("connecting");

    to_proxy->proxy_fd = netdial(TCP, bdata(proxy->server), proxy->port);
    check(to_proxy->proxy_fd >= 0, "Failed to connect to %s:%d", bdata(proxy->server), proxy->port);

    debug("Proxy connected to %s:%d", bdata(proxy->server), proxy->port);

    return to_proxy;
error:

    ProxyConnect_destroy(to_proxy);
    return NULL;
}


ProxyConnect *Proxy_sync_to_listener(ProxyConnect *to_proxy)
{
    ProxyConnect *to_listener = NULL;

    to_listener = ProxyConnect_create(to_proxy->client_fd, 
            h_malloc(BUFFER_SIZE+1), BUFFER_SIZE);

    check(to_listener, "Could not create listener side proxy connect.");

    // halloc will make sure the rendez goes away when the to_listener does
    to_listener->waiter = to_proxy->waiter;
    to_listener->proxy_fd = to_proxy->proxy_fd;

    // kick off one side as a task to do its thing on the proxy
    taskcreate(rwtask, (void *)to_listener, STACK);

    return to_listener;

error:
    ProxyConnect_destroy(to_listener);
    return NULL;
}


void
rwtask(void *v)
{
    ProxyConnect *to_listener = (ProxyConnect *)v;
    int rc = 0;
    int nread = 0;

    do {
        rc = fdsend(to_listener->client_fd, to_listener->buffer, nread);

        if(rc != nread) {
            break;
        }
        
    } while((nread = fdrecv(to_listener->proxy_fd, to_listener->buffer, to_listener->size)) > 0);

    taskbarrier(to_listener->waiter);

    fdclose(to_listener->proxy_fd);
    ProxyConnect_destroy(to_listener);
}

