#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define DEFAULT_PAGE_FRAME_NUM (3)
#define MAX_PAGE_FRAME_NUM     (10)
#define MAX_PAGE_SEQUENCE      (100)
#define PAGE_SEQUENCE_DELIM " "

enum replace_algo{ a_lru, a_fifo, a_optimal };

typedef struct {
    int in_page;       /* page number */
    int in_trace_no;   /* page trace number when the page get the page frame */
    int ref_trace_no;  /* last referenced page trace number */
} page_frame;

void print_frame(int pagetrace, int framenum, page_frame *frames, int hit) {
    int i;
    printf("%2d: ",pagetrace);
    for(i=0; i<framenum;i++)
        if (frames[i].in_page !=-1 )
            printf("%4d", frames[i].in_page);
        else
            printf("    ");
    if (hit)
        printf("    Hit\n");
    else
        printf("    Miss\n");
}

void print_stats(int seqnum, int framenum, int pagefault) {
    int i;
    printf("\nNumber of Page Frames = %d\n", framenum);
    printf("Total Number of References = %d\n", seqnum);
    printf("Number of Hits = %d\n", (seqnum-pagefault));
    printf("Number of Page Fault = %d\n", pagefault);
    printf("HitRatio = %.2f\n",  (double)(seqnum-pagefault)/seqnum );
}

int hit_frame( int page, int framenum, page_frame *frames )
{
    int i;
    for(i=0; i< framenum; i++) {
        if (frames[i].in_page == page )
            return i;
    }
    return -1;
}

int empty_frame(int framenum, page_frame *frames)
{
    int i;
    for(i=0; i< framenum; i++) {
        if (frames[i].in_page == -1)
            return i;
    }
    return -1;
}
void page_tokenize( const char* str, int *num, int *arr)
{
    char *token = NULL;
    char *cp;
    cp = (char *)str;
    int c=0;
    while(1) {
        if ((token = (char*)strtok(cp, PAGE_SEQUENCE_DELIM)) == NULL) {
            break;
        }
        *(arr + c)= atoi(token);
        c++;
        cp = NULL;
    }
    *num = c;
}

int get_LRU_idx(int framenum, page_frame *frames) {
    int i,lru_idx,min_ref_trace_no;
    if (framenum<1)
        return 0;
    lru_idx=0;
    min_ref_trace_no=frames[0].ref_trace_no;
    for(i=1; i< framenum; i++) {
        if (min_ref_trace_no > frames[i].ref_trace_no ) {
            min_ref_trace_no = frames[i].ref_trace_no;
            lru_idx =i;
        }
    }
    return lru_idx;
}

int get_FIFO_idx(int framenum, page_frame *frames) {
    int i,fifo_idx,min_in_trace_no;
    if (framenum<1)
        return 0;
    fifo_idx=0;
    min_in_trace_no=frames[0].in_trace_no;
    for(i=1; i< framenum; i++) {
        if (min_in_trace_no > frames[i].in_trace_no ) {
            min_in_trace_no = frames[i].in_trace_no;
            fifo_idx =i;
        }
    }
    return fifo_idx;
}

int get_OPTIMAL_idx(int framenum, page_frame *frames,
                        int future_seq_offset, int seqnum, int *pageseqs )
{
    int i,f,c,optimal_idx,max_future_not_used_period;
    if (framenum<1 || future_seq_offset >= (seqnum-1) )
        return 0;
    optimal_idx=0;
    max_future_not_used_period =0;
    for(i=0; i< framenum; i++) {
        for( f=future_seq_offset,c=0;
                frames[i].in_page!=pageseqs[f] && f<seqnum; f++,c++);
        if (c > max_future_not_used_period ) {
            max_future_not_used_period =c;
            optimal_idx =i;
        }
//printf("frame i:%d frame in_trace_no:%d max_future_not_used_period:%d\n",
//   i,frames[i].in_page, max_future_not_used_period);
    }
    return optimal_idx;
}

static void usage(const char* c)
{
  fprintf(stderr,
      "Usage: %s [options] \n"
      "Options are:\n"
      "    -a algorithm    Algorithm to used: either LRU, FIFO or OPTIMAL\n"
      "    -p sequences    Page # sequences in which the pages are accessed by some program\n"
      "                    in the order. max entry num is %d.  (space between each)\n"
      "    -f framenum     The number of page frames. max frame num is %d.\n"
      "    -h              Display usage information (this message)\n"
      "\n"
      "Examples: \n"
      "       %s -a LRU -p \"0 1 2 3 2 1 4 3 6 0 9\" -f 3 \n"
      "\n",
      c, MAX_PAGE_SEQUENCE, MAX_PAGE_FRAME_NUM, c);
    exit(1);
}

int main (int argc, char** argv) {
    int framenum,seqnum,pagefault,replace_idx;
    int pageseqs[MAX_PAGE_SEQUENCE], c,i,j;
    int hit_frame_idx, empty_frame_idx;
    char pageseqs_s[MAX_PAGE_SEQUENCE*3 + 1];
    page_frame *frames;
    framenum = DEFAULT_PAGE_FRAME_NUM;
    seqnum = pagefault = 0;
    enum replace_algo algo = a_lru;

    while ((c = getopt(argc, argv, "a:p:f:h")) != -1) {
        switch (c) {
        case 'p':
            if (!optarg) {
                fprintf(stderr, "invalid -s option\n");
                exit(1);
            }
            strncpy(pageseqs_s, optarg, MAX_PAGE_SEQUENCE*3 + 1);
            break;
        case 'f':
            if (sscanf(optarg, "%u", &framenum) != 1) {
                fprintf(stderr, "invalid -f option\n");
                exit(1);
            }
            break;
        case 'a':
            if (strcmp(optarg, "LRU") == 0 || strcmp(optarg, "lru") == 0)
                algo = a_lru;
            else if (strcmp(optarg, "FIFO") == 0 || strcmp(optarg, "fifo") == 0)
                algo = a_fifo;
            else if (strcmp(optarg, "OPTIMAL") == 0 || strcmp(optarg, "optimal") == 0)
                algo = a_optimal;
            else {
                fprintf(stderr, "invalid -a option\n");
                exit(1);
            }
            break;
        case 'h':
        default:
            usage(argv[0]);
            break;
        }
    }
    if (pageseqs_s == 0||*pageseqs_s == 0)
        usage(argv[0]);

    /* Initialize pageseqs, array of int */
    for (i=0;i<MAX_PAGE_SEQUENCE;i++) pageseqs[i] = -1;

    /* Initialize frames, array of page_frame struct */
    frames = (page_frame *)malloc( sizeof(page_frame) * framenum );
    for(i=0; i<framenum; i++) {
        frames[i].in_page=-1;
        frames[i].in_trace_no=-1;
        frames[i].ref_trace_no=-1;
    }
    page_tokenize(pageseqs_s, &seqnum, pageseqs);

    /* Do page access in the order of pageseqs */
    for(i=0; pageseqs[i] !=-1 && i<seqnum; i++) {
        hit_frame_idx =-1;
        empty_frame_idx=-1;
        /* Check if the page exists in frames. */
        /* If exists, replace previous ref_trace_no with new one. */
        hit_frame_idx = hit_frame(pageseqs[i], framenum, frames);
        if(hit_frame_idx > -1 ) {
            frames[hit_frame_idx].ref_trace_no = i;
            goto printframe;
        }
        /* Page fault occurs */
        pagefault++;
        /* Check if empty frame exists in frames. */
        /* If exists, use the empty frame for the page (page-in) */
        empty_frame_idx = empty_frame(framenum, frames);
        if (empty_frame_idx > -1) {
            frames[empty_frame_idx].in_page = pageseqs[i];
            frames[empty_frame_idx].in_trace_no = i;
            frames[empty_frame_idx].ref_trace_no = i;
            goto printframe;
        }
        /* Do Page replacement */
        switch(algo) {
        case a_lru:
            replace_idx = get_LRU_idx(framenum, frames);
            break;
        case a_fifo:
            replace_idx = get_FIFO_idx(framenum, frames);
            break;
        case a_optimal:
            replace_idx = get_OPTIMAL_idx(framenum, frames, i+1, seqnum, pageseqs);
            break;
        }
        frames[replace_idx].in_page = pageseqs[i];
        frames[replace_idx].in_trace_no = i;
        frames[replace_idx].ref_trace_no = i;
printframe:
        /* Printing the content of the frame */
        print_frame(pageseqs[i], framenum, frames, (hit_frame_idx > -1)?1:0 );
    }
    print_stats(seqnum, framenum, pagefault);
    return 0;
}
