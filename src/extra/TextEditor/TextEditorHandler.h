#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

#include <string>
#include <vector>
#include <json/json.h>
#include <extra/TextEditor/TextEditor.h>

class TextEditorHandler{
    public:
        void open();
        void close();
        void set_title(std::string t);
        void set_text(std::string t);
        int add_file_menu_option(std::string t);
        bool file_menu_item_clicked(int id);
        std::string get_text();
        nlohmann::json get_cursor_position();
        void init();
        void render();
    private:
        std::vector<std::string> file_menu_options;
        TextEditor editor;
        std::string title;
        std::string text;
        bool is_open;
        int file_menu_clicked_item = -1;
};

#endif