var menu = {};
var window_one = {};
var is_connected = false;
var timer = 0;
function setup()
{
	timer = time.millis();
	//console.log(file_dialog.save({filter:["*.text", "*.txt"]}) + "\n");
	if (gcode.parse_file("test/gcode_test.ngc"))
	{
		for (var x = 0; x < gcode.size(); x++)
		{
			//console.log(JSON.stringify(gcode.get(x)) + "\n");
		}
	}
	console.log("Parsed Gcode in " + (time.millis() - timer) + "\n");
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
