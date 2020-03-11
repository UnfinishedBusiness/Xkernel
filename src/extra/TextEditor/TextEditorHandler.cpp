#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <deque>
#include <chrono>
#include <cmath>
#include <string>
#include <algorithm>
#include <imgui/imgui.h>
#include <json/json.h>
#include <extra/TextEditor/TextEditor.h>
#include <extra/TextEditor/TextEditorHandler.h>

void TextEditorHandler::open()
{
    this->is_open = true;
}
void TextEditorHandler::close()
{
    this->init();
}
void TextEditorHandler::set_title(std::string t)
{
    this->title = t;
}
void TextEditorHandler::set_text(std::string t)
{
    this->text = t;
    editor.SetText(this->text);
}
std::string TextEditorHandler::get_text()
{
    return editor.GetText();
}
nlohmann::json TextEditorHandler::get_cursor_position()
{
    auto cpos = editor.GetCursorPosition();
    nlohmann::json ret;
    ret["line"] = cpos.mLine;
    ret["column"] = cpos.mColumn;
    return ret;
}
int TextEditorHandler::add_file_menu_option(std::string t)
{
    this->file_menu_options.push_back(t);
    return this->file_menu_options.size() - 1;
}
bool TextEditorHandler::file_menu_item_clicked(int id)
{
    if (this->is_open == true)
    {
        if (id == this->file_menu_clicked_item)
        {
            this->file_menu_clicked_item = -1;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
void TextEditorHandler::init()
{
    //auto lang = TextEditor::LanguageDefinition::Lua();
    //editor.SetLanguageDefinition(lang);
    this->show_find_dialog = false;
    this->show_find_replace_dialog = false;
    this->file_menu_options.clear();
    this->is_open = false;
    editor.SetText(this->text);
    this->set_title("Text Editor");
}
void TextEditorHandler::render()
{
    if (this->is_open == true)
    {
        if (this->show_find_dialog == true)
        {
            ImGui::Begin("Find");
            //Should autofil if theres currently a line selected
            if (std::string(this->find_text_input) == "")
            {
                sprintf(this->find_text_input, "%s", editor.GetSelectedText().c_str());
            }
            ImGui::InputText("Search", this->find_text_input, IM_ARRAYSIZE(this->find_text_input));
            ImGui::SameLine();
            if (ImGui::Button(">"))
            {
                std::string search = std::string(this->find_text_input);
                for (int x = editor.GetCursorPosition().mLine; x < editor.GetTotalLines(); x++)
                {
                    editor.MoveDown();
                    std::string current_line = editor.GetCurrentLineText();
                    if (current_line.find(search) != std::string::npos)
                    {
                        //Our search key exists on this line, now select it!
                        auto select_begin = editor.GetCursorPosition();
                        auto select_end = editor.GetCursorPosition();
                        for (int i = 0; i < current_line.size(); i++)
                        {
                            if (current_line[i] == search.front())
                            {
                                select_begin.mColumn = i;
                            }
                            if (current_line[i] == search.back())
                            {
                                select_end.mColumn = i+1;
                            }
                        }
                        editor.SetSelection(select_begin, select_end);
                        break;
                    }
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Close"))
            {
                this->show_find_dialog = false;
                sprintf(find_text_input, "");
            }
            ImGui::End();
        }
        if (this->show_find_replace_dialog == true)
        {
            ImGui::Begin("Find");
            //Should autofil if theres currently a line selected
            if (std::string(this->find_text_input) == "")
            {
                sprintf(this->find_text_input, "%s", editor.GetSelectedText().c_str());
            }
            ImGui::InputText("Find", this->find_text_input, IM_ARRAYSIZE(this->find_text_input));
            ImGui::InputText("Replace", this->replace_text_input, IM_ARRAYSIZE(this->replace_text_input));
            ImGui::SameLine();
            if (ImGui::Button(">"))
            {
                std::string search = std::string(this->find_text_input);
                for (int x = editor.GetCursorPosition().mLine; x < editor.GetTotalLines(); x++)
                {
                    editor.MoveDown();
                    std::string current_line = editor.GetCurrentLineText();
                    if (current_line.find(search) != std::string::npos)
                    {
                        //Our search key exists on this line, now select it!
                        auto select_begin = editor.GetCursorPosition();
                        auto select_end = editor.GetCursorPosition();
                        for (int i = 0; i < current_line.size(); i++)
                        {
                            if (current_line[i] == search.front())
                            {
                                select_begin.mColumn = i;
                            }
                            if (current_line[i] == search.back())
                            {
                                select_end.mColumn = i+1;
                            }
                        }
                        editor.SetSelection(select_begin, select_end);
                        editor.Delete();
                        editor.InsertText(this->replace_text_input);
                        break;
                    }
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("All"))
            {
                std::string search = std::string(this->find_text_input);
                for (int x = 0; x < editor.GetTotalLines(); x++)
                {
                    editor.MoveDown();
                    std::string current_line = editor.GetCurrentLineText();
                    if (current_line.find(search) != std::string::npos)
                    {
                        //Our search key exists on this line, now select it!
                        auto select_begin = editor.GetCursorPosition();
                        auto select_end = editor.GetCursorPosition();
                        for (int i = 0; i < current_line.size(); i++)
                        {
                            if (current_line[i] == search.front())
                            {
                                select_begin.mColumn = i;
                            }
                            if (current_line[i] == search.back())
                            {
                                select_end.mColumn = i+1;
                            }
                        }
                        editor.SetSelection(select_begin, select_end);
                        editor.Delete();
                        editor.InsertText(this->replace_text_input);
                    }
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Close"))
            {
                this->show_find_replace_dialog = false;
                sprintf(find_text_input, "");
            }
            ImGui::End();
        }
        auto cpos = editor.GetCursorPosition();
        ImGui::Begin(this->title.c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
       // ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    for (int x = 0; x < this->file_menu_options.size(); x++)
                    {
                        if (ImGui::MenuItem(this->file_menu_options[x].c_str()))
                        {
                            file_menu_clicked_item = x;
                        }
                    }
                    if (ImGui::MenuItem("Close", "Alt-F4")) this->close();
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Edit"))
                {
                    bool ro = editor.IsReadOnly();
                    if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
                        editor.SetReadOnly(ro);
                    ImGui::Separator();

                    if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
                        editor.Undo();
                    if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
                        editor.Redo();

                    ImGui::Separator();

                    if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
                        editor.Copy();
                    if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
                        editor.Cut();
                    if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
                        editor.Delete();
                    if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
                        editor.Paste();

                    ImGui::Separator();

                    if (ImGui::MenuItem("Select all", nullptr, nullptr))
                        editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

                    if (ImGui::MenuItem("Find", "Ctrl-F"))
                    {
                        this->show_find_dialog = true;
                    }
                    if (ImGui::MenuItem("Find & Replace", "Ctrl-H"))
                    {
                        this->show_find_replace_dialog = true;
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("View"))
                {
                    if (ImGui::MenuItem("Dark palette"))
                        editor.SetPalette(TextEditor::GetDarkPalette());
                    if (ImGui::MenuItem("Light palette"))
                        editor.SetPalette(TextEditor::GetLightPalette());
                    if (ImGui::MenuItem("Retro blue palette"))
                        editor.SetPalette(TextEditor::GetRetroBluePalette());
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
        }
        /*ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
                editor.IsOverwrite() ? "Ovr" : "Ins",
                editor.CanUndo() ? "*" : " ",
                editor.GetLanguageDefinition().mName.c_str(), "test.nc");*/

        editor.Render("TextEditor");
        ImGui::End();
    }
}