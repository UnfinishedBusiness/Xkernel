function setup()
{
	/*var array = serial_list_ports();
	for (var x = 0; x < array.length; x++)
	{
		print("array: " + array[x] + "\n");
	}*/
	var array = serial_list_ports();
	print(JSON.stringify(array) + "\n");
	create_window(1024, 600, "Test");
}
function loop()
{
	close_window();
}
