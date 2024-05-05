#include <stdio.h>
#include "view.h"
#include "edit.h"

int main(int argc, char *argv[])
{
    // Declare a structure variable to store tag details
    TagInfo tagInfo;

    // Check operation
    if(check_operation(argv) == view)
    {
        // Error handling for Read and validate arguments for view option
        if(read_and_validate_view_args(argv, &tagInfo) == v_failure)
        {
            printf("ERROR: read_and_validate_view_args function failed\n");
            return 1;
        }

        // error handling for Opening required files
        if(open_files_for_view(&tagInfo) == v_failure)
        {
            printf("open_files_for_view function failed\n");
            return 1;
        }

        // Error handling for Reading tag details
        if(read_hframe_details(&tagInfo) == v_failure)
        {
            printf("ERROR: read_tag_details function failed\n");
            return 1;
        }

        // Error handling for Displaying tag details
        if(display_tag_details(&tagInfo) != v_success)
        {
            printf("ERROR: display_details function failed\n");
            return 1;
        }
    }
    else if (check_operation(argv) == edit)
    {
        // Error hanlding for Read and validate arguments for edit option
        if(read_and_validate_edit_args(argv, &tagInfo) == e_failure)
        {
            printf("ERROR: read_and_validate_edit_args function failed\n");
            return 1;
        }

        // Error hanlding for Editing tag details
        if(edit_hframe_details(argv[2][1], &tagInfo) == e_failure)
        {
            printf("ERROR: edit_tag_details function failed\n");
            return 1;
        }
    }
    else if (check_operation(argv) == help)
    {
        // Error handling for Displaying help
        if(display_help() != v_success)
        {
            printf("ERROR: display_help function failed\n");
            return 1;
        }
    }
    else
    {   // Print error message in case of invalid arguments
        printf("ERROR: INVALID ARGUMENTS\n");
        printf("USAGE:\n");
        printf("To view please pass like: ./a.out -v <.mp3 file name>\n");
        printf("To edit please pass like: ./a.out -e -t/-a/-A/-m/-y/-c <.mp3 file name>\n");
        printf("To help please pass like: ./a.out --help\n");
        return 1;
    }
    
    return 0;
}