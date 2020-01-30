#ifndef DXF_H
#define DXF_H

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include <string>
#include <json/json.h>
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
        nlohmann::json readDXF(const char* file);
    private:
};

/**********************
 * VIEWER PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif