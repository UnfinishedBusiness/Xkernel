function setup()
{
	motion_control.set_baud(115200);
	motion_control.set_port("Arduino");
	motion_control.set_dro_interval(125);
}
function loop()
{
	var dro = motion_control.get_dro();
	console.log(JSON.stringify(dro) + "\n");
}