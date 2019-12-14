function setup()
{
	create_window(1024, 600, "ncPilot");
	render.set_style({ background_color: { r: 0, g: 0, b: 0.2, a: 1 }});

	var dxf_file = dxf.parse("test/test.dxf");
	for (var x = 0; x < dxf_file.length; x++)
	{
		render.add_entity(dxf_file[x]);
	}
}
function loop()
{
	var scroll = render.get_scroll();
	if (scroll.horizontal != 0 || scroll.vertical != 0)
	{
		//console.log(JSON.stringify(scroll) + "\n");
		render.pan((scroll.horizontal * 0.05) / render.get_zoom(), (scroll.vertical * -0.05) / render.get_zoom());
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
	}
}
