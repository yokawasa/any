/*
 * simple_rpc_client.cpp
 *
 * Libevent RPC framework Sample Program - simple_rpc_client
 *
 * Copyright (C) 2010 Yoichi Kawasaki All rights reserved.
 * yokawasa at gmail dot com
 * yk55.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef __cplusplus
extern "C" {
#endif
#include "simple.gen.h"
#ifdef __cplusplus
}
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/queue.h>
#include "event.h"
#include "evhttp.h"
#include "evrpc.h"

/* registe RPC : GetUser */
EVRPC_HEADER(GetUser, GetUserRequest, GetUserResponse);
/* generate RPC code : GetUser */
EVRPC_GENERATE(GetUser, GetUserRequest, GetUserResponse);

static void
 GetUserCallback(struct evrpc_status *status,
    struct GetUserRequest *req, struct GetUserResponse *res, void *arg)
{
    uint32_t errcode;
    char *name,*email;
    /* check return status */
    if (status->error != EVRPC_STATUS_ERR_NONE) {
        goto exitloop;
    }
    /* get response info */
    EVTAG_GET(res, errcode, &errcode);
    fprintf(stdout, "RESPONSE errcode=%d\n", errcode);
    if(EVTAG_HAS(res, name) ) {
        EVTAG_GET(res, name, &name);
        fprintf(stdout, "RESPONSE name=%s\n", name);
    }
    if(EVTAG_HAS(res, email) ) {
        EVTAG_GET(res, email, &email);
        fprintf(stdout, "RESPONSE email=%s\n", email);
    }
exitloop:
    event_loopexit(NULL);
}

static struct evrpc_pool *
 get_rpc_pool(struct event_base *base, const char *svr_addr, short svr_port)
{
    struct evhttp_connection *ev_conn;
    struct evrpc_pool *rpc_pool;
    rpc_pool = evrpc_pool_new(base);
    if (!rpc_pool) {
        fprintf(stderr, "failed to get new rpc pool\n");
        return NULL;
    }
    // create a connection to RPC server
    ev_conn = evhttp_connection_new(svr_addr, svr_port);
    if (!ev_conn) {
        fprintf(stderr, "failed to get new evhttp connection: %s:%d\n",
            svr_addr, svr_port);
        return NULL;
    }
    // add request connection in the pool
    evrpc_pool_add_connection(rpc_pool, ev_conn);
    return (rpc_pool);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stdout, "%s <server> <port>\n", argv[0]);
        return 1;
    }
    char *svr_addr = argv[1];
    short svr_port = atoi(argv[2]);
    struct event_base *ev_base = NULL;
    struct evhttp *ev_http = NULL;
    struct evrpc_base *rpc_base = NULL;
    struct evrpc_pool *rpc_pool = NULL;
    struct GetUserRequest *req;
    struct GetUserResponse *res;

    /* initialize base event mechanism */
    ev_base = event_init();
    /* initialize http internal buffer */
    ev_http = evhttp_new(ev_base);
    /* create a base for the RPC protocol */
    rpc_base = evrpc_init(ev_http);

    rpc_pool = get_rpc_pool(ev_base, svr_addr, svr_port);
    if(!rpc_pool) {
        return -1;
    }
    /* set up request data */
    req = GetUserRequest_new();
    EVTAG_ASSIGN(req, id, "foobar");

    /* create a response structure */
    res = GetUserResponse_new();
    /* set request, response, callback func */
    EVRPC_MAKE_REQUEST(GetUser, rpc_pool, req, res, GetUserCallback, NULL);
    /* process request and response */
    event_dispatch();

    GetUserRequest_free(req);
    GetUserResponse_free(res);

    evrpc_pool_free(rpc_pool);
    evrpc_free(rpc_base);
    evhttp_free(ev_http);
    return 0;
}
