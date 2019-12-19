var debug_window = {};
var debug_window = {};

var dro_data = { X_MCS: 0.0000, Y_MCS: 0.0000, VELOCITY: 0.0, THC_ARC_VOLTAGE: 0.0, THC_SET_VOLTAGE: 0.0, STATUS: 0 };

function parse_serial_line(line)
{
	//console.log(line + "\n");
	if (line.includes("DRO:"))
	{
		var dro_line = line.split("DRO: ")[1];
		var dro_pairs = dro_line.split(" ");
		for (var x = 0; x < dro_pairs.length; x++)
		{
			var pair = dro_pairs[x].split("=");
			var key = pair[0];
			var value = pair[1];
			//console.log("key=" + pair[0] + " value=" + pair[1]);
			if (key == "X_MCS")
			{
				dro_data.X_MCS = parseFloat(value);
			}
			if (key == "Y_MCS")
			{
				dro_data.Y_MCS = parseFloat(value);
			}
			if (key == "VELOCITY")
			{
				dro_data.VELOCITY = parseFloat(value);
			}
			if (key == "THC_SET_VOLTAGE")
			{
				dro_data.THC_SET_VOLTAGE = parseFloat(value);
			}
			if (key == "THC_ARC_VOLTAGE")
			{
				dro_data.THC_ARC_VOLTAGE = parseFloat(value);
			}
			if (key == "UNITS")
			{

			}
			if (key == "STATUS")
			{
				STATUS = value;
			}
		}
	}	
}

function parse_gcode(gcode_file)
{
	render.clear();
	var timestamp = time.millis();
	var last_pointer = { x: 0, y: 0 };
	var pointer = { x: 0, y: 0 };
	console.log("Parsing Gcode: " + gcode_file + "\n");
	if (gcode.parse_file(gcode_file))
	{
		for (var x = 0; x < gcode.size(); x++)
		{
			var block = gcode.get(x);
			pointer = {x: block.x, y: block.y};
			if (block.g == 0)
			{
				last_pointer = { x: pointer.x, y: pointer.y };
			}
			if (block.g == 1)
			{
				render.add_entity({ type: "line", start: {x: last_pointer.x, y: last_pointer.y}, end: {x: pointer.x, y: pointer.y}, color: { r: 1, g: 1, b: 1} });
				last_pointer = { x: pointer.x, y: pointer.y };
			}
		}
	}
	console.log("Parsed Gcode in " + (time.millis() - timestamp) + "ms\n");
}

var control_window = {};
var dro_window = {};
var menu = {};
var fox;
function setup()
{
	//print(JSON.stringify(http_get("www.melronelectronics.com", 80, "/")) + "\n");
	console.log(JSON.stringify(serial.list_ports()) + "\n");
	console.log(JSON.stringify(ini.get("test.ini", "user", "name", "unknown")) + "\n");
	//console.log("Setup!\n");
	create_window(1024, 600, "ncPilot");
	render.set_style({ background_color: { r: 0, g: 0, b: 0.2, a: 1 }});
	render.show_crosshair({ visable: true });
	debug_window.window = gui.new_window("Debug Window");
	debug_window.test_text = gui.add_text(debug_window.window, "Window one text!");
	debug_window.test_checkbox = gui.add_checkbox(debug_window.window, "Show stuff?", false);
	debug_window.test_slider = gui.add_slider(debug_window.window, "Range", 0, -100, 100);
	debug_window.button = gui.add_button(debug_window.window, "Test Button");
	gui.add_text(debug_window.window, "Window two text!!!");

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
	dro_window.x_label = gui.add_text(dro_window.window, "X:      ");
	gui.set_text_style(dro_window.window, dro_window.x_label, { size: 0.6, color: {r: 1, g: 0, b: 0 }});
	gui.sameline(dro_window.window);
	dro_window.x_dro = gui.add_text(dro_window.window, "0.0000");
	gui.set_text_style(dro_window.window, dro_window.x_dro, { size: 0.6, color: {r: 0, g: 1, b: 0 }});
	gui.sameline(dro_window.window);
	dro_window.x_dro_abs = gui.add_text(dro_window.window, "ABS: ");
	gui.set_text_style(dro_window.window, dro_window.x_dro_abs, { size: 0.350, color: {r: 0.3, g: 0.3, b: 0.3 }});
	gui.sameline(dro_window.window);
	dro_window.x_abs_dro = gui.add_text(dro_window.window, "0.0000");
	gui.set_text_style(dro_window.window, dro_window.x_abs_dro, { size: 0.350, color: {r: 0.74, g: 0.458, b: 0.03 }});
	gui.separator(dro_window.window);
	dro_window.y_label = gui.add_text(dro_window.window, "Y:      ");
	gui.set_text_style(dro_window.window, dro_window.y_label, { size: 0.6, color: {r: 1, g: 0, b: 0 }});
	gui.sameline(dro_window.window);
	dro_window.y_dro = gui.add_text(dro_window.window, "0.0000");
	gui.set_text_style(dro_window.window, dro_window.y_dro, { size: 0.6, color: {r: 0, g: 1, b: 0 }});
	gui.sameline(dro_window.window);
	dro_window.y_dro_abs = gui.add_text(dro_window.window, "ABS: ");
	gui.set_text_style(dro_window.window, dro_window.y_dro_abs, { size: 0.350, color: {r: 0.3, g: 0.3, b: 0.3 }});
	gui.sameline(dro_window.window);
	dro_window.y_abs_dro = gui.add_text(dro_window.window, "0.0000");
	gui.set_text_style(dro_window.window, dro_window.y_abs_dro, { size: 0.350, color: {r: 0.74, g: 0.458, b: 0.03 }});
	gui.separator(dro_window.window);
	dro_window.arc_label = gui.add_text(dro_window.window, "ARC:    ");
	gui.set_text_style(dro_window.window, dro_window.arc_label, { size: 0.6, color: {r: 1, g: 1, b: 0 }});
	gui.sameline(dro_window.window);
	dro_window.arc_dro = gui.add_text(dro_window.window, "0.0000");
	gui.set_text_style(dro_window.window, dro_window.arc_dro, { size: 0.6, color: {r: 0, g: 1, b: 0 }});
	gui.sameline(dro_window.window);
	dro_window.arc_set_label = gui.add_text(dro_window.window, "SET: ");
	gui.set_text_style(dro_window.window, dro_window.arc_set_label, { size: 0.350, color: {r: 0.3, g: 0.3, b: 0.3 }});
	gui.sameline(dro_window.window);
	dro_window.arc_set_dro = gui.add_text(dro_window.window, "0.0");
	gui.set_text_style(dro_window.window, dro_window.arc_set_dro, { size: 0.350, color: {r: 0.74, g: 0.458, b: 0.03 }});
	gui.separator(dro_window.window);
	dro_window.status_label = gui.add_text(dro_window.window, "STATUS: ");
	gui.set_text_style(dro_window.window, dro_window.status_label, { size: 0.6, color: {r: 0, g: 1, b: 1 }});
	gui.sameline(dro_window.window);
	dro_window.status_text = gui.add_text(dro_window.window, "Halt");
	gui.set_text_style(dro_window.window, dro_window.status_text, { size: 0.6, color: {r: 0, g: 1, b: 0 }});
	gui.sameline(dro_window.window);
	dro_window.feed_label = gui.add_text(dro_window.window, "    FEED:");
	gui.set_text_style(dro_window.window, dro_window.feed_label, { size: 0.350, color: {r: 0.3, g: 0.3, b: 0.3 }});
	gui.sameline(dro_window.window);
	dro_window.feed_text = gui.add_text(dro_window.window, "0.0");
	gui.set_text_style(dro_window.window, dro_window.feed_text, { size: 0.350, color: {r: 0.74, g: 0.458, b: 0.03 }});


	menu.file = {};
	menu.file.menu = window_menu.create("File");
	menu.file.open = window_menu.add_button(menu.file.menu, "Open");
	menu.file.save = window_menu.add_button(menu.file.menu, "Save");
	menu.file.save_as = window_menu.add_button(menu.file.menu, "Save As");
	menu.file.checkbox = window_menu.add_checkbox(menu.file.menu, "Fullscreen", false);
	menu.file.close = window_menu.add_button(menu.file.menu, "Close");
	menu.edit = {};
	menu.edit.menu = window_menu.create("Edit");
	menu.edit.copy = window_menu.add_button(menu.edit.menu, "Copy");
	menu.edit.paste = window_menu.add_button(menu.edit.menu, "Paste");
	menu.view = {};
	menu.view.menu = window_menu.create("View");
	menu.view.cnc_controls = window_menu.add_checkbox(menu.view.menu, "CNC Controls", true);
	menu.view.cnc_dro = window_menu.add_checkbox(menu.view.menu, "CNC DRO", true);
	menu.view.view_controls = window_menu.add_checkbox(menu.view.menu, "View Controls", false);
	menu.view.debug = window_menu.add_checkbox(menu.view.menu, "Debug", false);

	render.add_entity({ type: "rectangle", size: {x: 45.5, y: 45.5}, color: {r: 0.3, g: 0.3, b: 0.3} });

	render.add_entity({ type: "text", text: "This is a text test!", height: 0.125 });
	fox = render.add_entity({ type: "text", position: {x: 0, y: 0.175}, text: "the little red fox jumps over the hill!", height: 0.125});

	motion_control.set_port("Arduino");
	render.set_loop_delay(30);
}
var count = 0;
var a_once = false;
var c_once = false;
var is_connected = false;
var loop_time = time.millis();
function loop()
{
	var dro = motion_control.get_dro();
	console.log("DRO: " + JSON.stringify(dro) + "\n");
	var scroll = render.get_scroll();
	if (scroll.horizontal != 0 || scroll.vertical != 0)
	{
		//console.log(JSON.stringify(scroll) + "\n");
		render.pan((scroll.horizontal * 0.05) / render.get_zoom(), (scroll.vertical * -0.05) / render.get_zoom());
	}
	if (window_menu.get_button(menu.file.menu, menu.file.close))
	{
		exit(0);
	}
	show_view_controls(window_menu.get_checkbox(menu.view.menu, menu.view.view_controls));
	if (window_menu.get_checkbox(menu.view.menu, menu.view.cnc_controls))
	{
		gui.show(control_window.window, true);
	}
	else
	{
		gui.show(control_window.window, false);
	}
	if (window_menu.get_checkbox(menu.view.menu, menu.view.debug))
	{
		gui.show(debug_window.window, true);
	}
	else
	{
		gui.show(debug_window.window, false);
	}
	if (window_menu.get_checkbox(menu.view.menu, menu.view.cnc_dro))
	{
		gui.show(dro_window.window, true);
	}
	else
	{
		gui.show(dro_window.window, false);
	}
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
		if (key.char == "=")
		{
			render.zoom(0.05 * render.get_zoom());
		}
		if (key.char == "-")
		{
			render.zoom(-(0.05 * render.get_zoom()));
		}
		if (key.keycode == 265) //Up
		{
			render.pan(0, ((0.01 / render.get_zoom())));
		}
		if (key.keycode == 264) //Down
		{
			render.pan(0, (-(0.01 / render.get_zoom())));
		}
		if (key.keycode == 263) //Left
		{
			render.pan((-(0.01 / render.get_zoom())), 0);
		}
		if (key.keycode == 262) //Right
		{
			render.pan(((0.01 / render.get_zoom())), 0);
		}
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
			motion_control.send("G0 X10 Y10");
			motion_control.send("G0 X1 Y1");
			motion_control.send("G0 X0 Y0");
		}
	}
	if (gui.get_button(debug_window.window, debug_window.button))
	{
		console.log("Button Pressed!\n");
		render.clear();
		live_render.clear();
		parse_gcode();
		//var input_text = gui.get_input_text(control_window.window, control_window.input_text);
		//console.log("Input Text: " + input_text + "\n");
		//var input_double = gui.get_input_double(control_window.window, control_window.input_double);
		//console.log("Input Double: " + input_double + "\n");
	}
	if (window_menu.get_button(menu.file.menu, menu.file.open))
	{
		parse_gcode(file_dialog.open({ filter: ["*.nc", "*.ngc"]}));
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
	render.show_crosshair({ pos: {x: dro_data.X_MCS, y: dro_data.Y_MCS} });
	gui.set_text(debug_window.window, debug_window.test_text, "Loop Time: " + (time.millis() - loop_time) + "\nLoop Count: " + count + "\n" + "checkbox: " + gui.get_checkbox(debug_window.window, debug_window.test_checkbox) + "\nslider: " + gui.get_slider(debug_window.window, debug_window.test_slider) + "\nfullscreen: " + window_menu.get_checkbox(menu.file.menu, menu.file.checkbox) + "\nMousePos: (" + gui.get_mouse().x + ", " + gui.get_mouse().y + ")\nRenderMousePos: (" + render.get_mouse().x.toFixed(4) + ", " +  render.get_mouse().y.toFixed(4) + ")");
	
	gui.set_text(dro_window.window, dro_window.x_dro, dro_data.X_MCS.toFixed(4));
	gui.set_text(dro_window.window, dro_window.y_dro, dro_data.Y_MCS.toFixed(4));
	gui.set_text(dro_window.window, dro_window.x_abs_dro, dro_data.X_MCS.toFixed(4));
	gui.set_text(dro_window.window, dro_window.y_abs_dro, dro_data.Y_MCS.toFixed(4));
	gui.set_text(dro_window.window, dro_window.arc_dro, dro_data.THC_ARC_VOLTAGE.toFixed(2));
	gui.set_text(dro_window.window, dro_window.feed_text, dro_data.VELOCITY.toFixed(2));
	gui.set_text(dro_window.window, dro_window.arc_set_dro, gui.get_slider(control_window.window, control_window.thc_set_voltage).toFixed(2));
	count++;
	loop_time = time.millis();
}
