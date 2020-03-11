this.file_menu = {};
function setup()
{
    create_window(1024, 600, "Text Editor");
    render.set_style({ background_color: { r: 0, g: 0, b: 0.2, a: 1 }});

    text_editor.open();

    /*this.file_menu.file = {};
	this.file_menu.file.menu = window_menu.create("File");
	this.file_menu.file.open = window_menu.add_button(this.file_menu.file.menu, "Open");
	this.file_menu.file.close = window_menu.add_button(this.file_menu.file.menu, "Close");*/
}
function loop()
{

}