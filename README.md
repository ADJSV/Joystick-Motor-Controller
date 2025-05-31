# Joystick-Motor-Controller
Controlling a step servo motor using a joytick from the TI Booster pack for the MSP430  
04/18/2022  
Note this is a reconstructed project. The code is lost so parts of it are missing such as the DEFINES and global variables  
Additional credits to Evan Cloutier, Alejandro Rodriguez and Caleb Butler
# Problem Description
So, a 2 kg mass must be lifted 1 m over 3 seconds with our motor (100 mm diameter):

Minimum Torque requirement for your motor.
T = (0.05 m)(2 kg)(10 m/s^2) = 1 Nm
Minimum Power requirement for your motor.
P = 8.36 W
Your motor speed in rpm:
80 rpm
Motor step rate in steps/second.
1.33 rps
Time delay between steps
3.75 ms
Motor Digikey part # 
2300-103H7128-5740-ND
https://www.digikey.com/en/products/detail/sanyo-denki-sanmotion-products/103H7128-5740/13533531

# Project Description
We interfaced these inputs through the boosterpack connector pins. We determined what pins to use through careful reading of the MKII hardware documentation and MSP430 hardware documentation.
To interface the joystick with the MCU, we used:
 MY_JOYSTICK_BUTTON_PORT         GPIO_PORT_P6  
 MY_JOYSTICK_BUTTON_PIN          GPIO_PIN2  
 MY_JOYSTICK_X_PORT              GPIO_PORT_P3  
 MY_JOYSTICK_X_PIN               GPIO_PIN3  
 MY_JOYSTICK_Y_PORT              GPIO_PORT_P1  
 MY_JOYSTICK_Y_PIN               GPIO_PIN2  

To process all user input captured, we created a function called ‘processInputs’ that accepts one parameter and returns a certain motor mode. The various modes include: CW Fast, CW Slow, CCW Fast, CCW Slow, as well as STD Pattern Mode and DEBUG Pattern Mode.
Depending on the input entered, the function determines the Y position of the joystick, then the X position, then the joystick pushbutton, and then finally the MKII pushbuttons are checked if they are pressed or not; this is all accomplished with a series of if/else statements. 

To control the stepper motor, our design uses two functions, myMotorDriver and myMotorController; myMotorDriver was taken from laboratory session 10. The function myMotorController is not from laboratory 10 but has been created to work best for this project. The function myMotorDriver 
is given a sequence number and sets the magnets within the stepper motor according to that sequence number. The function myMotorController is given an argument that is either NO STEP, CLOCKWISE STEP, or COUNTER-CLOCKWISE STEP, and calls myMotorDriver so that the stepper motor moves the corresponding half step.

# Project Analysis
Our current design uses polling to capture inputs. An alternative could be responding to each input interrupt–for the ADC and for the push-buttons. This might improve the program’s performance, but it would certainly add complexity.
 The input sensor data is processed by the ADC. This is required for all possible designs of the program. However, the ADC has many configuration options and there are many ways one could configure it differently than how we did in this project. If we intended for this program to use the least amount of power,
 we could have configured the ADC clock to be much slower, and only capture joystick inputs every second or many seconds.
 We interact with the stepper motor through the boosterpack pins 3.0-3.3. Alternatively, we could use different pins. We could use a different H-bridge circuit that provides a similar function. We also move the stepper motor in terms of half-steps; we could change our design to instead move the stepper motor in full-steps.
 We did not use these methods for our design for the sake of consistency with laboratory session 10 and for simplicity. For example, we mostly used polling to capture input from the MSP430 in order to simplify our design and not have to go between many interrupt service routines. We used the ADC the same way we were taught in laboratory session 10; 
 we interacted with the stepper motor the same way we were taught as well. This meant we could reuse our understanding and our code from laboratory session 10.

# Module Descriptions
MAIN: For the main function, we stop the watchdog timer. Initialize all the global variables defined such as the SlowHelpingVar, CurrentPositionForPatternMode, CurrentPatternIndex, ToggleLEDHelper, PatternDelayHelper, HasPatternCompleted, InPatternStandardMode. After Initializing the global variables, we initialize the configIO() function, we start 
 the timer A counter, and then go into low power mode. Finally, we create the spin loop.

myMotorDriver: The myMotorDriver uses a switch-case statement to simulate the stepper motor sequence. The sequence consists of 7 different types of movements from case 0 through case 7.

myMotorController: the function starts by defining a static local variable placeInSequence. Then we created an if-else statement that defines the type of movement the stepper motor will take depending on the type of action given. If the movement type has no step, then do nothing. If the movement type was clockwise, then move clockwise and increase the sequence.
 Otherwise decrease the sequence. 

doPatternMode: This function starts by declaring the variables clockwiseDistance, counterClockwiseDistance, and the movementType. Then create a if - else statement that specifies the current position of the stepper motor, and depending on the next position the stepper motor will decide to go clockwise or counterclockwise depending on the closest amount of steps 
 from the current position. 
 configIO:
 
The configIO: This functions configure all the inputs and outputs pins from the MKII boosterpack and the MSP430FR555994. From the MKII BoosterPack, we set the PushButtons PBS1, PBS2, and joystick button as input pins with pull up resistors. From the MSP430 we set the LED1 and LED2 as output pins. We initialize the joystick_init(), and the ADC_init() functions, 
 configure Timer A, and set the stepper motor pins as output low pins.

joyStick_init: The function initializes the X axis and the Y axis of the Joystick and sets them as peripheral modules.

ADC_init: The ADC function initializes the parameters for the memory from both joystickX and joystickY. The function initializes the parameters for the ADC12B module and enables it. The function also configures the parameters structure from the JoystickX and joystickY, and clears the interrupt at the end.

configTimerA: The function sets the parameter for Timer A. sets the clocksource, clock divider value, delay value, clears the interrupt, and starts the timer.

captureInputs: The function gets the inputs from both joystickX and JoystickY using the ADC12 analog to digital conversion used in the ADC_init function. The function sets the interrupts from both the joysticks and the pushbuttons from the MKII Boosterpack.

processInputs: The function starts by setting the motor mode to off, and then creates an if-else statement. The if-else statement consists of choosing the inputs and its functions depending on the case. If the joystickY is HIGH, go clockwise fast, if LOW go clockwise slow. If joysticX is HIGH go counterclockwise fast, if LOW go counterclockwise slow. 
 If PBS1 pressed, go clockwise fast, if PBS2 is pressed go counterclockwise slow. If both PBS1 and PBS2 are pressed, or the joystick button is pressed, do pattern mode.
