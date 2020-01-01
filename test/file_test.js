function setup()
{
    if (file.open("test.txt", "w"))
    {
        file.write("This is a file write test!");
        file.close();
        console.log("Writing file!\n");
    }
    if (file.open("test.txt", "r"))
    {
        console.log(file.read());
        file.close();
    }
    exit(0);
}
function loop()
{
    
}