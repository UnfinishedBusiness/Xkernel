var click_once = false;
function setup()
{
	create_window(1024, 600, "3D View");
	render.set_style({ background_color: { r: 0, g: 0, b: 0.2, a: 1 }});

	render.add_entity({type: "line", start: {x: 0, y: 0, z: 0}, end: {x: 10, y: 0, z: 0}, color: {r: 1, g: 0, b: 0} });
	render.add_entity({type: "line", start: {x: 0, y: 0, z: 0}, end: {x: 0, y: 10, z: 0}, color: {r: 0, g: 1, b: 0} });
	render.add_entity({type: "line", start: {x: 0, y: 0, z: 0}, end: {x: 0, y: 0, z: 10}, color: {r: 0, g: 0, b: 1} });

	render.add_entity({type: "line", start: {x: 0, y: 0, z: 10}, end: {x: 10, y: 0, z: 10}, color: {r: 0, g: 0, b: 1} });
	render.add_entity({type: "line", start: {x: 10, y: 0, z: 10}, end: {x: 10, y: 10, z: 10}, color: {r: 0, g: 0, b: 1} });
	render.add_entity({type: "line", start: {x: 10, y: 10, z: 10}, end: {x: 0, y: 10, z: 10}, color: {r: 0, g: 0, b: 1} });
	render.add_entity({type: "line", start: {x: 0, y: 10, z: 10}, end: {x: 0, y: 0, z: 10}, color: {r: 0, g: 0, b: 1} });

	render.zoom(0.15);
}
function loop()
{
	var scroll = render.get_scroll();
	if (scroll.horizontal != 0 || scroll.vertical != 0)
	{
		//console.log(JSON.stringify(scroll) + "\n");
		//render.pan((scroll.horizontal * 0.05) / render.get_zoom(), (scroll.vertical * -0.05) / render.get_zoom()); //Swipe pan for using MacBook
		var old_zoom = render.get_zoom();
		if (scroll.vertical > 0)
		{
			render.zoom(0.05 * render.get_zoom());
		}
		else
		{
			render.zoom(-0.05 * render.get_zoom());
		}
		var scalechange = (old_zoom - render.get_zoom());
		//console.log("Scale change: " + scalechange + "\n");
		//console.log("Zoom: " + render.get_zoom() + "\n");
		var pan_x = render.get_mouse().x * scalechange;
		var pan_y = render.get_mouse().y * scalechange;
		//console.log("PanX: " + pan_x + "\n");
		//console.log("PanY: " + pan_y + "\n");
		render.pan(pan_x, pan_y);
	}
	var key = gui.get_keyboard();
	if (key.keycode > 0)
	{
		//console.log(JSON.stringify(key) + "\n");
		if (key.char == "W")
		{
			render.rotate_x(1);
		}
		if (key.char == "S")
		{
			render.rotate_x(-1);
		}
		if (key.char == "A")
		{
			render.rotate_y(1);
		}
		if (key.char == "D")
		{
			render.rotate_y(-1);
		}
		if (key.char == "X")
		{
			render.rotate_z(1);
		}
		if (key.char == "Z")
		{
			render.rotate_z(-1);
		}
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
			render.pan(0, ((0.05 / render.get_zoom())));
		}
		if (key.keycode == 264) //Down
		{
			render.pan(0, (-(0.05 / render.get_zoom())));
		}
		if (key.keycode == 263) //Left
		{
			render.pan((-(0.05 / render.get_zoom())), 0);
		}
		if (key.keycode == 262) //Right
		{
			render.pan(((0.05 / render.get_zoom())), 0);
		}
	}
	var m = gui.get_mouse_click();
	//console.log(JSON.stringify(m) + "\n");
	if (m.keycode == 2)
	{
		if (click_once == false)
		{
			click_once = true;
			render.orbit_start();
		}
		else
		{
			render.orbit_drag();
		}
	}
	else
	{
		click_once = false;
	}
	
	//console.log("X: " + m.x + " Y: " + m.y + " Z:" + m.z + "\n");
}
