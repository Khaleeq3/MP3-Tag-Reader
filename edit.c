#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "view.h"
#include "edit.h"

/* Read and validates the arguments for editing 
 * Input: Command line arguments and address of variable which holds the tag details 
 * Output: File name and new content to be added 
 * Description: reads and Validates the arguments for editing
 * Return value: e_success and e_failure
 */
Status read_and_validate_edit_args(char *argv[], TagInfo *tagInfo)
{
    if(argv[2] == NULL || (strcmp(argv[2], "-t") && strcmp(argv[2], "-a") && strcmp(argv[2], "-A") && strcmp(argv[2], "-y") && strcmp(argv[2], "-c") && strcmp(argv[2], "-g")) || argv[3] == NULL || argv[4] == NULL)
    {
        puts("-----------SELECT EDIT DETAILS-----------");
        puts("eg: ./a.out -e -t/-a/-A/-m/-y/-c \"value\" <.mp3 file name>");
        return e_failure;
    }
    
    // Temporary variable to check the extension type
    char *str;

    if((str = strstr(argv[4], ".")) == NULL || strcmp(str, ".mp3"))
    {
        printf("ERROR: Unsupported format\n");
        printf("USAGE:\n");
        printf("To view please pass like: ./a.out -v <.mp3 file name>\n");
        printf("To edit please pass like: ./a.out -e -t/-a/-A/-m/-y/-c <.mp3 file name>\n");
        printf("To help please pass like: ./a.out --help\n");
        return e_failure;
    }
    else
    {
        tagInfo->new_content = argv[3];
        tagInfo->mp3_fname_src = argv[4];
        return e_success;
    }
}

/* Opens the rquired files for editing
 * Input: source file name
 * Output: File pointer to source file
 * Description: Opens the required for editing
 * Return value: e_success and e_failure
 */
Status open_files_for_editing(TagInfo *tagInfo)
{
    tagInfo->fptr_mp3_src = fopen(tagInfo->mp3_fname_src, "r+");
    
    if(tagInfo->fptr_mp3_src == NULL)
    {
        perror("fopen");
        printf("ERROR: Unable to open %s\n", tagInfo->mp3_fname_src);
        return e_failure;
    }
    
    // Read tag idendifier
    char tag_identifier[4] = {0};
    if(fread(tag_identifier, sizeof(char), 3, tagInfo->fptr_mp3_src) != 3) return v_failure;

    // Error handling for tag identifier
    if(strcmp(tag_identifier, "ID3"))
    {
        printf("ERROR: Unsupported file\n");
        printf("ERROR: ID3 tag not found\n");
        return e_failure;
    }

    // Read version
    unsigned short version;
    if(fread(&version, sizeof(unsigned short), 1, tagInfo->fptr_mp3_src) != 1) return v_failure;

    // Error handling for version
    version = version >> 8 | version << 8;
    if(version != 0x0300)
    {
        printf("ERROR: Unsupported version\n");
        printf("ERROR: Version should be ID3v2.3\n");
        return e_failure;
    }

    // file is valid and opened successfully
    return e_success;
}


/* Edit tag details
 * Input: selected edit option and address of variable which holds the tag details
 * Output: File with edited tag content
 * Description: modify the existing tag details with new content
 * Return value: e_success and e_failure
 */
Status edit_hframe_details(char option, TagInfo *tagInfo)
{
    puts("-----------------------SELECT EDIT DETAILS-----------------------");

    // Error handling for Opening required file for editing
    if(open_files_for_editing(tagInfo) == e_failure)
    {
        printf("ERROR: open_files_for_editing function failed\n");
        return e_failure;
    }

    switch (option)
    {
    
    // Edit title tag content
    case 't':
        puts("-------------------Selected Title Change Option-------------------");
        printf("%15s %5s %10s\n", "Title", ":", tagInfo->new_content);

        if(edit_title(tagInfo->fptr_mp3_src, tagInfo->new_content) == e_failure)
        {
            printf("ERROR: edit_title function failed\n");
            return e_failure;
        }

        printf("------------------Title changed successfully-------------------\n");
        break;
    
    // Edit Artist content
    case 'a':
        puts("-------------------Selected Artist Change Option-------------------");
        printf("%15s %5s %10s\n", "Artist", ":", tagInfo->new_content);

        if(edit_artist(tagInfo->fptr_mp3_src, tagInfo->new_content) == e_failure)
        {
            printf("ERROR: edit_artist function failed\n");
            return e_failure;
        }
        
        printf("------------------Artist changed successfully-------------------\n");
        
        break;
    
    // Edit Album content
    case 'A':
        puts("-------------------Selected Title Album Option-------------------");
        printf("%15s %5s %10s\n", "Album", ":", tagInfo->new_content);

        if(edit_album(tagInfo->fptr_mp3_src, tagInfo->new_content) == e_failure)
        {
            printf("ERROR: edit_album function failed\n");
            return e_failure;
        }
        
        printf("------------------Album changed successfully-------------------\n");
        
        break;

    // Edit year content
    case 'y':
        puts("-------------------Selected Year Change Option-------------------");
        printf("%15s %5s %10s\n", "Year", ":", tagInfo->new_content);

        if(edit_year(tagInfo->fptr_mp3_src, tagInfo->new_content) == e_failure)
        {
            printf("ERROR: edit_year function failed\n");
            return e_failure;
        }

        printf("------------------year changed successfully-------------------\n");
    
        break;
    
    // Edit Genre content
    case 'g':
        puts("-------------------Selected Genre Change Option-------------------");
        printf("%15s %5s %10s\n", "Genre", ":", tagInfo->new_content);

        if(edit_genre(tagInfo->fptr_mp3_src, tagInfo->new_content) == e_failure)
        {
            printf("ERROR: edit_genre function failed\n");
            return e_failure;
        }

        printf("------------------Genre changed successfully-------------------\n");
        
        break;

    // Edit composer content
    case 'c':
        puts("-------------------Selected Composer Change Option-------------------");
        printf("%15s %5s %10s\n", "Composer", ":", tagInfo->new_content);

        if(edit_comment(tagInfo->fptr_mp3_src, tagInfo->new_content) == e_failure)
        {
            printf("ERROR: edit_comment function failed\n");
            return e_failure;
        }

        printf("------------------Composer changed successfully-------------------\n");
        
        break;
    
    // Error message for invalid option
    default:
        printf("ERROR: Invalid argument\n");
        printf("USAGE:\n");
        printf("To view please pass like: ./a.out -v <.mp3 file name>\n");
        printf("To edit please pass like: ./a.out -e -t/-a/-A/-m/-y/-c <.mp3 file name>\n");
        printf("To help please pass like: ./a.out --help\n");
        return e_failure;
    }

    // Close source file
    fclose(tagInfo->fptr_mp3_src);
    return e_success;
}

/* Edit title details
 * Input: Source file pointer and new content
 * Output: File with modified title
 * Description: edits the title content
 * Return value: e_success and e_failure
 */
Status edit_title(FILE *fptr_mp3_src, char *new_content)
{
    // Skip 10 header bytes
    if (fseek(fptr_mp3_src, 10, SEEK_SET) != 0) return e_failure;

    char hframe[MAX_HFRAME_SIZE];
    uint size;

    for(int i = 0; i < 32; i++)
    {
        // Read header frame
        if(read_hframe(hframe, fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_hframe function failed\n");
            return e_failure;
        }
        
        // Read content size
        if(read_size(&size, fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_size function failed\n");
            return e_failure;
        }

        // Skip 3 flag bytes
        if(fseek(fptr_mp3_src, FLAG_BYTES, SEEK_CUR) != 0) return e_failure;

        // Edit title
        if(!strcmp(hframe, "TIT2"))
        {
            // Edit title content
            if(edit_content(fptr_mp3_src, new_content, size) == e_failure)
            {
                printf("ERROR: edit_content function failed\n");
                return e_failure;
            }

            return e_success;
        }
        else
        {
            // Skip (size - 1) bytes for next header frame
            if(fseek(fptr_mp3_src, size - 1, SEEK_CUR) != 0) return e_failure;
        }
    }

    // If TIT2 header frame not found
    printf("ERROR: Title not found\n");
    return e_failure;
}

/* Edit Artist details
 * Input: Source file pointer and new content
 * Output: File with modified title
 * Description: edits the Artist content
 * Return value: e_success and e_failure
 */
Status edit_artist(FILE *fptr_mp3_src, char *new_content)
{
    // Skip 10 header bytes
    if (fseek(fptr_mp3_src, 10, SEEK_SET) != 0) return e_failure;

    char hframe[MAX_HFRAME_SIZE];
    uint size;

    for(int i = 0; i < 32; i++)
    {   
        // Read header frame
        if(read_hframe(hframe, fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_hframe function failed\n");
            return e_failure;
        }

        // Read content size
        if(read_size(&size, fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_size function failed\n");
            return e_failure;
        }

        // Skip 3 flag bytes
        if(fseek(fptr_mp3_src, FLAG_BYTES, SEEK_CUR) != 0) return e_failure;

        if(!strcmp(hframe, "TPE1"))
        {
            // Edit artist content
            if(edit_content(fptr_mp3_src, new_content, size) == e_failure)
            {
                printf("ERROR: edit_content function failed\n");
                return e_failure;
            }

            return v_success;
        }
        else
        {
            // Skip (size - 1) bytes for next header frame
            if(fseek(fptr_mp3_src, size - 1, SEEK_CUR) != 0) return e_failure;
        }
    }

    // If TPE1 header frame not found
    printf("ERROR: Artist not found\n");
    return e_failure;
}

/* Edit Album details
 * Input: Source file pointer and new content
 * Output: File with modified title
 * Description: edits the Album content
 * Return value: e_success and e_failure
 */
Status edit_album(FILE *fptr_mp3_src, char *new_content)
{
    // Skip 10 header bytes
    if (fseek(fptr_mp3_src, 10, SEEK_SET) != 0) return e_failure;

    char hframe[MAX_HFRAME_SIZE];
    uint size;

    for(int i = 0; i < 32; i++)
    {
        // Read header frame
        if(read_hframe(hframe, fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_hframe function failed\n");
            return e_failure;
        }

        // Read content size
        if(read_size(&size, fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_size function failed\n");
            return e_failure;
        }

        // Skip 3 flag bytes
        if(fseek(fptr_mp3_src, FLAG_BYTES, SEEK_CUR) != 0) return e_failure;

        // Edit album content
        if(!strcmp(hframe, "TALB"))
        {
            if(edit_content(fptr_mp3_src, new_content, size) == e_failure)
            {
                printf("ERROR: edit_content function failed\n");
                return e_failure;
            }

            return v_success;
        }
        else
        {
            // Skip (size - 1) bytes for next header frame
            if(fseek(fptr_mp3_src, size - 1, SEEK_CUR) != 0) return e_failure;
        }
    }

    // If TALB header frame not found
    printf("ERROR: Album not found\n");
    return e_failure;
}

/* Edit Year details
 * Input: Source file pointer and new content
 * Output: File with modified title
 * Description: edits the Year content
 * Return value: e_success and e_failure
 */
Status edit_year(FILE *fptr_mp3_src, char *new_content)
{
    // Skip 10 header bytes
    if (fseek(fptr_mp3_src, 10, SEEK_SET) != 0) return e_failure;

    char hframe[MAX_HFRAME_SIZE];
    uint size;

    for(int i = 0; i < 32; i++)
    {
        // Read header frame
        if(read_hframe(hframe, fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_hframe function failed\n");
            return e_failure;
        }

        // Read content size
        if(read_size(&size, fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_size function failed\n");
            return e_failure;
        }

        // Skip 3 flag bytes
        if(fseek(fptr_mp3_src, FLAG_BYTES, SEEK_CUR) != 0) return e_failure;

        // Edit year details
        if(!strcmp(hframe, "TYER"))
        {
            // Check if the user has entered digits only
            for(int j = 0; j < strlen(new_content); j++)
            {
                if(new_content[j] < '0' || new_content[j] > '9')
                {
                    printf("ERROR: Invalid entry for year\n");
                    printf("ERROR: Year should be in digits\n");
                    return e_failure;
                }
            }

            // Edit year content
            if(edit_content(fptr_mp3_src, new_content, size) == e_failure)
            {
                printf("ERROR: edit_content function failed\n");
                return e_failure;
            }

            return v_success;
        }
        else
        {
            // Skip (size - 1) bytes for next header frame
            if(fseek(fptr_mp3_src, size - 1, SEEK_CUR) != 0) return e_failure;
        }
    }

    // If TYER header frame not found
    printf("ERROR: Year not found\n");
    return e_failure;
}

/* Edit Genre details
 * Input: Source file pointer and new content
 * Output: File with modified title
 * Description: edits the Genre content
 * Return value: e_success and e_failure
 */
Status edit_genre(FILE *fptr_mp3_src, char *new_content)
{
    // Skip 10 header bytes
    if (fseek(fptr_mp3_src, 10, SEEK_SET) != 0) return e_failure;

    char hframe[MAX_HFRAME_SIZE];
    uint size;

    for(int i = 0; i < 32; i++)
    {
        // Read header frame
        if(read_hframe(hframe, fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_hframe function failed\n");
            return e_failure;
        }

        // Read content size
        if(read_size(&size, fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_size function failed\n");
            return e_failure;
        }

        // Read 3 flag bytes
        if(fseek(fptr_mp3_src, FLAG_BYTES, SEEK_CUR) != 0) return e_failure;

        // Edit genre details
        if(!strcmp(hframe, "TCON"))
        {
            if(edit_content(fptr_mp3_src, new_content, size) == e_failure)
            {
                printf("ERROR: edit_content function failed\n");
                return e_failure;
            }

            return v_success;
        }
        else
        {
            // Skip (size - 1) bytes for next header frame
            if(fseek(fptr_mp3_src, size - 1, SEEK_CUR) != 0) return e_failure;
        }
    }

    // If TCON header frame not found
    printf("ERROR: Genre not found\n");
    return e_failure;
}

/* Edit Comment details
 * Input: Source file pointer and new content
 * Output: File with modified title
 * Description: edits the Comment content
 * Return value: e_success and e_failure
 */
Status edit_comment(FILE *fptr_mp3_src, char *new_content)
{
    // Skip 10 header bytes
    if (fseek(fptr_mp3_src, 10, SEEK_SET) != 0) return e_failure;

    char hframe[MAX_HFRAME_SIZE];
    uint size;

    for(int i = 0; i < 32; i++)
    {
        // Read header frame
        if(read_hframe(hframe, fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_hframe function failed\n");
            return e_failure;
        }

        // Read content size
        if(read_size(&size, fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_size function failed\n");
            return e_failure;
        }

        // Skip 3 flag bytes
        if(fseek(fptr_mp3_src, FLAG_BYTES, SEEK_CUR) != 0) return e_failure;

        // Edit comment content
        if(!strcmp(hframe, "COMM"))
        {
            if(edit_content(fptr_mp3_src, new_content, size) == e_failure)
            {
                printf("ERROR: edit_content function failed\n");
                return e_failure;
            }

            return v_success;
        }
        else
        {
            // Skip (size - 1) bytes for next header frame
            if(fseek(fptr_mp3_src, size - 1, SEEK_CUR) != 0) return e_failure;
        }
    }

    // If COMM Header frame not found
    printf("ERROR: Comment not found\n");
    return e_failure;
}

/* Edit content
 * Input: Tag details, existing content location and available size
 * Output: Modified tag content
 * Description: Modifies the existing tag content with new content
 * Return value: e_success and e_failure
 */
Status edit_content(FILE *fptr_mp3_src, char *new_content, uint size)
{
    char *content_buffer = calloc(size, sizeof(char));

    // Copy new content in variable which contain tag content (old tag content)
    strcpy(content_buffer, new_content);

    // Modify the existing tag content in the file with new content
    if(fwrite(content_buffer, sizeof(char), size - 1, fptr_mp3_src) != size - 1) return e_failure;

    free(content_buffer);

    // Modified successfully
    return e_success;
}