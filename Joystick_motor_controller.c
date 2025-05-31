/*Reference
MY_JOYSTICK_BUTTON_PORT GPIO_PORT_P6
MY_JOYSTICK_BUTTON_PIN     GPIO_PIN2
MY_JOYSTICK_X_PORT               GPIO_PORT_P3
MY_JOYSTICK_X_PIN                  GPIO_PIN3
MY_JOYSTICK_Y_PORT              GPIO_PORT_P1
MY_JOYSTICK_Y_PIN                 GPIO_PIN2
*/

// Main function
void main(void)
{
    // Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);
    // Initalize Globals
    SlowHelpingVar = 0;
    CurrentPositionForPatternMode = 0;
    CurrentPatternIndex = 0;
    ToggleLEDHelper = 0;
    PatternDelayHelper = 0;
    HasPatternCompleted = false;
    InPatternStandardMode = false;


    // Configure all IO and activate configuration
    configIO();
    // Start Timer
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);


    //Go into low power mode (Stop MSP from doing anything but reacting to
    // Timer A interrupt)
    __low_power_mode_0();
    __no_operation();


    // Spin Loop (The program should never reach here)
    while (1);
}

void myMotorDriver(uint8_t motorSequence)
{
    switch(motorSequence) {
    case 0:
        GPIO_setOutputHighOnPin(MY_MOTOR_A_PORT, MY_MOTOR_A_PIN);
        GPIO_setOutputLowOnPin(MY_MOTOR_B_BAR_PORT, MY_MOTOR_B_BAR_PIN);
        GPIO_setOutputLowOnPin(MY_MOTOR_B_PORT, MY_MOTOR_B_PIN);
        GPIO_setOutputLowOnPin(MY_MOTOR_A_BAR_PORT, MY_MOTOR_A_BAR_PIN);
        break;
    case 1:
        GPIO_setOutputHighOnPin(MY_MOTOR_A_PORT, MY_MOTOR_A_PIN);
        GPIO_setOutputLowOnPin(MY_MOTOR_B_BAR_PORT, MY_MOTOR_B_BAR_PIN);
        GPIO_setOutputHighOnPin(MY_MOTOR_B_PORT, MY_MOTOR_B_PIN);
        GPIO_setOutputLowOnPin(MY_MOTOR_A_BAR_PORT, MY_MOTOR_A_BAR_PIN);
        break;
    case 2:
        GPIO_setOutputLowOnPin(MY_MOTOR_A_PORT, MY_MOTOR_A_PIN);
        GPIO_setOutputLowOnPin(MY_MOTOR_B_BAR_PORT, MY_MOTOR_B_BAR_PIN);
        GPIO_setOutputHighOnPin(MY_MOTOR_B_PORT, MY_MOTOR_B_PIN);
        GPIO_setOutputLowOnPin(MY_MOTOR_A_BAR_PORT, MY_MOTOR_A_BAR_PIN);
        break;
    case 3:
        GPIO_setOutputLowOnPin(MY_MOTOR_A_PORT, MY_MOTOR_A_PIN);
        GPIO_setOutputHighOnPin(MY_MOTOR_B_BAR_PORT, MY_MOTOR_B_BAR_PIN);
        GPIO_setOutputHighOnPin(MY_MOTOR_B_PORT, MY_MOTOR_B_PIN);
        GPIO_setOutputLowOnPin(MY_MOTOR_A_BAR_PORT, MY_MOTOR_A_BAR_PIN);
        break;
    case 4:
        GPIO_setOutputLowOnPin(MY_MOTOR_A_PORT, MY_MOTOR_A_PIN);
        GPIO_setOutputHighOnPin(MY_MOTOR_B_BAR_PORT, MY_MOTOR_B_BAR_PIN);
        GPIO_setOutputLowOnPin(MY_MOTOR_B_PORT, MY_MOTOR_B_PIN);
        GPIO_setOutputLowOnPin(MY_MOTOR_A_BAR_PORT, MY_MOTOR_A_BAR_PIN);
        break;
    case 5:
        GPIO_setOutputLowOnPin(MY_MOTOR_A_PORT, MY_MOTOR_A_PIN);
        GPIO_setOutputHighOnPin(MY_MOTOR_B_BAR_PORT, MY_MOTOR_B_BAR_PIN);
        GPIO_setOutputLowOnPin(MY_MOTOR_B_PORT, MY_MOTOR_B_PIN);
        GPIO_setOutputHighOnPin(MY_MOTOR_A_BAR_PORT, MY_MOTOR_A_BAR_PIN);
        break;
    case 6:
        GPIO_setOutputLowOnPin(MY_MOTOR_A_PORT, MY_MOTOR_A_PIN);
        GPIO_setOutputLowOnPin(MY_MOTOR_B_BAR_PORT, MY_MOTOR_B_BAR_PIN);
        GPIO_setOutputLowOnPin(MY_MOTOR_B_PORT, MY_MOTOR_B_PIN);
        GPIO_setOutputHighOnPin(MY_MOTOR_A_BAR_PORT, MY_MOTOR_A_BAR_PIN);
        break;
    case 7:
        GPIO_setOutputHighOnPin(MY_MOTOR_A_PORT, MY_MOTOR_A_PIN);
        GPIO_setOutputLowOnPin(MY_MOTOR_B_BAR_PORT, MY_MOTOR_B_BAR_PIN);
        GPIO_setOutputLowOnPin(MY_MOTOR_B_PORT, MY_MOTOR_B_PIN);
        GPIO_setOutputHighOnPin(MY_MOTOR_A_BAR_PORT, MY_MOTOR_A_BAR_PIN);
        break;
    }
}

void myMotorController(MotorMovementTypes movementType)
{
    static uint8_t placeInSequence = 0;


    if (movementType == NO_STEP)
        return;


    if (movementType == CLOCKWISE_STEP) {
        // Increment placeInSequence
        if (placeInSequence == 7)
            placeInSequence = 0;
        else
            placeInSequence++;
    } else {
        // Decrement placeInSequence
        if (placeInSequence == 0)
            placeInSequence = 7;
        else
            placeInSequence--;
    }
    myMotorDriver(placeInSequence);
}

MotorMovementTypes doPatternMode(void)
{
    MotorMovementTypes movementType;


    //variables to calculate distance from current position
    uint8_t clockwiseDistance, counterClockwiseDistance;


    if (CurrentPositionForPatternMode == PATTERN_IN_STEPS[CurrentPatternIndex]) {
        PatternDelayHelper++;
        if (PatternDelayHelper >= 134) {
            PatternDelayHelper = 0;
            // Increment CurrentPatternIndex
            if (CurrentPatternIndex >= sizeof(PATTERN_IN_STEPS) - 1) {
                CurrentPatternIndex = 0;
                HasPatternCompleted = true;
            } else {
                CurrentPatternIndex++;
            }
        }
        // Set movementType to NO_STEP
        movementType = NO_STEP;
    } else {
        // Choose counter clockwise or clockwise based on what is the shortest distance
        if (CurrentPositionForPatternMode > PATTERN_IN_STEPS[CurrentPatternIndex]) {
            clockwiseDistance = PATTERN_IN_STEPS[CurrentPatternIndex] + 200 - CurrentPositionForPatternMode;
            counterClockwiseDistance = CurrentPositionForPatternMode - PATTERN_IN_STEPS[CurrentPatternIndex];
        } else {
            clockwiseDistance = PATTERN_IN_STEPS[CurrentPatternIndex] - CurrentPositionForPatternMode;
            counterClockwiseDistance = CurrentPositionForPatternMode + 200 - PATTERN_IN_STEPS[CurrentPatternIndex];
        }
        if (clockwiseDistance <= counterClockwiseDistance) {
            CurrentPositionForPatternMode++;
            if (CurrentPositionForPatternMode >= 200)
                CurrentPositionForPatternMode = 0;
            movementType = CLOCKWISE_STEP;
        } else {
            if (CurrentPositionForPatternMode == 0)
                CurrentPositionForPatternMode = 199;
            else
                CurrentPositionForPatternMode--;
            movementType = COUNTER_CLOCKWISE_STEP;
        }
    }
    return movementType;
}

void configIO(void)
{
    // Configure MKII Pushbuttons as inputs
    GPIO_setAsInputPinWithPullUpResistor(MY_PBS1_PORT, MY_PBS1_PIN);
    GPIO_setAsInputPinWithPullUpResistor(MY_PBS2_PORT, MY_PBS2_PIN);


    // Configure Stepper Motor as output and set all pins to low
    GPIO_setAsOutputPin(MY_MOTOR_A_PORT, MY_MOTOR_A_PIN);
    GPIO_setAsOutputPin(MY_MOTOR_B_PORT, MY_MOTOR_B_PIN);
    GPIO_setAsOutputPin(MY_MOTOR_A_BAR_PORT, MY_MOTOR_A_BAR_PIN);
    GPIO_setAsOutputPin(MY_MOTOR_B_BAR_PORT, MY_MOTOR_B_BAR_PIN);


    GPIO_setOutputLowOnPin(MY_MOTOR_A_PORT, MY_MOTOR_A_PIN);
    GPIO_setOutputLowOnPin(MY_MOTOR_B_PORT, MY_MOTOR_B_PIN);
    GPIO_setOutputLowOnPin(MY_MOTOR_A_BAR_PORT, MY_MOTOR_A_BAR_PIN);
    GPIO_setOutputLowOnPin(MY_MOTOR_B_BAR_PORT, MY_MOTOR_B_BAR_PIN);


    // Configure MSP430 LEDs as outputs and set all pins to low
    GPIO_setAsOutputPin(MY_LED1_PORT, MY_LED1_PIN);
    GPIO_setAsOutputPin(MY_LED2_PORT, MY_LED2_PIN);
    GPIO_setOutputLowOnPin(MY_LED1_PORT, MY_LED1_PIN);
    GPIO_setOutputLowOnPin(MY_LED2_PORT, MY_LED2_PIN);


    // Configure Joystick Button as input
    GPIO_setAsInputPinWithPullUpResistor(MY_JOYSTICK_BUTTON_PORT, MY_JOYSTICK_BUTTON_PIN);


    // Configure Joystick as peripheral
    joyStick_init();


    // Configure ADC to read Joystick inputs
    ADC_init();


    // Configure Timer A
    configTimerA(MY_TIMER_TYPE, MY_TIMER_PERIOD, MY_TIMER_DIVIDER);
    Timer_A_initUpMode(TIMER_A0_BASE, &MyTimerA);
    Timer_A_enableInterrupt(TIMER_A0_BASE);
    Timer_A_clearTimerInterrupt(TIMER_A0_BASE);


    // Enable global interrupts (set GIE pin to high)
    __enable_interrupt();


    // Activate all configurations
    PMM_unlockLPM5();
}

void joyStick_init(void)
{
    // JoyStick X
    GPIO_setAsPeripheralModuleFunctionInputPin(
            MY_JOYSTICK_X_PORT, MY_JOYSTICK_X_PIN, GPIO_TERNARY_MODULE_FUNCTION);
    // JoyStick Y
    GPIO_setAsPeripheralModuleFunctionInputPin(
            MY_JOYSTICK_Y_PORT, MY_JOYSTICK_Y_PIN, GPIO_TERNARY_MODULE_FUNCTION);
}

void ADC_init(void)
{
    // Declare local variables
    ADC12_B_initParam initParam = {0};
    ADC12_B_configureMemoryParam joyStickXParam = {0};
    ADC12_B_configureMemoryParam joyStickYParam = {0};


    // Initialize the ADC12B Module
    initParam.sampleHoldSignalSourceSelect = ADC12_B_SAMPLEHOLDSOURCE_SC;
    initParam.clockSourceSelect = ADC12_B_CLOCKSOURCE_ADC12OSC;
    initParam.clockSourceDivider = ADC12_B_CLOCKDIVIDER_1;
    initParam.clockSourcePredivider = ADC12_B_CLOCKPREDIVIDER__1;
    initParam.internalChannelMap = ADC12_B_NOINTCH;
    ADC12_B_init(ADC12_B_BASE, &initParam);


    //Enable the ADC12B module
    ADC12_B_enable(ADC12_B_BASE);
    ADC12_B_setupSamplingTimer(ADC12_B_BASE,
      ADC12_B_CYCLEHOLD_16_CYCLES,
      ADC12_B_CYCLEHOLD_4_CYCLES,
      ADC12_B_MULTIPLESAMPLESENABLE);


    //Configure Memory Buffer


    //  JoyStickXParam Structure
    joyStickXParam.memoryBufferControlIndex = ADC12_B_MEMORY_0;
    joyStickXParam.inputSourceSelect = ADC12_B_INPUT_A2;
    joyStickXParam.refVoltageSourceSelect = ADC12_B_VREFPOS_AVCC_VREFNEG_VSS;
    joyStickXParam.endOfSequence = ADC12_B_NOTENDOFSEQUENCE;
    joyStickXParam.windowComparatorSelect = ADC12_B_WINDOW_COMPARATOR_DISABLE;
    joyStickXParam.differentialModeSelect = ADC12_B_DIFFERENTIAL_MODE_DISABLE;
    ADC12_B_configureMemory(ADC12_B_BASE, &joyStickXParam);


    //  JoyStickYParam Structure
    joyStickYParam.memoryBufferControlIndex = ADC12_B_MEMORY_1;
    joyStickYParam.inputSourceSelect = ADC12_B_INPUT_A15;
    joyStickYParam.refVoltageSourceSelect = ADC12_B_VREFPOS_AVCC_VREFNEG_VSS;
    joyStickYParam.endOfSequence = ADC12_B_ENDOFSEQUENCE;
    joyStickYParam.windowComparatorSelect = ADC12_B_WINDOW_COMPARATOR_DISABLE;
    joyStickYParam.differentialModeSelect = ADC12_B_DIFFERENTIAL_MODE_DISABLE;
    ADC12_B_configureMemory(ADC12_B_BASE, &joyStickYParam);


    // Clear Interrupt
    ADC12_B_clearInterrupt(ADC12_B_BASE,0,ADC12_B_IFG1);
}

void configTimerA(uint16_t clockSource, uint16_t delayValue, uint16_t clockDividerValue)
{
    MyTimerA.clockSource = clockSource;
    MyTimerA.clockSourceDivider = clockDividerValue;
    MyTimerA.timerPeriod = delayValue;
    MyTimerA.timerClear = TIMER_A_DO_CLEAR;
    MyTimerA.startTimer = false;
}

void captureInputs(Inputs *inputs)
{
    // Get input for Joystick X and Y
    ADC12_B_startConversion(ADC12_B_BASE,ADC12_B_START_AT_ADC12MEM0,ADC12_B_SEQOFCHANNELS);
    while(ADC12_B_getInterruptStatus(ADC12_B_BASE,0,ADC12_B_IFG1) != ADC12_B_IFG1); // Wait for conversion
    ADC12_B_clearInterrupt(ADC12_B_BASE,0,ADC12_B_IFG1);
    inputs->joystickX = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_0);
    inputs->joystickY = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_1);


    // Get input from PBS1, PBS2, and PBjoyStick
    inputs->pbs1 = GPIO_getInputPinValue(MY_PBS1_PORT, MY_PBS1_PIN);
    inputs->pbs2 = GPIO_getInputPinValue(MY_PBS2_PORT, MY_PBS2_PIN);
    inputs->pbJoystick = GPIO_getInputPinValue(MY_JOYSTICK_BUTTON_PORT, MY_JOYSTICK_BUTTON_PIN);
}

MotorModes processInputs(Inputs *inputs)
{
    MotorModes mode = OFF;


    if (inputs->pbs1 == GPIO_INPUT_PIN_HIGH && inputs->pbs2 == GPIO_INPUT_PIN_HIGH) {
        // If both push buttons are high, it is regular operation
        if (inputs->joystickY >= YHIGH) {
            mode = CLOCKWISE_FAST;
            //added
            GPIO_setOutputLowOnPin(MY_LED1_PORT, MY_LED1_PIN);   //LED1 OFF
            GPIO_setOutputHighOnPin(MY_LED2_PORT, MY_LED2_PIN);  //LED2 ON
        } else if (inputs->joystickY <= YLOW) {
            mode = CLOCKWISE_SLOW;
            GPIO_setOutputLowOnPin(MY_LED1_PORT, MY_LED1_PIN);  //LED1 OFF
            GPIO_setOutputHighOnPin(MY_LED2_PORT, MY_LED2_PIN); //LED2 ON
        } else if (inputs->joystickX >= XHIGH) {
            mode = COUNTER_CLOCKWISE_FAST;
            GPIO_setOutputHighOnPin(MY_LED1_PORT, MY_LED1_PIN); //LED1 ON
            GPIO_setOutputLowOnPin(MY_LED2_PORT, MY_LED2_PIN);  //LED2 OFF
        } else if (inputs->joystickX <= XLOW) {
            mode = COUNTER_CLOCKWISE_SLOW;
            GPIO_setOutputHighOnPin(MY_LED1_PORT, MY_LED1_PIN); //LED1ON
            GPIO_setOutputLowOnPin(MY_LED2_PORT, MY_LED2_PIN);  //LED2 OFF
        } else {
            mode = OFF;
            // If joystick is centered, check if pushbutton is pressed
            if (inputs->pbJoystick == GPIO_INPUT_PIN_LOW) {
                mode = PATTERN_STANDARD_MODE;
                GPIO_setOutputHighOnPin(MY_LED1_PORT, MY_LED1_PIN); //LED1 ON
                GPIO_setOutputHighOnPin(MY_LED2_PORT, MY_LED2_PIN); //LED2 ON
            } else {
                GPIO_setOutputLowOnPin(MY_LED1_PORT, MY_LED1_PIN); //LED1 OFF
                ToggleLEDHelper++;
                if (ToggleLEDHelper >= 134) {
                    ToggleLEDHelper = 0;
                    GPIO_toggleOutputOnPin(MY_LED2_PORT, MY_LED2_PIN); //LED2 Toggle
                }
            }
        }
    } else if (inputs->pbs1 == GPIO_INPUT_PIN_HIGH && inputs->pbs2 == GPIO_INPUT_PIN_LOW) {
        mode = CLOCKWISE_FAST;
    } else if (inputs->pbs1 == GPIO_INPUT_PIN_LOW && inputs->pbs2 == GPIO_INPUT_PIN_HIGH) {
        mode = COUNTER_CLOCKWISE_SLOW;
    } else if (inputs->pbs1 == GPIO_INPUT_PIN_LOW && inputs->pbs2 == GPIO_INPUT_PIN_LOW) {
        mode = PATTERN_STANDARD_MODE;
    }


    return mode;
}
