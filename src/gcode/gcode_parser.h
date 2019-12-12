#ifndef UTILS_GCODE_PARSER_H
#define UTILS_GCODE_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**********************
 * VIEWER PROTOTYPES
 **********************/
#define GCODE_COMMENT 1
#define GCODE_SUBROUTINE 2
#define GCODE_NOFILE -1
#define GCODE_MOVE 0
#define GCODE_NONMOVE 3
typedef struct
{
    int n;
    float g[100];
    float m[100];

    float x;
    bool x_set;
    float y;
    bool y_set;
    float z;
    bool z_set;

    float i;
    bool i_set;
    float j;
    bool j_set;
    float k;
    bool k_set;

    float r;
    bool r_set;

    float f;
    bool f_set;

    int type;
    int status; //is -1 at end of stack
    int g_word_counter;
    int m_word_counter;
}gcode_t;

typedef struct
{
    int g;
    float x;
    float y;
    float z; 
    float i;
    float j;
    //float k;
    float f;
}gcode_move_t;

bool gcode_parse(const char *);
gcode_t gcode_stack_next();
void gcode_stack_dump();
void gcode_stack_clear();
void gcode_parse_moves();
gcode_move_t gcode_get_move(size_t);
size_t gcode_get_move_count();
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*UTILS_GCODE_PARSER_H*/