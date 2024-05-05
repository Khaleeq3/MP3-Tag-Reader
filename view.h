#ifndef VIEW_H
#define VIEW_H

#include <stdio.h>
#include "type.h"

#define HFRAME_BUFFER   4
#define MAX_HFRAME_SIZE (HFRAME_BUFFER + 1)
#define FLAG_BYTES    3

// User defined data type to store tag details
typedef struct _TagInfo {
    /* Album details */
    char TALB_hframe[MAX_HFRAME_SIZE];
    uint TALB_size;
    char *TALB_content;

    /* Genre details */
    char TCON_hframe[MAX_HFRAME_SIZE];
    uint TCON_size;
    char *TCON_content;

    /* Title details */
    char TIT2_hframe[MAX_HFRAME_SIZE];
    uint TIT2_size;
    char *TIT2_content;

    /* year details */
    char TYER_hframe[MAX_HFRAME_SIZE];
    uint TYER_size;
    char *TYER_content;

    /* composer details */
    char COMM_hframe[MAX_HFRAME_SIZE];
    uint COMM_size;
    char *COMM_content;

    /* Artist details */
    char TPE1_hframe[MAX_HFRAME_SIZE];
    uint TPE1_size;
    char *TPE1_content;

    /* source mp3 file details */
    char *mp3_fname_src;
    FILE *fptr_mp3_src;

    /* new content */
    char *new_content;
} TagInfo;

/* Function prototypes */

/* Checks the operation type */
OperationType check_operation(char *argv[]);

/* Read and validates arguments for view operation */
Status read_and_validate_view_args(char *argv[], TagInfo *tagInfo);

/* Open required file for view operation */
Status open_files_for_view(TagInfo *tagInfo);

/* Reads all the the required header frame details */
Status read_hframe_details(TagInfo *tagInfo);

/* Reads header frame */
Status read_hframe(char *hframe, FILE *fptr_mp3_src);

/* Reads size of content */
Status read_size(uint *size, FILE *fptr_mp3_src);

/* Reads content */
Status read_content(uint size, char **hframe_content, FILE *fptr_mp3_src);

/* Reads title details */
Status read_title(TagInfo *tagInfo);

/* Reads artist details */
Status read_artist(TagInfo *tagInfo);

/* Reads album details */
Status read_album(TagInfo *tagInfo);

/* Reads year detail */
Status read_year(TagInfo *tagInfo);

/* Reads genre details */
Status read_genre(TagInfo *tagInfo);

/* Reads comment */
Status read_comment(TagInfo *tagInfo);

/* Display tag details */
Status display_tag_details(TagInfo *tagInfo);

/* Display help */
Status display_help();

#endif