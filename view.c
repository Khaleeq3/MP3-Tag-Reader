#include <stdio.h>
#include "view.h"
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

/* function definitions */

/* Checks the operation type
 * Input: Command line arguments
 * Output: Operation type
 * Description: Determines the operation
 * Return value: view, edit, help, unsupported
 */
OperationType check_operation(char *argv[])
{
    if (argv[1] == NULL)
    {
        return unsupported;
    }

    if(!strcmp(argv[1], "-v"))
    {
        return view;
    }
    else if (!strcmp(argv[1], "-e"))
    {
        return edit;
    }
    else if (!strcmp(argv[1], "--help"))
    {
        return help;
    }
    else
    {
        return unsupported;
    }
}

/* Read and validate the arguments for viewing option 
 * Input: Command line arguments and tag details 
 * Output: Validation of arguments for viewing option
 * Description: reads the arguments and validates them 
 * Return value: v_success and v_failure
 */
Status read_and_validate_view_args(char *argv[], TagInfo *tagInfo)
{
    if (argv[2] == NULL)
    {
        printf("ERROR: INVALID ARGUMENTS\n");
        printf("USAGE:\n");
        printf("To view please pass like: ./a.out -v <.mp3 file name>\n");
        printf("To help please pass like: ./a.out --help\n");
        return v_failure;
    }
    
    char *str;
    if((str = strstr(argv[2], ".")) == NULL || strcmp(str, ".mp3"))
    {
        printf("ERROR: Unsupported format\n");
        printf("USAGE:\n");
        printf("To view please pass like: ./a.out -v <.mp3 file name>\n");
        printf("To help please pass like: ./a.out --help\n");
        return v_failure;
    }

    // get the name of source file
    tagInfo->mp3_fname_src = argv[2];

    return v_success;
}

/*Opens the required files 
 * Input: Source file name
 * Output: File pointer of source file
 * Description: Opens the source file
 * Return value: v_success and v_failure
 */
Status open_files_for_view(TagInfo *tagInfo)
{
    // Open mp3 file in read mode
    tagInfo->fptr_mp3_src = fopen(tagInfo->mp3_fname_src, "r");

    // Error handling for opening mp3 file
    if(tagInfo->fptr_mp3_src == NULL)
    {
        perror("fopen");
    	printf("ERROR: Unable to open file %s\n", tagInfo->mp3_fname_src);

        return v_failure;
    }

    // Read tag identifier
    char tag_identifier[4] = {0};
    if(fread(tag_identifier, sizeof(char), 3, tagInfo->fptr_mp3_src) != 3) return v_failure;

    // Error handling for tag identifier
    if(strcmp(tag_identifier, "ID3"))
    {
        printf("ERROR: Unsupported file\n");
        printf("ERROR: ID3 tag not found\n");
        return v_failure;
    }

    // Read version 
    unsigned short version;
    if(fread(&version, sizeof(unsigned short), 1, tagInfo->fptr_mp3_src) != 1) return v_failure;

    // Error handling for version
    // convert network (big endiann) to host (little endiann)
    version = version >> 8 | version << 8;
    // printf("%#X\n", version);
    if(version != 0x0300)
    {
        printf("ERROR: Unsupported version\n");
        printf("ERROR: Version should be ID3v2.3\n");
        return v_failure;
    }

    // File is valid and opened successfully
    return v_success;
}

/* Read the required tag details
 * Input: Address of variable which holds the tag details
 * Output: Tag details
 * Description: Reads all the required tag details from source file
 * Return value: v_success and v_failure
 */
Status read_hframe_details(TagInfo *tagInfo)
{
    // Read title details
    if(read_title(tagInfo) == v_failure)
    {
        printf("ERROR: read_title function failed\n");
        return v_failure;
    }

    // Read artist details
    if(read_artist(tagInfo) == v_failure)
    {
        printf("ERROR: read_artist function failed\n");
        return v_failure;
    }

    // Read album details
    if(read_album(tagInfo) == v_failure)
    {
        printf("ERROR: read_album function failed\n");
        return v_failure;
    }

    // Read year details
    if(read_year(tagInfo) == v_failure)
    {
        printf("ERROR: read_year function failed\n");
        return v_failure;
    }

    // Read genre details
    if(read_genre(tagInfo) == v_failure)
    {
        printf("ERROR: read_genre function failed\n");
        return v_failure;
    }

    // Read comment details
    if(read_comment(tagInfo) == v_failure)
    {
        printf("ERROR: read_comment function failed\n");
        return v_failure;
    }

    // Successfully read all the required details
    return v_success;
}

/* Read header frame
 * Input: variable to store header frame  and mp3 source file pointer 
 * Output: header frame
 * Descrition: Reads the 4 bytes of header frame 
 * Return value: v_failue and v_success
 */
Status read_hframe(char *hframe, FILE *fptr_mp3_src)
{
    if(fread(hframe, sizeof(char), HFRAME_BUFFER, fptr_mp3_src) != HFRAME_BUFFER) return v_failure;

    return v_success;
}

/* Reads the size of header frame content 
 * Input: Variable to store size and source file pointer
 * Output: Size of header frame content
 * Descriptoin: Reads the size of header frame content i.e 4 bytes
 * Return value: v_success and v_failure
 */
Status read_size(uint *size, FILE *fptr_mp3_src)
{
    if(fread(size, sizeof(uint), 1, fptr_mp3_src) != 1) return v_failure;

    // Convert big endian to little endian
    *size = ntohl(*size);
    return v_success;
}

/* Read the content of header frame
 * Input: Size, variable to hold header frame content and source file pointer 
 * Output: Header frame content
 * Description: Reads the content of header frame 
 * Return value: v_success and v_failure
 */
Status read_content(uint size, char **hframe_content, FILE *fptr_mp3_src)
{
    // Allocate memory for header frame content dynamically
    *hframe_content = calloc(size, sizeof(char));
    
    // Read header frame content
    if(fread(*hframe_content, sizeof(char), size - 1, fptr_mp3_src) != size - 1) return v_failure;

    return v_success;
}

/* Read title details
 * Input: tag Information
 * Output: Title details
 * Description: Reads the title details
 * Return v_success and v_failure
 */
Status read_title(TagInfo *tagInfo)
{
    // Skip 10 header bytes
    if (fseek(tagInfo->fptr_mp3_src, 10, SEEK_SET) != 0) return v_failure;

    char hframe_temp[MAX_HFRAME_SIZE];
    uint size_temp;

    for(int i = 0; i < 32; i++)
    {   
        // Read header frame
        if(read_hframe(hframe_temp, tagInfo->fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_hframe function failed\n");
            return v_failure;
        }
        
        // Read content size
        if(read_size(&size_temp, tagInfo->fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_size function failed\n");
            return v_failure;
        }

        // Skip 3 flag bytes
        if(fseek(tagInfo->fptr_mp3_src, FLAG_BYTES, SEEK_CUR) != 0) return v_failure;

        // Reads title details
        if(!strcmp(hframe_temp, "TIT2"))
        {
            strcpy(tagInfo->TIT2_hframe, hframe_temp);

            tagInfo->TIT2_size = size_temp;

            // Read title content
            if(read_content(tagInfo->TIT2_size, &tagInfo->TIT2_content, tagInfo->fptr_mp3_src) == v_failure)
            {
                printf("ERROR: read_content function failed\n");
                return v_failure;
            }

            return v_success;
        }
        else
        {   // skip (size - 1) bytes for next header frame
            if(fseek(tagInfo->fptr_mp3_src, size_temp - 1, SEEK_CUR) != 0) return v_failure;
        }
    }
    
    // If header frmae not found
    printf("ERROR: Title not found\n");
    return v_failure;
}

/* Read Artist details
 * Input: Tag Information
 * Output: Artist details
 * Description: Reads the Artist details
 * Return v_success and v_failure
 */
Status read_artist(TagInfo *tagInfo)
{
    // Skip 10 header bytes
    if (fseek(tagInfo->fptr_mp3_src, 10, SEEK_SET) != 0) return v_failure;
    
    char hframe_temp[MAX_HFRAME_SIZE];
    uint size_temp;

    for(int i = 0; i < 32; i++)
    {
        // Read header frame
        if(read_hframe(hframe_temp, tagInfo->fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_hframe function failed\n");
            return v_failure;
        }

        // Read content size
        if(read_size(&size_temp, tagInfo->fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_size function failed\n");
            return v_failure;
        }

        // Skip 3 flag bytes
        if(fseek(tagInfo->fptr_mp3_src, FLAG_BYTES, SEEK_CUR) != 0) return v_failure;

        // Read artist details
        if(!strcmp(hframe_temp, "TPE1"))
        {
            strcpy(tagInfo->TPE1_hframe, hframe_temp);

            tagInfo->TPE1_size = size_temp;

            // Read artist content
            if(read_content(tagInfo->TPE1_size, &tagInfo->TPE1_content, tagInfo->fptr_mp3_src) == v_failure)
            {
                printf("ERROR: read_content function failed\n");
                return v_failure;
            }
            
            return v_success;
        }
        else
        {
            // Skip (size - 1) bytes for next header frame
            if(fseek(tagInfo->fptr_mp3_src, size_temp - 1, SEEK_CUR) != 0) return v_failure;
        }
    }
    
    // If header frmae not found
    printf("ERROR: Artist not found\n");
    return v_failure;
}

/* Read Album details
 * Input: Tag Information
 * Output: Album details
 * Description: Reads the Album details
 * Return v_success and v_failure
 */
Status read_album(TagInfo *tagInfo)
{
    // Skip 10 header bytes
    if (fseek(tagInfo->fptr_mp3_src, 10, SEEK_SET) != 0) return v_failure;
    
    char hframe_temp[MAX_HFRAME_SIZE];
    uint size_temp;

    for(int i = 0; i < 32; i++)
    {   
        // Read header frame
        if(read_hframe(hframe_temp, tagInfo->fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_hframe function failed\n");
            return v_failure;
        }

        // Read content size
        if(read_size(&size_temp, tagInfo->fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_size function failed\n");
            return v_failure;
        }

        // Skip 3 flag bytes 
        if(fseek(tagInfo->fptr_mp3_src, FLAG_BYTES, SEEK_CUR) != 0) return v_failure;

        // Read album details
        if(!strcmp(hframe_temp, "TALB"))
        {
            strcpy(tagInfo->TALB_hframe, hframe_temp);

            tagInfo->TALB_size = size_temp;

            // read album content
            if(read_content(tagInfo->TALB_size, &tagInfo->TALB_content, tagInfo->fptr_mp3_src) == v_failure)
            {
                printf("ERROR: read_content function failed\n");
                return v_failure;
            }

            return v_success;
        }
        else
        {   // Skip (size - 1) bytes for next header frame
            if(fseek(tagInfo->fptr_mp3_src, size_temp - 1, SEEK_CUR) != 0) return v_failure;
        }
    }
    
    // If header frame not found
    printf("ERROR: Album not found\n");
    return v_failure;
}

/* Read Year details
 * Input: Tag Information
 * Output: Year details
 * Description: Reads the Year details
 * Return v_success and v_failure
 */
Status read_year(TagInfo *tagInfo)
{
    // Skip 10 header bytes
    if (fseek(tagInfo->fptr_mp3_src, 10, SEEK_SET) != 0) return v_failure;
    
    char hframe_temp[MAX_HFRAME_SIZE];
    uint size_temp;

    for(int i = 0; i < 32; i++)
    {
        // Read header frame
        if(read_hframe(hframe_temp, tagInfo->fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_hframe function failed\n");
            return v_failure;
        }

        // Read content size
        if(read_size(&size_temp, tagInfo->fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_size function failed\n");
            return v_failure;
        }

        // Skip 3 flag bytes
        if(fseek(tagInfo->fptr_mp3_src, FLAG_BYTES, SEEK_CUR) != 0) return v_failure;

        // Read year details
        if(!strcmp(hframe_temp, "TYER"))
        {
            strcpy(tagInfo->TYER_hframe, hframe_temp);

            tagInfo->TYER_size = size_temp;
            
            // Read year content
            if(read_content(tagInfo->TYER_size, &tagInfo->TYER_content, tagInfo->fptr_mp3_src) == v_failure)
            {
                printf("ERROR: read_content function failed\n");
                return v_failure;
            }

            return v_success;
        }
        else
        {
            // Skip (size - 1) bytes for next header frame
            if(fseek(tagInfo->fptr_mp3_src, size_temp - 1, SEEK_CUR) != 0) return v_failure;
        }
    }
    
    // If header frame not found
    printf("ERROR: Year not found\n");
    return v_failure;
}

/* Read Genre details
 * Input: Tag Information
 * Output: Genre details
 * Description: Reads the Genre details
 * Return v_success and v_failure
 */
Status read_genre(TagInfo *tagInfo)
{
    // Skip 10 header bytes
    if (fseek(tagInfo->fptr_mp3_src, 10, SEEK_SET) != 0) return v_failure;
    
    char hframe_temp[MAX_HFRAME_SIZE];
    uint size_temp;

    for(int i = 0; i < 32; i++)
    {
        // Read header frame
        if(read_hframe(hframe_temp, tagInfo->fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_hframe function failed\n");
            return v_failure;
        }

        // Read content size
        if(read_size(&size_temp, tagInfo->fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_size function failed\n");
            return v_failure;
        }

        // Skip 3 flag bytes
        if(fseek(tagInfo->fptr_mp3_src, FLAG_BYTES, SEEK_CUR) != 0) return v_failure;

        // Read genre details
        if(!strcmp(hframe_temp, "TCON"))
        {
            strcpy(tagInfo->TCON_hframe, hframe_temp);

            tagInfo->TCON_size = size_temp;

            if(read_content(tagInfo->TCON_size, &tagInfo->TCON_content, tagInfo->fptr_mp3_src) == v_failure)
            {
                printf("ERROR: read_content function failed\n");
                return v_failure;
            }

            return v_success;
        }
        else
        {   
            // skip (size - 1) bytes for next header frame
            if(fseek(tagInfo->fptr_mp3_src, size_temp - 1, SEEK_CUR) != 0) return v_failure;
        }
    }
    
    // If header frame not found
    printf("ERROR: Genre not found\n");
    return v_failure;
}

/* Read Comment details
 * Input: Tag Information
 * Output: Comment details
 * Description: Reads the Comment details
 * Return v_success and v_failure
 */
Status read_comment(TagInfo *tagInfo)
{
    // Skip 10 header bytes
    if (fseek(tagInfo->fptr_mp3_src, 10, SEEK_SET) != 0) return v_failure;
    
    char hframe_temp[MAX_HFRAME_SIZE];
    uint size_temp;

    for(int i = 0; i < 32; i++)
    {
        // Read header frame
        if(read_hframe(hframe_temp, tagInfo->fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_hframe function failed\n");
            return v_failure;
        }

        // Read content size
        if(read_size(&size_temp, tagInfo->fptr_mp3_src) == v_failure)
        {
            printf("ERROR: read_size function failed\n");
            return v_failure;
        }

        // Skip 3 falg bytes
        if(fseek(tagInfo->fptr_mp3_src, FLAG_BYTES, SEEK_CUR) != 0) return v_failure;

        // Read comment details
        if(!strcmp(hframe_temp, "COMM"))
        {
            strcpy(tagInfo->COMM_hframe, hframe_temp);

            tagInfo->COMM_size = size_temp;

            // Read comment content
            if(read_content(tagInfo->COMM_size, &tagInfo->COMM_content, tagInfo->fptr_mp3_src) == v_failure)
            {
                printf("ERROR: read_content function failed\n");
                return v_failure;
            }

            return v_success;
        }
        else
        {
            // Skip (size - 1) bytes for next header frame
            if(fseek(tagInfo->fptr_mp3_src, size_temp - 1, SEEK_CUR) != 0) return v_failure;
        }
    }
    
    // If header frame not found
    printf("ERROR: Comment not found\n");
    return v_failure;
}

/* Display tag details
 * Input: tag details
 * Output: Displays tag details on stdout
 * Return value: v_success
 */
Status display_tag_details(TagInfo *tagInfo)
{
    puts("---------------------------------------------------------------");
    puts("MP3 Tag Reader and Editor for ID3V2");
    puts("---------------------------------------------------------------");

    // Display Title
    printf("%-15s %-10c %s\n", "Title", ':', tagInfo->TIT2_content);

    // Display Artist name
    printf("%-15s %-10c %s\n", "Artist",':', tagInfo->TPE1_content);

    // Display Album name
    printf("%-15s %-10c %s\n", "Album", ':', tagInfo->TALB_content);

    // Display year
    printf("%-15s %-10c %s\n", "Year", ':', tagInfo->TYER_content);

    // Display genre
    printf("%-15s %-10c %s\n", "Music", ':', tagInfo->TCON_content);

    // Display composer
    printf("%-15s %-10c %s\n", "Comment", ':', tagInfo->COMM_content);

    puts("---------------------------------------------------------------");

    // close the source file
    fclose(tagInfo->fptr_mp3_src);
    return v_success;
}

/* Display help */
Status display_help()
{
    puts("Usage: ./a.out -[tTaAycg] \"value\" <.mp3 file name>");
    puts("-t\tModifies a Titile tag");
//    puts("-T\tModifies a Track tag");
    puts("-a\tModifies an Artist tag");
    puts("-A\tModifies an Album tag");
    puts("-y\tModifies a year tag");
    puts("-c\tModifies a Comment tag");
    puts("-g\tModifies a Genre tag");
    puts("--help\tDisplay this help info");
//    puts("-v\tPrints version info");

    return v_success;
}