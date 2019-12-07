var window_one = {};
var window_two = {};


var control_window = {};
var dro_window = {};
var menu = {};
function setup()
{
	//print(JSON.stringify(http_get("www.melronelectronics.com", 80, "/")) + "\n");
	print(JSON.stringify(serial_list_ports()) + "\n");
	print(JSON.stringify(ini_get("test.ini", "user", "name", "unknown")) + "\n");
	print("Setup!\n");
	create_window(1024, 600, "Test");
	//show_view_controls(true);
	//render_show_crosshair(true);
	window_one.window = gui_new_window("Test Window");
	window_one.test_text = gui_window_add_text(window_one.window, "Window one text!");
	window_two.window = gui_new_window("Test Window Two");
	window_two.test_checkbox = gui_window_add_checkbox(window_two.window, "Show stuff?", false);
	window_two.test_slider = gui_window_add_slider(window_two.window, "Range", 0, -100, 100);
	window_two.button = gui_window_add_button(window_two.window, "Test Button");
	gui_window_add_text(window_two.window, "Window two text!!!");

	control_window.window = gui_new_window("Controls");
	control_window.thc_set_voltage = gui_window_add_slider(control_window.window, "THC", 0, 0, 200);
	gui_window_separator(control_window.window);
	control_window.arc_ok_enable = gui_window_add_checkbox(control_window.window, "Arc OK", true);
	gui_window_separator(control_window.window);
	control_window.x_origin = gui_window_add_button(control_window.window, "X=0");
	gui_window_sameline(control_window.window);
	control_window.x_origin = gui_window_add_button(control_window.window, "Y=0");
	control_window.edit = gui_window_add_button(control_window.window, "Edit");
	gui_window_sameline(control_window.window);
	control_window.mdi = gui_window_add_button(control_window.window, "MDI");
	control_window.park = gui_window_add_button(control_window.window, "Park");
	gui_window_sameline(control_window.window);
	control_window.park = gui_window_add_button(control_window.window, "Hold");
	control_window.wpos = gui_window_add_button(control_window.window, "Wpos");
	gui_window_sameline(control_window.window);
	control_window.touch = gui_window_add_button(control_window.window, "Touch");
	control_window.run = gui_window_add_button(control_window.window, "Run");
	gui_window_sameline(control_window.window);
	control_window.stop = gui_window_add_button(control_window.window, "Stop");

	dro_window.window = gui_new_window("DRO");
	gui_window_add_text(dro_window.window, "X:      ");
	gui_window_sameline(dro_window.window);
	dro_window.x_dro = gui_window_add_text(dro_window.window, "0.0000");
	gui_window_sameline(dro_window.window);
	gui_window_add_text(dro_window.window, "ABS: ");
	gui_window_sameline(dro_window.window);
	dro_window.x_abs_dro = gui_window_add_text(dro_window.window, "0.0000");
	gui_window_separator(dro_window.window);
	gui_window_add_text(dro_window.window, "Y:      ");
	gui_window_sameline(dro_window.window);
	dro_window.y_dro = gui_window_add_text(dro_window.window, "0.0000");
	gui_window_sameline(dro_window.window);
	gui_window_add_text(dro_window.window, "ABS: ");
	gui_window_sameline(dro_window.window);
	dro_window.y_abs_dro = gui_window_add_text(dro_window.window, "0.0000");
	gui_window_separator(dro_window.window);
	gui_window_add_text(dro_window.window, "ARC:    ");
	gui_window_sameline(dro_window.window);
	dro_window.arc_dro = gui_window_add_text(dro_window.window, "0.0000");
	gui_window_sameline(dro_window.window);
	gui_window_add_text(dro_window.window, "SET: ");
	gui_window_sameline(dro_window.window);
	dro_window.arc_set_dro = gui_window_add_text(dro_window.window, "0.0000");
	gui_window_separator(dro_window.window);
	gui_window_add_text(dro_window.window, "STATUS: ");
	gui_window_sameline(dro_window.window);
	dro_window.status_text = gui_window_add_text(dro_window.window, "Halt");
	gui_window_sameline(dro_window.window);
	gui_window_add_text(dro_window.window, "  FEED:");
	gui_window_sameline(dro_window.window);
	dro_window.feed_text = gui_window_add_text(dro_window.window, "0.0");


	menu.file = {};
	menu.file.menu = window_create_menu("File");
	menu.file.open = window_add_menu_button(menu.file.menu, "Open");
	menu.file.save = window_add_menu_button(menu.file.menu, "Save");
	menu.file.save_as = window_add_menu_button(menu.file.menu, "Save As");
	menu.file.checkbox = window_add_menu_checkbox(menu.file.menu, "Fullscreen", false);
	menu.edit = {};
	menu.edit.menu = window_create_menu("Edit");
	menu.edit.copy = window_add_menu_button(menu.edit.menu, "Copy");
	menu.edit.paste = window_add_menu_button(menu.edit.menu, "Paste");
}
var count = 0;
function loop()
{
	//print("Loop: " + count + "\n");
	if (gui_window_get_button(window_two.window, window_two.button))
	{
		print("Button Pressed!\n");
	}
	if (window_get_menu_button(menu.file.menu, menu.file.open))
	{
		print("Open File!\n");
	}
	if (window_get_menu_button(menu.file.menu, menu.file.save))
	{
		print("Save File!\n");
	}
	if (window_get_menu_button(menu.edit.menu, menu.edit.copy))
	{
		print("Copy!\n");
	}
	show_view_controls(gui_window_get_checkbox(window_two.window, window_two.test_checkbox));
	gui_window_set_text(window_one.window, window_one.test_text, "Loop: " + count + "\n" + "checkbox: " + gui_window_get_checkbox(window_two.window, window_two.test_checkbox) + "\nslider: " + gui_window_get_slider(window_two.window, window_two.test_slider) + "\nfullscreen: " + window_get_menu_checkbox(menu.file.menu, menu.file.checkbox) + "\n");
	count++;
}
