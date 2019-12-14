#ifndef DXF_H
#define DXF_H

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include <string>
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
class DXF{
    public:
        std::string readDXF(const char* file);
    private:
};

/**********************
 * VIEWER PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif