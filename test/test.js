function setup()
{
	//print(JSON.stringify(http_get("www.melronelectronics.com", 80, "/")) + "\n");
	//print(JSON.stringify(serial_list_ports()) + "\n");
	//print(JSON.stringify(ini_get("test.ini", "user", "name", "unknown")) + "\n");
	print("Setup!\n");
	create_window(1024, 600, "Test");
	//show_view_controls(true);
	//render_show_crosshair(true);
	var window_one = gui_new_window("Test Window");
	gui_window_add_text(window_one, "Window one text!");
	var window_two = gui_new_window("Test Window Two");
	gui_window_add_text(window_two, "Window two text!!!");
}
var count = 0;
function loop()
{
	//print("Loop: " + count + "\n");
	count++;
}
