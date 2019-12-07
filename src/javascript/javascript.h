//-----------------------------------------------------------------------------
// The interface bewteen JetCAD and Javascript counter parts
//-----------------------------------------------------------------------------
#ifndef JAVASCRIPT_H
#define JAVASCRIPT_H

#include <duktape/duktape.h>
#include <string>
#include <stdio.h>

class Javascript{

    public:
        bool loop;
        bool window_open;
        int ret;
        bool lines_available;
        FILE* file_handle;
        char* read_line = NULL;
        size_t len = 0;
        ssize_t read;
        void init();
        void destroy();
        void refresh();
        void bind(std::string name, duk_ret_t (*callback)(duk_context *ctx), int number_of_arguments);
        void bind_module(std::string name, const duk_function_list_entry function_list[]);
        std::string eval(std::string exp);
        void eval_file(std::string file);
    private:
        duk_context *ctx;
};

extern Javascript js;

#endif
