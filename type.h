#ifndef TYPES_H
#define TYPES_H

// User defined data type
typedef unsigned int uint;

// Status will be used in function return type
typedef enum {
    v_success,
    v_failure,
    e_success = 0,
    e_failure
} Status;

typedef enum {
    view,
    edit,
    help,
    unsupported
} OperationType;

#endif