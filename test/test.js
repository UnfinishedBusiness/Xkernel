var menu = {};
var window_one = {};
var is_connected = false;
function setup()
{
	console.log(JSON.stringify(serial.list_ports()) + "\n");
	if (serial.open("/dev/ttyACM0", 9600))
	{
		console.log("Port is open!\n");
		is_connected = true;
	}
	else
	{
		console.log("Port is not open!\n");
	}
	if (serial.is_open())
	{
		console.log("Port is still open!\n");
	}
	//serial.close();
	if (!serial.is_open())
	{
		console.log("Port has closed!\n");
	}
	//serial.test();
	//console.log(file_dialog.select_folder({default_path: "/"}) + "\n");
}
function loop()
{
	if (serial.is_open())
	{
		var avail = serial.available();
		if (avail > 0)
		{
			console.log(serial.read(avail));
		}
	}
	else
	{
		if (is_connected == true)
		{
			console.log("Serial disconect!\n");
			is_connected = false;
		}
		if (serial.open("/dev/ttyACM0", 9600))
		{
			console.log("Port is open again!\n");
			is_connected = true;
		}
	}
}
