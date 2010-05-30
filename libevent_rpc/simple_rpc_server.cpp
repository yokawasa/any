/*
 * simple_rpc_server.cpp
 *
 * Libevent RPC framework Sample Program - simple_rpc_server
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
GetUserCallback(EVRPC_STRUCT(GetUser)* rpc, void *arg)
{
    struct GetUserRequest* req = rpc->request;
    struct GetUserResponse* res = rpc->reply;

    /* get request info */
    char *id;
    if(!EVTAG_HAS(req, id)) {
        fprintf(stderr, "failed to get request info\n");
        EVTAG_ASSIGN(res, errcode, -1);
    }
    EVTAG_GET(req, id, &id);
    fprintf(stdout, "REQUEST id = %s\n", id);

    /* do somthing due to request info */
    /* set response info */
    EVTAG_ASSIGN(res, errcode, 0);
    EVTAG_ASSIGN(res, name, "Fooo Baaar");
    EVTAG_ASSIGN(res, email, "baz@foo.bar");

    /* no reply to the RPC */
    EVRPC_REQUEST_DONE(rpc);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stdout, "%s <server> <port>\n", argv[0]);
        return 1;
    }
    char *svr_addr = argv[1];
    short svr_port = atoi(argv[2]);

    struct evhttp *ev_http = NULL;
    struct evrpc_base *rpc_base = NULL;

    /* initialize base event mechanism */
    event_init();
    ev_http = evhttp_start(svr_addr, svr_port);
    /* create a base for the RPC protocol */
    rpc_base = evrpc_init(ev_http);

    /* register all of the handlers for all RPCs */
    EVRPC_REGISTER(rpc_base, GetUser, GetUserRequest, GetUserResponse, GetUserCallback, NULL);

    /* run the dispather */
    event_dispatch();

    EVRPC_UNREGISTER(rpc_base, GetUser);
    evrpc_free(rpc_base);
    evhttp_free(ev_http);
    return 0;
}
