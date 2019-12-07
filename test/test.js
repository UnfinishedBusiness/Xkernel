var menu = {};
var window_one = {};
function setup()
{
	console.log(JSON.stringify(serial.list_ports()) + "\n");
	console.log("test!\n");
	create_window(1024, 600, "Test");
	menu.file = {};
	menu.file.root = window_menu.create("File");
	menu.file.open = window_menu.add_button(menu.file.root, "Open");
	menu.file = window_menu.create("Edit");
	menu.file = window_menu.create("View");

	window_one.window = gui.new_window("Test Window");
	window_one.test_text = gui.add_text(window_one.window, "Window one text!");

	exit(0);
	if (file.open("test/test.js", "r"))
	{
		console.log("Opened file!\n");
	}
	//console.log(file.read() + "\n");
	while(file.lines_available())
	{
		console.log(file.read());
	}
	file.close();

	console.log("\n");
}
function loop()
{
 	//exit(0);
}
