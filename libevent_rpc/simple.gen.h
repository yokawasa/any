/*
 * Automatically generated from ./simple.rpc
 */

#ifndef ___SIMPLE_RPC_
#define ___SIMPLE_RPC_

#include <event-config.h>
#ifdef _EVENT_HAVE_STDINT_H
#include <stdint.h>
#endif
#define EVTAG_HAS(msg, member) ((msg)->member##_set == 1)
#ifdef __GNUC__
#define EVTAG_ASSIGN(msg, member, args...) (*(msg)->base->member##_assign)(msg, ## args)
#define EVTAG_GET(msg, member, args...) (*(msg)->base->member##_get)(msg, ## args)
#else
#define EVTAG_ASSIGN(msg, member, ...) (*(msg)->base->member##_assign)(msg, ## __VA_ARGS__)
#define EVTAG_GET(msg, member, ...) (*(msg)->base->member##_get)(msg, ## __VA_ARGS__)
#endif
#define EVTAG_ADD(msg, member) (*(msg)->base->member##_add)(msg)
#define EVTAG_LEN(msg, member) ((msg)->member##_length)

struct GetUserRequest;
struct GetUserResponse;

/* Tag definition for GetUserRequest */
enum getuserrequest_ {
  GETUSERREQUEST_ID=1,
  GETUSERREQUEST_MAX_TAGS
};

/* Structure declaration for GetUserRequest */
struct GetUserRequest_access_ {
  int (*id_assign)(struct GetUserRequest *, const char *);
  int (*id_get)(struct GetUserRequest *, char * *);
};

struct GetUserRequest {
  struct GetUserRequest_access_ *base;

  char *id_data;

  uint8_t id_set;
};

struct GetUserRequest *GetUserRequest_new(void);
void GetUserRequest_free(struct GetUserRequest *);
void GetUserRequest_clear(struct GetUserRequest *);
void GetUserRequest_marshal(struct evbuffer *, const struct GetUserRequest *);
int GetUserRequest_unmarshal(struct GetUserRequest *, struct evbuffer *);
int GetUserRequest_complete(struct GetUserRequest *);
void evtag_marshal_GetUserRequest(struct evbuffer *, uint32_t, 
    const struct GetUserRequest *);
int evtag_unmarshal_GetUserRequest(struct evbuffer *, uint32_t,
    struct GetUserRequest *);
int GetUserRequest_id_assign(struct GetUserRequest *, const char *);
int GetUserRequest_id_get(struct GetUserRequest *, char * *);
/* --- GetUserRequest done --- */

/* Tag definition for GetUserResponse */
enum getuserresponse_ {
  GETUSERRESPONSE_ERRCODE=1,
  GETUSERRESPONSE_NAME=2,
  GETUSERRESPONSE_EMAIL=3,
  GETUSERRESPONSE_MAX_TAGS
};

/* Structure declaration for GetUserResponse */
struct GetUserResponse_access_ {
  int (*errcode_assign)(struct GetUserResponse *, const uint32_t);
  int (*errcode_get)(struct GetUserResponse *, uint32_t *);
  int (*name_assign)(struct GetUserResponse *, const char *);
  int (*name_get)(struct GetUserResponse *, char * *);
  int (*email_assign)(struct GetUserResponse *, const char *);
  int (*email_get)(struct GetUserResponse *, char * *);
};

struct GetUserResponse {
  struct GetUserResponse_access_ *base;

  uint32_t errcode_data;
  char *name_data;
  char *email_data;

  uint8_t errcode_set;
  uint8_t name_set;
  uint8_t email_set;
};

struct GetUserResponse *GetUserResponse_new(void);
void GetUserResponse_free(struct GetUserResponse *);
void GetUserResponse_clear(struct GetUserResponse *);
void GetUserResponse_marshal(struct evbuffer *, const struct GetUserResponse *);
int GetUserResponse_unmarshal(struct GetUserResponse *, struct evbuffer *);
int GetUserResponse_complete(struct GetUserResponse *);
void evtag_marshal_GetUserResponse(struct evbuffer *, uint32_t, 
    const struct GetUserResponse *);
int evtag_unmarshal_GetUserResponse(struct evbuffer *, uint32_t,
    struct GetUserResponse *);
int GetUserResponse_errcode_assign(struct GetUserResponse *, const uint32_t);
int GetUserResponse_errcode_get(struct GetUserResponse *, uint32_t *);
int GetUserResponse_name_assign(struct GetUserResponse *, const char *);
int GetUserResponse_name_get(struct GetUserResponse *, char * *);
int GetUserResponse_email_assign(struct GetUserResponse *, const char *);
int GetUserResponse_email_get(struct GetUserResponse *, char * *);
/* --- GetUserResponse done --- */

#endif  /* ___SIMPLE_RPC_ */
