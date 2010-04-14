#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <fstream>
#include <curl/curl.h>
using std::string;
using std::vector;
using std::fstream;
using std::ios;

/*
*
* [compile]
* g++ multi_fetch.cpp -o multi_fetch -I/usr/include -L/usr/lib -lcurl
*
* [execute]
* ./multi_fetch -f urls.txt -p
*
*/

#define MAX_LINE_BUF (512)
static void
 seturls (const string& file, vector<string> & urls )
{
    char buf[MAX_LINE_BUF + 1];
    string l;
    fstream fs;
    fs.open (file.c_str(), ios::in );
    if (!fs.is_open()) {
        fprintf( stderr, "ERROR: opening file %s\n", file.c_str());
        _exit(1);
    }
    while( !( fs.eof() || fs.bad() || fs.fail() ) ) {
        memset(buf, 0, MAX_LINE_BUF);
        fs.getline(buf, MAX_LINE_BUF);
        l = buf;
        if(!l.empty()) urls.push_back(l);
    }
//for(vector<string>::iterator it=urls.begin();it!=urls.end(); ++it)
//      printf("dump url:%s\n", (*it).c_str());
    fs.close();
}

static void
 usage(const char* c)
{
    fprintf(stderr,
      "Usage: %s <options> \n"
      "Options: -f file  URL list file\n"
      "         -p       HTTP pipelining mode (optional) \n", c);
    _exit(1);
}

int
 main(int argc, char **argv)
{
    string urlfile;
    bool pipelining = false;
    vector<string> urls;
    vector<CURL*> c_handles;
    vector<CURL*>::iterator cit;
    CURLM *m_handle;
    CURLMsg *msg;
    int c, still_running, i, msgs_left;
    while ((c = getopt(argc, argv, "f:p")) != -1) {
        switch (c) {
        case 'f':
            if (!optarg) {
                fprintf(stderr, "invalid -f option\n");
                _exit(1);
            }
            urlfile = optarg;
            break;
        case 'p':
            pipelining =true;
            break;
        }
    }
    if (urlfile.empty())
        usage(argv[0]);

    seturls(urlfile, urls);
    if (urls.size()<1) usage(argv[0]);

    /* init a curl handle */
    for(i=0; i< urls.size(); i++) {
        CURL * _handle = curl_easy_init();
        c_handles.push_back(_handle);
    }
    /* init a multi stack */
    m_handle = curl_multi_init();
    i =0;
    for(vector<string>::iterator it=urls.begin();
            it!=urls.end(); ++it) {
        curl_easy_setopt(c_handles[i], CURLOPT_URL, (*it).c_str());
        curl_easy_setopt(c_handles[i], CURLOPT_TCP_NODELAY, 1L);
        curl_easy_setopt(c_handles[i], CURLOPT_VERBOSE, 1L);
        curl_multi_add_handle(m_handle, c_handles[i]);
        i++;
    }
    if (pipelining)
        curl_multi_setopt(m_handle, CURLMOPT_PIPELINING, 1L);

    while(CURLM_CALL_MULTI_PERFORM ==
            curl_multi_perform(m_handle, &still_running));

    while(still_running) {
        struct timeval timeout;
        int rc, maxfd;
        fd_set fdread, fdwrite, fdexcep;
        FD_ZERO(&fdread);
        FD_ZERO(&fdwrite);
        FD_ZERO(&fdexcep);

        /* set timeout */
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        curl_multi_fdset(m_handle, &fdread, &fdwrite, &fdexcep, &maxfd);
        rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);
        switch(rc) {
        case -1: /* select error */
        case 0:  /* timeout */
            break;
        default:
            /* there's still data to read or write */
            while(CURLM_CALL_MULTI_PERFORM ==
                    curl_multi_perform(m_handle, &still_running));
            break;
        }
    }
    while ((msg = curl_multi_info_read(m_handle, &msgs_left))) {
        if (msg->msg == CURLMSG_DONE) {
            int found = 0;
            /* check out which curl handle to be done */
            for(cit=c_handles.begin(); (!found && cit!=c_handles.end()); ++cit)
                found = (msg->easy_handle == *cit);
            printf("HTTP transfer completed with status %d\n", msg->data.result);
        }
    }
    curl_multi_cleanup(m_handle);
    for(cit=c_handles.begin(); cit!=c_handles.end(); ++cit)
        curl_easy_cleanup(*cit);
    return 0;
}
