var window_one;
var window_two;
var test_text;
var test_checkbox;
var test_slider;
function setup()
{
	//print(JSON.stringify(http_get("www.melronelectronics.com", 80, "/")) + "\n");
	print(JSON.stringify(serial_list_ports()) + "\n");
	print(JSON.stringify(ini_get("test.ini", "user", "name", "unknown")) + "\n");
	print("Setup!\n");
	create_window(1024, 600, "Test");
	//show_view_controls(true);
	//render_show_crosshair(true);
	window_one = gui_new_window("Test Window");
	test_text = gui_window_add_text(window_one, "Window one text!");
	window_two = gui_new_window("Test Window Two");
	test_checkbox = gui_window_add_checkbox(window_two, "Show stuff?", false);
	test_slider = gui_window_add_slider(window_two, "Range", 0, -100, 100);
	gui_window_add_text(window_two, "Window two text!!!");
}
var count = 0;
function loop()
{
	//print("Loop: " + count + "\n");
	gui_window_set_text(window_one, test_text, "Loop: " + count + "\n" + "checkbox: " + gui_window_get_checkbox(window_two, test_checkbox) + "\n" + gui_window_get_slider(window_two, test_slider));
	count++;
}
