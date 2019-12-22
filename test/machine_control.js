var run_once = true;
var last_position = { x: 0, y: 0 };
function setup()
{
	motion_control.set_port("Arduino");
	motion_control.set_dro_interval(75);
}
function loop()
{
	if (motion_control.is_connected())
	{
		if (run_once == true)
		{
			console.log("Sending Gcodes!\n");
			run_once = false;
			motion_control.set_work_offset({ x: 0, y: 0});
			motion_control.send("G0 Y10");
			motion_control.send("G0 Y1");
		}
		//console.log(JSON.stringify(motion_control.get_dro()) + "\n");
		var dro = motion_control.get_dro();
		if (last_position.x != dro.MCS.x || last_position.y != dro.MCS.y)
		{
			console.log("X: " + dro.MCS.x + " Y: " + dro.MCS.y + "\n");
		}
		last_position = { x: dro.MCS.x, y: dro.MCS.y };
	}		
}
