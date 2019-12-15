var marker_points = [];
function add_point_marker(point)
{
	marker_points.push({ x: point.x, y: point.y });
	render.add_entity({ type: "rectangle", bottom_left: {x: 0.032 + point.x, y: -0.032 + point.y}, size: {x: -0.064, y: 0.064}, color: {r: 0.3, g: 1.0, b: 0.01} });
}
function mark_endpoints()
{
	var points = [];
	for(var x = 0; x < render.stack_size(); x++)
	{
		var entity = render.get_entity(x);
		if (entity != null)
		{
			if (entity.type == "line")
			{
				//console.log(JSON.stringify(entity) + "\n");
				points.push(entity.start);
				//points.push(entity.end);
			}
		}
	}
	for (var x = 0; x < points.length; x++)
	{
		//console.log("Adding point: " + JSON.stringify(points[x]) + "\n");
		//add_point_marker(points[x]);
	}
}
function algorythm()
{
	for(var x = 0; x < render.stack_size(); x++)
	{
		var entity = render.get_entity(x);
		var point = {};
		if (entity != null)
		{
			if (entity.type == "line")
			{
				//console.log(JSON.stringify(entity) + "\n");
				for (var t = 0; t < 1; t += 0.01)
				{
					point.x = entity.start.x * (1-t) + entity.end.x * t;
					point.y = entity.start.y * (1-t) + entity.end.y * t;
					console.log("New point: " + JSON.stringify(point) + "\n");
					add_point_marker(point);
				}
			}
		}
	}
	for(var x = 1; x < marker_points.length - 1; x++)
	{
		console.log("Adding text: " + JSON.stringify(marker_points[x]) + "\n");
		var P1 = { x: marker_points[x].x, y: marker_points[x].x };
		var P2 = { x: marker_points[x-1].x, y: marker_points[x-1].x };
		var P3 = { x: marker_points[x+1].x, y: marker_points[x+1].x };
		var result = Math.atan2(P3.y - P1.y, P3.x - P1.x) - Math.atan2(P2.y - P1.y, P2.x - P1.x);
		result = result * (180.0/3.141592653589793238463);
		result = Math.abs(result)
		if (result == 180) result = 0;
		console.log("Angle: " + result + "\n");
		render.add_entity({ type: "text", position: marker_points[x], text: result.toFixed(2), height: 0.010 });
	}
}
function setup()
{
	create_window(1024, 600, "Segments");
	//render.set_style({ background_color: { r: 0, g: 0, b: 0.2, a: 1 }});
	render.zoom(40);
	
	render.add_entity({ type: "line", start: { x: 0, y: 0}, end: { x: 10, y: 0 }});
	render.add_entity({ type: "line", start: { x: 10, y: 0}, end: { x: 10, y: 10 }});
	render.add_entity({ type: "line", start: { x: 10, y: 10}, end: { x: 0, y: 0 }});

	//add_point_marker({x: 10, y: 10});
	//mark_endpoints();
	algorythm();
}
var key_once = true;
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
			render.zoom(0.125 * render.get_zoom());
		}
		else
		{
			render.zoom(-0.125 * render.get_zoom());
		}
		var scalechange = old_zoom - render.get_zoom();
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
	else
	{
		key_once = false;
	}
}
