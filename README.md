# Basic-GPIO-LED-Controller
The project demonstrates how to control an LED with a button on the Raspberry Pi Zero 2 W through the lipgpiod library in C. 

The code provided will:

•	Set your base flashing speed at .5 seconds on and .5 seconds off.

•	The button has two modes: Long press and short press

  o	The short press (1s<)  will double the blinking frequency with each touch.
 
  o	The long press (1s>) will reset the blinking frequency.
  
You will need the following supplies (Found in any standard starter kit.): 

•	One Solderless Breadboard

•	One LED 

•	One 330 Ohm Resistors

•	Four Jumper Wires

•	One Button

You will want to connect your Raspberry Pi in the manner shown below.

Before running the code provided, you will first want to run these lines on your Raspberry Pi’s.

"sudo apt update"

"sudo apt install libgpiod-dev -y"

Next, do: "nano main.c"

Now, you are ready to paste the code into the file!

To compile the code: "gcc -o main.c -lgpiod"

Then to run: "./main". If that doesn't work, try "sudo ./main".
