var marker_points = [];
function add_point_marker(point)
{
	render.add_entity({ type: "rectangle", bottom_left: {x: 0.005 + point.x, y: -0.005 + point.y}, size: {x: -0.010, y: 0.010}, color: {r: 0.3, g: 1.0, b: 0.01} });
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
	if (render.stack_size() < 1) return;
	var max_jerk = 40;
	//Calculate point chunks and fill marker_points with chunk points
	var chunk_length = 0.015;
	var accel_rate = 15; //Accelerate 15 inches/sec^2
	var min_feed = 5;
	var target_feed = 100;
	for(var x = 0; x < render.stack_size(); x++)
	{
		var entity = render.get_entity(x);
		var point = {};
		if (entity != null)
		{
			if (entity.type == "line")
			{
				//console.log(JSON.stringify(entity) + "\n");
				var px = entity.end.x - entity.start.x;
  				var py = entity.end.y - entity.start.y;
  				var pz = entity.end.z - entity.start.z;
				var line_length = Math.sqrt(px*px + py*py + pz*pz);
				console.log("Line Length: " + line_length + "\n");
				var number_segments = (line_length / chunk_length);
				for (var t = (1 / number_segments); t < 1; t += (1 / number_segments))
				{
					point.x = entity.start.x * (1-t) + entity.end.x * t;
					point.y = entity.start.y * (1-t) + entity.end.y * t;
					//console.log("New point: " + JSON.stringify(point) + "\n");
					marker_points.push({ x: point.x, y: point.y });
					//add_point_marker(point);
				}
				marker_points.push({ x: entity.end.x, y: entity.end.y });
				marker_points[marker_points.length-1].marker = {x: marker_points[marker_points.length-1].x, y: marker_points[marker_points.length-1].y};
			}
		}
	}
	//Itterate through chunk points and calculate the relative angle between last point and next point to determine a directiional change.
	//Calculate the feedrate that we need to be at once we arive at this point based on the angle change
	for(var x = 1; x < marker_points.length-1; x++)
	{
		//console.log("Adding text: " + JSON.stringify(marker_points[x]) + "\n");
		var P1 = { x: marker_points[x].x, y: marker_points[x].y };
		var P2 = { x: marker_points[x-1].x, y: marker_points[x-1].y };
		var P3 = { x: marker_points[x+1].x, y: marker_points[x+1].y };
		var result = Math.atan2(P3.y - P1.y, P3.x - P1.x) - Math.atan2(P2.y - P1.y, P2.x - P1.x);
		result = result * (180.0/3.141592653589793238463);
		result = Math.abs(result)
		if (result > 178 && result < 182) result = 180;
		if (result == 0) result = 180; //Straight line
		if (result > 180) result -= 90;
		if (result < 90) result = 90; //A 90 degree corner will warrent a slowdown to min_feed
		//console.log("Angle: " + result + "\n");
		marker_points[x].angle_change = result;
		var max_corner_feed = FastMath.map(marker_points[x].angle_change, 180, 90, max_jerk, min_feed);
		if (max_corner_feed > target_feed) max_corner_feed = target_feed;
		marker_points[x].max_corner_feed = max_corner_feed; 
		marker_points[x].current_feed = marker_points[x].max_corner_feed;
		//render.add_entity({ type: "text", position: { x: marker_points[x].x, y: marker_points[x].y}, text: marker_points[x].angle_change.toFixed(2) + ">" + marker_points[x].max_corner_feed.toFixed(2), height: 0.001 });
	}
	marker_points[marker_points.length-1].max_corner_feed = 5; //Make sure we decelerate on last point
	marker_points[marker_points.length-1].angle_change = 180;
	marker_points[0].max_corner_feed = min_feed;
	marker_points[0].angle_change = 180;
	//Calculate the feedrate based on acceleration value for each chunk and hit each chunks specified feedrate from the last itteration
	var feed_from_distance = function(accel_rate, distance_into_move)
	{
	  var accel_time = Math.sqrt((0.5 * accel_rate) * distance_into_move) * (1.0/(0.5 * accel_rate));
	  return (accel_rate * accel_time);
	}
	var current_feed = min_feed;
	var accel_per_chunk = feed_from_distance(accel_rate, chunk_length);
	console.log("Accel_per_chunk: " + accel_per_chunk + "\n");
	//Populate all points with a current_feed attribute!
	marker_points[0].current_feed = marker_points[0].max_corner_feed;
	for (var x = 1; x < marker_points.length; x++)
	{
		current_feed += accel_per_chunk;
		if (current_feed > target_feed)
		{
			current_feed = target_feed;
		}
		marker_points[x].current_feed = current_feed;
	}
	//We have to iterate the path forwards until we get to a marker. Then itterate backwards until we get to a point were our feedrate peaks for that segment.
	//Then iterate forward to find the next segment, then itterate backwards until our feedrate peaks, etc
	var next_marker = 0;
	for (var x = 0; x < marker_points.length; x++)
	{
		if (marker_points[x].marker != undefined)
		{
			//Iterate forard from this marker to next marker
			var current_feed;
			if (marker_points[x].current_feed > marker_points[x].max_corner_feed)
			{
				current_feed = marker_points[x].max_corner_feed;
			}
			else
			{
				current_feed = marker_points[x].current_feed;
			}
			for (var i = x + 1; i < marker_points.length; i++)
			{
				current_feed += accel_per_chunk;
				marker_points[i].current_feed = current_feed;
				if (marker_points[i].current_feed > target_feed) marker_points[i].current_feed = target_feed;
				if (marker_points[i].marker != undefined)
				{
					next_marker = i;
					break;
				}
			}
			//Iterate backwards from next marker to this marker
			if (marker_points[next_marker].current_feed > marker_points[next_marker].max_corner_feed)
			{
				var current_feed = marker_points[next_marker].max_corner_feed;
				marker_points[next_marker].current_feed = current_feed;
				for (var i = next_marker -1; i > x; i--)
				{
					current_feed += accel_per_chunk;
					marker_points[i].current_feed = current_feed;
					if (marker_points[i].current_feed >= marker_points[i-1].current_feed)
					{
						break;
					}
				}
			}
			//Iterate from 
		}
	}
	//Only for visualization
	for (var x = 0; x < marker_points.length; x++)
	{
		//console.log(x + ": " + JSON.stringify(marker_points[x]) + "\n");
		render.add_entity({ type: "text", position: {x: marker_points[x].x, y: marker_points[x].y - 0.002}, text: "F: " + marker_points[x].current_feed.toFixed(2), height: 0.001 });
		if (marker_points[x].marker != undefined)
		{
			render.add_entity({ type: "text", position: {x: marker_points[x].marker.x, y: marker_points[x].marker.y - 0.004}, text: "marker", height: 0.001 });
		}
		render.add_entity({ type: "text", position: {x: marker_points[x].x, y: marker_points[x].y}, text: "M: " + marker_points[x].max_corner_feed.toFixed(2), height: 0.001 });
		render.add_entity({ type: "text", position: {x: marker_points[x].x, y: marker_points[x].y + 0.002}, text: "A: " + marker_points[x].angle_change.toFixed(2), height: 0.001 });
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
	gcode.clear();
	console.log("Parsed Gcode in " + (time.millis() - timestamp) + "ms\n");
}
function setup()
{
	create_window(1024, 600, "Segments");
	//render.set_style({ background_color: { r: 0, g: 0, b: 0.2, a: 1 }});
	render.zoom(40);
	
	//riangle Path
	//render.add_entity({ type: "line", start: { x: 0, y: 0}, end: { x: 10, y: 0 }});
	//render.add_entity({ type: "line", start: { x: 10, y: 0}, end: { x: 10, y: 10 }});
	//render.add_entity({ type: "line", start: { x: 10, y: 10}, end: { x: 0, y: 0 }});

	/*render.add_entity({ type: "line", start: { x: 0, y: 0}, end: { x: 1, y: 0 }});
	render.add_entity({ type: "line", start: { x: 1, y: 0}, end: { x: 1, y: 3 }});
	render.add_entity({ type: "line", start: { x: 1, y: 3}, end: { x: 0, y: 5 }});
	render.add_entity({ type: "line", start: { x: 0, y: 5}, end: { x: 2, y: 2 }});
	render.add_entity({ type: "line", start: { x: 2, y: 2}, end: { x: 5, y: 5 }});
	render.add_entity({ type: "line", start: { x: 5, y: 5}, end: { x: 6, y: 5 }});
	render.add_entity({ type: "line", start: { x: 6, y: 5}, end: { x: 10, y: 0 }});
	render.add_entity({ type: "line", start: { x: 10, y: 0}, end: { x: 1, y: 1 }});*/

	//add_point_marker({x: 10, y: 10});
	//mark_endpoints();
	parse_gcode("test/0.nc");
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
			render.pan(0, ((0.001 * render.get_zoom())));
		}
		if (key.keycode == 264) //Down
		{
			render.pan(0, (-(0.001 * render.get_zoom())));
		}
		if (key.keycode == 263) //Left
		{
			render.pan((-(0.001 * render.get_zoom())), 0);
		}
		if (key.keycode == 262) //Right
		{
			render.pan(((0.001 * render.get_zoom())), 0);
		}
	}
	else
	{
		key_once = false;
	}
}
