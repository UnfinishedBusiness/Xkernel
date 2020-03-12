#include <duktape/duktape.h>
#include <duktape/duk_module_duktape.h>
#include <javascript/javascript.h>
#include <tinyfiledialog/tinyfiledialogs.h>
#include <json/json.h>
#include <network/httplib.h>
#include <render/render.h>
#include <serial/serial.h>
#include <gcode/gcode_parser.h>
#include <motion_control/motion_control.h>
#include <geometry/geometry.h>
#include <dxf/dxf.h>
#include <inih/inih.h>
#include <gui/gui.h>
#include <dirent.h>
#include <string>
#include <chrono>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #include <windows_specific.h>
#endif

using json = nlohmann::json;
serial::Serial my_serial;

typedef struct {
	/* The double value in the union is there to ensure alignment is
	 * good for IEEE doubles too.  In many 32-bit environments 4 bytes
	 * would be sufficiently aligned and the double value is unnecessary.
	 */
	union {
		size_t sz;
		double d;
	} u;
} alloc_hdr;

size_t total_allocated = 0;
static size_t max_allocated = 100 * ONE_MEGABYTE;  /* 1MB sandbox */

static void my_dump_memstate(void)
{
	//fprintf(stderr, "Allocated: %ld bytes / %ld bytes\n", (long) total_allocated, (long) max_allocated);
	//fflush(stderr);
}

static void *my_alloc(void *udata, duk_size_t size) {
	alloc_hdr *hdr;

	(void) udata;  /* Suppress warning. */

	if (size == 0) {
		return NULL;
	}

	if (total_allocated + size > max_allocated) {
		fprintf(stderr, "my maximum allocation size reached, %ld requested in my_alloc\n",
		        (long) size);
		fflush(stderr);
		return NULL;
	}

	hdr = (alloc_hdr *) malloc(size + sizeof(alloc_hdr));
	if (!hdr) {
		return NULL;
	}
	hdr->u.sz = size;
	total_allocated += size;
	my_dump_memstate();
	return (void *) (hdr + 1);
}

static void *my_realloc(void *udata, void *ptr, duk_size_t size) {
	alloc_hdr *hdr;
	size_t old_size;
	void *t;

	(void) udata;  /* Suppress warning. */

	/* Handle the ptr-NULL vs. size-zero cases explicitly to minimize
	 * platform assumptions.  You can get away with much less in specific
	 * well-behaving environments.
	 */

	if (ptr) {
		hdr = (alloc_hdr *) (((char *) ptr) - sizeof(alloc_hdr));
		old_size = hdr->u.sz;

		if (size == 0) {
			total_allocated -= old_size;
			free((void *) hdr);
			my_dump_memstate();
			return NULL;
		} else {
			if (total_allocated - old_size + size > max_allocated) {
				fprintf(stderr, "my maximum allocation size reached, %ld requested in my_realloc\n",
				        (long) size);
				fflush(stderr);
				return NULL;
			}

			t = realloc((void *) hdr, size + sizeof(alloc_hdr));
			if (!t) {
				return NULL;
			}
			hdr = (alloc_hdr *) t;
			total_allocated -= old_size;
			total_allocated += size;
			hdr->u.sz = size;
			my_dump_memstate();
			return (void *) (hdr + 1);
		}
	} else {
		if (size == 0) {
			return NULL;
		} else {
			if (total_allocated + size > max_allocated) {
				fprintf(stderr, "my maximum allocation size reached, %ld requested in my_realloc\n",
				        (long) size);
				fflush(stderr);
				return NULL;
			}

			hdr = (alloc_hdr *) malloc(size + sizeof(alloc_hdr));
			if (!hdr) {
				return NULL;
			}
			hdr->u.sz = size;
			total_allocated += size;
			my_dump_memstate();
			return (void *) (hdr + 1);
		}
	}
}

static void my_free(void *udata, void *ptr) {
	alloc_hdr *hdr;

	(void) udata;  /* Suppress warning. */

	if (!ptr) {
		return;
	}
	hdr = (alloc_hdr *) (((char *) ptr) - sizeof(alloc_hdr));
	total_allocated -= hdr->u.sz;
	free((void *) hdr);
	my_dump_memstate();
}

static void push_file_as_string(duk_context *ctx, const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (f)
    {
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);  /* same as rewind(f); */
        char *string = (char*)malloc(fsize + 1);
        fread(string, 1, fsize, f);
        fclose(f);
        duk_push_lstring(ctx, (char*)string, (long)fsize);
        free(string);
    }
    else
    {
        duk_push_undefined(ctx);
    }
}
static void my_fatal(void *udata, const char *msg) {
	(void) udata;  /* Suppress warning. */
	fprintf(stderr, "FATAL: %s\n", (msg ? msg : "no message"));
	fflush(stderr);
	exit(1);  /* must not return */
}
/* Begin bindings */
#include <javascript/bindings/serial.bind>
#include <javascript/bindings/render.bind>
#include <javascript/bindings/live_render.bind>
#include <javascript/bindings/http.bind>
#include <javascript/bindings/gui.bind>
#include <javascript/bindings/file.bind>
#include <javascript/bindings/ini.bind>
#include <javascript/bindings/console.bind>
#include <javascript/bindings/classless.bind>
#include <javascript/bindings/window_menu.bind>
#include <javascript/bindings/file_dialog.bind>
#include <javascript/bindings/system.bind>
#include <javascript/bindings/time.bind>
#include <javascript/bindings/gcode.bind>
#include <javascript/bindings/dxf.bind>
#include <javascript/bindings/FastMath.bind>
#include <javascript/bindings/motion_control.bind>
#include <javascript/bindings/geometry.bind>
#include <javascript/bindings/text_editor.bind>
#include <javascript/bindings/trace_master.bind>
/* End Bindings */ 
std::string Javascript::eval(std::string exp)
{
    duk_push_string(ctx, exp.c_str());
    if (duk_peval(ctx) != 0)
    {
        printf("[Javascript::eval] (%s) eval failed: %s\n",exp.c_str(), duk_safe_to_string(ctx, -1));
    }
    else
    {
        if (std::string(duk_safe_to_string(ctx, -1)) != "undefined")
        {
            printf("[scriptEval] result is: %s\n", duk_safe_to_string(ctx, -1));
        }
    }
	std::string ret = std::string(duk_get_string(ctx, -1));
    duk_pop(ctx);  /* pop result */
    duk_gc(ctx, 0);
	return ret;
}
void Javascript::eval_file(std::string file)
{
    push_file_as_string(ctx, file.c_str());
    if (duk_peval(ctx) != 0) {
        printf("[Javascript::eval_file] Error: %s\n", duk_safe_to_string(ctx, -1));
    }
    duk_pop(ctx);  /* ignore result */
    duk_gc(ctx, 0);
}
void Javascript::init()
{
    using namespace std::literals;
    using Clock = std::chrono::system_clock;
    auto t = Clock::now();
    this->init_timestamp_micros = t.time_since_epoch() / 1us;
    this->init_timestamp_millis = t.time_since_epoch() / 1ms;

    this->lines_available = true;
    this->window_open = false;
    this->loop = true;
    //ctx = duk_create_heap_default();
    ctx = ctx = duk_create_heap(my_alloc, my_realloc, my_free, NULL, my_fatal);
    printf("(Javascript::init) Created sandbox heap with a size of -> %ld MB\n", max_allocated / ONE_MEGABYTE);

    /* Register binidngs */
    classless_register_bindings();
    render_register_bindings();
    live_render_register_bindings();
    serial_register_bindings();
    http_register_bindings();
    ini_register_bindings();
    gui_register_bindings();
    window_menu_register_bindings();
    console_register_bindings();
    file_register_bindings();
    file_dialog_register_bindings();
    system_register_bindings();
    time_register_bindings();
    gcode_register_bindings();
    dxf_register_bindings();
    FastMath_register_bindings();
    motion_control_register_bindings();
    geometry_register_bindings();
    text_editor_register_bindings();
    trace_master_register_bindings();
}
void Javascript::bind(std::string name, duk_ret_t (*callback)(duk_context *ctx), int number_of_arguments)
{
    duk_push_c_function(ctx, callback, number_of_arguments /*nargs*/);
    duk_put_global_string(ctx, name.c_str());
}
void Javascript::bind_module(std::string name, const duk_function_list_entry function_list[])
{
    duk_push_global_object(ctx);
    duk_push_object(ctx);  /* -> [ ... global obj ] */
    duk_put_function_list(ctx, -1, function_list);
    duk_put_prop_string(ctx, -2, name.c_str());  /* -> [ ... global ] */
    duk_pop(ctx);
}
void Javascript::destroy()
{
    duk_destroy_heap(ctx);
}
void Javascript::refresh()
{
    if (ctx != NULL)
    {
        destroy();
    }
    init();
}