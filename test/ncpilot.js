var window_one = {};
var window_two = {};

function parse_gcode()
{
	var last_pointer = { x: 0, y: 0 };
	var pointer = { x: 0, y: 0 };
	console.log("Parsing Gcode\n");
	file.open("test/gcode_test.ngc", "r");
	while(file.lines_available())
	{
		var line = file.read();
		//console.log("Line: " + line);
		var split_line = line.split(" ");
		var gWord = split_line[0].replace(/(\r\n|\n|\r)/gm, "");
		var split = [];
		if (split_line[1] != undefined) split[1] = split_line[1].replace(/(\r\n|\n|\r)/gm, "");
		if (split_line[2] != undefined) split[2] = split_line[2].replace(/(\r\n|\n|\r)/gm, "");
		if (split_line[3] != undefined) split[3] = split_line[3].replace(/(\r\n|\n|\r)/gm, "");
		if (gWord.toLowerCase() == "g0")
		{
			for (var x = 0; x < split.length; x++)
			{
				if (split[x] != undefined)
				{
					//console.log("Split[" + x + "] " + split[x] + "\n");
					if (split[x].toLowerCase().startsWith("x"))
					{
						pointer.x = Number(split[x].substr(1));
					}
					if (split[x].toLowerCase().startsWith("y"))
					{
						pointer.y = Number(split[x].substr(1));
					}
					if (split[x].toLowerCase().startsWith("z"))
					{
						pointer.z = Number(split[x].substr(1));
					}
				}	
			}
			//console.log("Found G0: x: " + pointer.x + ", y: " + pointer.y + "\n");
		}
		if (gWord.toLowerCase() == "g1")
		{
			last_pointer = {x: pointer.x, y: pointer.y, z: pointer.z};
			for (var x = 0; x < split.length; x++)
			{
				if (split[x] != undefined)
				{
					//console.log("Split[" + x + "] " + split[x] + "\n");
					if (split[x].toLowerCase().startsWith("x"))
					{
						pointer.x = Number(split[x].substr(1));
					}
					if (split[x].toLowerCase().startsWith("y"))
					{
						pointer.y = Number(split[x].substr(1));
					}
					if (split[x].toLowerCase().startsWith("z"))
					{
						pointer.z = Number(split[x].substr(1));
					}
				}	
			}
			//console.log("Found G1: x: " + pointer.x + ", y: " + pointer.y + "\n");
			//console.log("Line = start-> x: " + last_pointer.x + " y: " + last_pointer.y + " end-> x: " + pointer.x + " y: " + pointer.y + "\n");
			render.add_entity({ type: "line", start: {x: last_pointer.x, y: last_pointer.y}, end: {x: pointer.x, y: pointer.y}, color: { r: 1, g: 0, b: 0} });
		}
		//console.log("'" + gWord + "'\n");
	}
	file.close();
}

var control_window = {};
var dro_window = {};
var menu = {};
function setup()
{
	//print(JSON.stringify(http_get("www.melronelectronics.com", 80, "/")) + "\n");
	console.log(JSON.stringify(serial.list_ports()) + "\n");
	console.log(JSON.stringify(ini.get("test.ini", "user", "name", "unknown")) + "\n");
	console.log("Setup!\n");
	create_window(1024, 600, "ncPilot");
	render.show_crosshair({ visable: true });
	window_one.window = gui.new_window("Test Window");
	window_one.test_text = gui.add_text(window_one.window, "Window one text!");
	window_two.window = gui.new_window("Test Window Two");
	window_two.test_checkbox = gui.add_checkbox(window_two.window, "Show stuff?", false);
	window_two.test_slider = gui.add_slider(window_two.window, "Range", 0, -100, 100);
	window_two.button = gui.add_button(window_two.window, "Test Button");
	gui.add_text(window_two.window, "Window two text!!!");

	control_window.window = gui.new_window("Controls");
	control_window.thc_set_voltage = gui.add_slider(control_window.window, "THC", 0, 0, 200);
	gui.separator(control_window.window);
	control_window.arc_ok_enable = gui.add_checkbox(control_window.window, "Arc OK", true);
	gui.separator(control_window.window);
	control_window.x_origin = gui.add_button(control_window.window, "X=0");
	gui.sameline(control_window.window);
	control_window.x_origin = gui.add_button(control_window.window, "Y=0");
	control_window.edit = gui.add_button(control_window.window, "Edit");
	gui.sameline(control_window.window);
	control_window.mdi = gui.add_button(control_window.window, "MDI");
	control_window.park = gui.add_button(control_window.window, "Park");
	gui.sameline(control_window.window);
	control_window.park = gui.add_button(control_window.window, "Hold");
	control_window.wpos = gui.add_button(control_window.window, "Wpos");
	gui.sameline(control_window.window);
	control_window.touch = gui.add_button(control_window.window, "Touch");
	control_window.run = gui.add_button(control_window.window, "Run");
	gui.sameline(control_window.window);
	control_window.stop = gui.add_button(control_window.window, "Stop");

	dro_window.window = gui.new_window("DRO");
	gui.add_text(dro_window.window, "X:      ");
	gui.sameline(dro_window.window);
	dro_window.x_dro = gui.add_text(dro_window.window, "0.0000");
	gui.sameline(dro_window.window);
	gui.add_text(dro_window.window, "ABS: ");
	gui.sameline(dro_window.window);
	dro_window.x_abs_dro = gui.add_text(dro_window.window, "0.0000");
	gui.separator(dro_window.window);
	gui.add_text(dro_window.window, "Y:      ");
	gui.sameline(dro_window.window);
	dro_window.y_dro = gui.add_text(dro_window.window, "0.0000");
	gui.sameline(dro_window.window);
	gui.add_text(dro_window.window, "ABS: ");
	gui.sameline(dro_window.window);
	dro_window.y_abs_dro = gui.add_text(dro_window.window, "0.0000");
	gui.separator(dro_window.window);
	gui.add_text(dro_window.window, "ARC:    ");
	gui.sameline(dro_window.window);
	dro_window.arc_dro = gui.add_text(dro_window.window, "0.0000");
	gui.sameline(dro_window.window);
	gui.add_text(dro_window.window, "SET: ");
	gui.sameline(dro_window.window);
	dro_window.arc_set_dro = gui.add_text(dro_window.window, "0.0000");
	gui.separator(dro_window.window);
	gui.add_text(dro_window.window, "STATUS: ");
	gui.sameline(dro_window.window);
	dro_window.status_text = gui.add_text(dro_window.window, "Halt");
	gui.sameline(dro_window.window);
	gui.add_text(dro_window.window, "  FEED:");
	gui.sameline(dro_window.window);
	dro_window.feed_text = gui.add_text(dro_window.window, "0.0");


	menu.file = {};
	menu.file.menu = window_menu.create("File");
	menu.file.open = window_menu.add_button(menu.file.menu, "Open");
	menu.file.save = window_menu.add_button(menu.file.menu, "Save");
	menu.file.save_as = window_menu.add_button(menu.file.menu, "Save As");
	menu.file.checkbox = window_menu.add_checkbox(menu.file.menu, "Fullscreen", false);
	menu.edit = {};
	menu.edit.menu = window_menu.create("Edit");
	menu.edit.copy = window_menu.add_button(menu.edit.menu, "Copy");
	menu.edit.paste = window_menu.add_button(menu.edit.menu, "Paste");
}
var count = 0;
var a_once = false;
var c_once = false;
var e;
function loop()
{
	var mouse = gui.get_mouse_click();
	{
		if (mouse.keycode > -1)
		{
			//console.log("keycode: " + mouse.keycode + "\n");
		}
	}
	var key = gui.get_keyboard();
	if (key.keycode > 0)
	{
		//console.log(JSON.stringify(key) + "\n");
		if (key.char == "A" && a_once == false)
		{
			a_once = true;
			render.add_entity({ type: "line", start: {x: 0, y: 0}, end: {x: 10, y: 10}, color: { r: 0, g: 1} });
			//console.log(render.stack_size() + "\n");
			render.show_crosshair({ pos: {x: 1, y: 0} });
		}
		if (key.char == "B")
		{
			e = render.get_entity(0);
			e.color.r = 1;
			e.color.g = 0;
			e.start.x = 30;
			e.start.y = 45.5;
			render.set_entity(0, e);
		}
		if (key.char == "C" && c_once == false)
		{
			c_once = true;
			render.del_entity(0);
		}
	}
	if (gui.get_button(window_two.window, window_two.button))
	{
		console.log("Button Pressed!\n");
		render.clear();
		live_render.clear();
		parse_gcode();
	}
	if (window_menu.get_button(menu.file.menu, menu.file.open))
	{
		console.log("Open File!\n");
	}
	if (window_menu.get_button(menu.file.menu, menu.file.save))
	{
		console.log("Save File!\n");
	}
	if (window_menu.get_button(menu.edit.menu, menu.edit.copy))
	{
		console.log("Copy!\n");
	}
	var mouse = render.get_mouse();
	render.show_crosshair({ pos: {x: mouse.x, y: mouse.y} });
	show_view_controls(gui.get_checkbox(window_two.window, window_two.test_checkbox));
	gui.set_text(window_one.window, window_one.test_text, "Loop: " + count + "\n" + "checkbox: " + gui.get_checkbox(window_two.window, window_two.test_checkbox) + "\nslider: " + gui.get_slider(window_two.window, window_two.test_slider) + "\nfullscreen: " + window_menu.get_checkbox(menu.file.menu, menu.file.checkbox) + "\nMousePos: (" + gui.get_mouse().x + ", " + gui.get_mouse().y + ")\nRenderMousePos: (" + render.get_mouse().x.toFixed(4) + ", " +  render.get_mouse().y.toFixed(4) + ")");
	count++;
}
