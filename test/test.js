var menu = {};
var window_one = {};
var is_connected = false;
var timer = 0;
function setup()
{
	timer = time.millis();
}
function loop()
{
	if ((timer + 999) < time.millis())
	{
		console.log("(" + system.os() + ") Timer Fired: " + time.millis() + " " + time.date("%h:%m") + "\n");
		timer = time.millis();
	}
}
