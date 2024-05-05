#ifndef EDIT_H
#define EDIT_H
#include "view.h"

/* function prototypes for editing */

/* Read and validates the arguments for editing */
Status read_and_validate_edit_args(char *argv[], TagInfo *tagInfo);

/* Openes the files required for editing */
Status open_files_for_editing(TagInfo *tagInfo);

/* Edits the tag details */
Status edit_hframe_details(char option, TagInfo *tagInfo);

/* Edit title content */
Status edit_title(FILE *fptr_mp3_src, char *new_content);

/* Edit artist content */
Status edit_artist(FILE *fptr_mp3_src, char *new_content);

/* Edit album content */
Status edit_album(FILE *fptr_mp3_src, char *new_content);

/* Edit year content */
Status edit_year(FILE *fptr_mp3_src, char *new_content);

/* Edit genre content */
Status edit_genre(FILE *fptr_mp3_src, char *new_content);

/* Edit comment content */
Status edit_comment(FILE *fptr_mp3_src, char *new_content);

/* Checks the capacity to store new content */
Status check_capacity(uint available_size, uint new_size);

/* Edit header frame content */
Status edit_content(FILE *fptr_mp3_src, char *new_content, uint size);

#endif