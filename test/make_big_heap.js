var big = [];
function setup()
{
    for (var x = 0; x < 1200; x++)
    {
        big.push(file.get_contents("/tmp/100.meg"));
        console.log("Pushing file contents to array[" + x + "]!\n");
    }
    big = null;
    console.log("Setting huge array to null!\n");
    while(1);
}
function loop()
{
    exit(0);
}