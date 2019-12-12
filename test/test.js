var menu = {};
var window_one = {};
var is_connected = false;
var timer = 0;
function setup()
{
	var count = 0;
	if (file.open("test/1.nc", "r"))
	{
		while(file.lines_available())
		{
			file.read();
			count++;
		}
	}
	else
	{
		console.log("Could not open file!\n");
	}
	file.close();
	timer = time.millis();
	//console.log(file_dialog.save({filter:["*.text", "*.txt"]}) + "\n");
	if (gcode.parse_file("test/1.nc"))
	{
		for (var x = 0; x < gcode.size(); x++)
		{
			//console.log(JSON.stringify(gcode.get(x)) + "\n");
		}
	}
	console.log("Parsed " + count + " lines of Gcode in " + (time.millis() - timer) + "ms\n");
	exit(0);
}
function loop()
{
	if ((timer + 999) < time.millis())
	{
		console.log("(" + system.os() + ") Timer Fired: " + time.millis() + " " + time.date("%h:%m") + "\n");
		timer = time.millis();
	}
}
