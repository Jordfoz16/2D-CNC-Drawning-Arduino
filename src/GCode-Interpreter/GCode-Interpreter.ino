#include <Servo.h>
#include <Stepper.h>

#define MAX_BUFFER_LENGTH 512

struct vector3
{
    float x;
    float y;
    float z;
};

boolean logging = true;

/////////////////////////////////////
//---------Servo Values--------------
/////////////////////////////////////

//Servo values for pen up and down
int zAxisUP = 20;
int zAxisDOWN = 0;

//Servo pin number
const int servoPin = 6;

Servo penServo;

/////////////////////////////////////
//---------Stepper Values------------
/////////////////////////////////////

const int stepsPerRevolution = 20;

Stepper stepperY(stepsPerRevolution, 5, 4, 3, 2);
Stepper stepperX(stepsPerRevolution, 11, 10, 9, 8);


/////////////////////////////////////
//---------Config Settings-----------
/////////////////////////////////////

float stepInc = 1;
int stepDelay = 1;
int lineDelay = 40;
int penDelay = 50;

float stepsPerMillimeterX = 40.0;
float stepsPerMillimeterY = 40.0;

float xMin = 0;
float xMax = 40;
float yMin = 0;
float yMax = 40;
float zMin = 0;
float zMax = 1;

/////////////////////////////////////
//---------Position Settings---------
/////////////////////////////////////

//Current Position
float xPos = xMin;
float yPos = yMin;
float zPos = zMax;

struct vector3 penPosition;

void setup(){

    Serial.begin(9600);

    //Wait for serial to become available
    while(!Serial);

    //Servo setup
    penServo.attach(servoPin);
    penServo.write(zAxisUP);
    delay(200);

    //Stepper motor setup
    stepperX.setSpeed(1500);
    stepperY.setSpeed(1500);

    startUpMessage();
}

void loop(){

  while(1){
    while(!Serial);
    formatInput();
  }
}

void startUpMessage(){

    Serial.println("Setup Complete");

    //Printing X value range
    Serial.print("X values from ");
    Serial.print(xMin);
    Serial.print(" to ");
    Serial.print(xMax);
    Serial.println(" mm.");

    //Printing Y value range
    Serial.print("Y values from ");
    Serial.print(yMin);
    Serial.print(" to ");
    Serial.print(yMax);
    Serial.println(" mm.");

    Serial.println("Ready for G Code File");
}

void formatInput(){

    delay(200);
    
    char inputLine[MAX_BUFFER_LENGTH];
    char c;
    int lineLength = 0;
    boolean lineComment = false, lineSemiColon = false;

    //Loop while data is being received
    while(Serial.available() > 0){

        //Loads the next character of the line
        c = Serial.read();

        //Checks if c is a newline
        if((c == '\n') || (c == '\r')){
            //Check if the line is longer than 0 characters
            if(lineLength > 0){
                //Replaces new line with \0
                inputLine[lineLength] = '\0';

                if(logging){
                    Serial.print("Line Received: ");
                    Serial.println(inputLine);
                }
                
                //Sends the formatted line for processing
                processLine(inputLine, lineLength);
                lineLength = 0;
            }

            Serial.println("Next Command");
            lineComment = false;
            lineSemiColon = false;

        }else{
            
            //Remove all unnecessary characters
            //Only addes characters that are needed to the array

            if((lineComment) || (lineSemiColon)){
                //Doesnt added comment characters
                if(c == ')') lineComment = false;

            }else{
                if(c <= ' '){
                    //Doesnt add whitespace
                }else if (c == '/'){
                    //Doesnt add slashes
                }else if (c == '('){
                    //Enables the comment flag
                    lineComment = true;
                }else if (c == ';'){
                    //Enable semi colon flag
                    lineSemiColon = true;
                }else if (lineLength >= MAX_BUFFER_LENGTH - 1){
                    Serial.println("ERROR: Buffer Overflow");
                    lineComment = false;
                    lineSemiColon = false;
                }else if (c >= 'a' && c <= 'z'){
                    //Convert the line to uppercase
                    char upper = c - 'a' + 'A';
                    
                    //Add the character to the array
                    inputLine[lineLength] = upper;

                    //Increases line length
                    lineLength++;
                    
                }else{
                    //Character is already uppercase
                    inputLine[lineLength] = c;

                    //Increases line length
                    lineLength++;
                }
            }
        }
    }
}

void processLine(char *inputLine, int lineLength){
    
    int index = 0;

    //Max length for one command
    char buffer[64];

    /*
        Command List
        G - Movement Command
            G00 - Slow Movement
            G10 - Fast Movement
            G20 - Setting position
            G30 - Go Home
        M - Miscellaneous function
    */

   char* indexX;
   char* indexY;
    
   while (index < lineLength){
       
       switch (inputLine[index++])
       {
        
        case 'G':
            //Loads the number after G into buffer
            buffer[0] = inputLine[index++];
            //Puts \0 to end the command
            buffer[1] = '\0';

            //Target position setup
            struct vector3 targetPosition;
            targetPosition.x = 0.0;
            targetPosition.y = 0.0;

            // X must come before Y
            //Gets the x position from the string
            indexX = strchr(inputLine + index, 'X');
            //Gets the y position from the string
            indexY = strchr(inputLine + index, 'Y');

            if(indexY <= 0){
                //If there is no Y then it loads the X
                targetPosition.x = atof(indexX + 1);
                targetPosition.y = penPosition.y;

            }else if(indexX <= 0){
                //If there is no X then it loads the Y
                targetPosition.x = penPosition.x;
                targetPosition.y = atof(indexY + 1);

            }else{
                //Gets the Y value first to prevent the X value from loading Y
                targetPosition.y = atof(indexY + 1);
                //Ends the array at the Y index
                indexY = '\0';
                //Gets the X value
                targetPosition.x = atof(indexX + 1);
            }

            //Gets the integer value
            switch (atoi(buffer)){
                //G0 and G1 do the same in this program
                 case 1:
                    //Moves the pen
                    drawLine(targetPosition.x, targetPosition.y);
                    //Assumes the pen has moved to the newposition
                    penPosition.x = targetPosition.x;
                    penPosition.y = targetPosition.y;
                     
                 break;

                 case 2:
                    
                    movePen(targetPosition.x, targetPosition.y);
                 
                 break;

                 case 3:
                    returnHome();

                 break;
            }

            case 'M':
              //Loads the number after M into buffer
              buffer[0] = inputLine[index++];
              //Puts \0 to end the command
              buffer[1] = '\0';

              switch(atoi(buffer)){
                case 1:
                    penDown();
                break;

                case 2:
                    penUp();
                break;

                case 3:
                    jogPenUp();
                break;

                case 4:
                    jogPenDown();
                break;
              }
       }
   }
}

void movePen(float xa, float ya){

    if(logging){
        //Print the movement location for the pen
        Serial.print("xa, ya: ");
        Serial.print(xa);
        Serial.print(", ");
        Serial.println(ya);
    }

    //Convert corrdinates to steps
    xa = (int)(xa * stepsPerMillimeterX);
    ya = (int)(ya * stepsPerMillimeterY);
    float x0 = 0;
    float y0 = 0;

    //Find the change in position
    long dx = abs(xa);
    long dy = abs(ya);

    //Get the direction of the step
    int stepX = stepInc;
    int stepY = stepInc;

    if(x0 > xa){
        stepX = -stepInc;
    }
    if(y0 > ya){
        stepY = -stepInc;
    }

    long i;
    long over = 0;

    //Move X axis first
    if(dx > dy){
        for(i = 0; i < dx; i++){
            stepperX.step(stepX);

            //Allows the x and y to finish at the same time
            over += dy;
            if(over >= dx){
                over -= dx;
                stepperY.step(stepY);
            }
            delay(stepDelay);
        }
    }else{
        //Else the Y axis moves first
        for(i = 0; i < dy; i++){
            stepperY.step(stepY);

            //Allows the x and y to finish at the same time
            over += dx;
            if(over >= dy){
                over -= dy;
                stepperX.step(stepX);
            }
            delay(stepDelay);
        }
    }

    if(logging){
        Serial.print("Moving from (");
        Serial.print(x0);
        Serial.print(", ");
        Serial.print(y0);
        Serial.println(")");
    }

    if(logging){
        //Prints the change in the position
        Serial.print("Moving to (");
        Serial.print(dx);
        Serial.print(", ");
        Serial.print(dy);
        Serial.println(")");
    }
    
    xPos = 0;
    yPos = 0;
}

void returnHome(){
    drawLine(0, 0);
}

void drawLine(float xa, float ya){

    //Keep the pen position within max limits
    if(xa >= xMax) xa = xMax;
    if(xa <= xMin) xa = xMin;
    if(ya >= yMax) ya = yMax;
    if(ya <= yMin) ya = yMin;

    if(logging){
        //Print the current position of the pen
        Serial.print("xPos, yPos: ");
        Serial.print(xPos);
        Serial.print(", ");
        Serial.println(yPos);
    }

    if(logging){
        //Print the movement location for the pen
        Serial.print("xa, ya: ");
        Serial.print(xa);
        Serial.print(", ");
        Serial.println(ya);
    }

    //Convert corrdinates to steps
    xa = (int)(xa * stepsPerMillimeterX);
    ya = (int)(ya * stepsPerMillimeterY);
    float x0 = xPos;
    float y0 = yPos;

    //Find the change in position
    long dx = abs(xa - x0);
    long dy = abs(ya - y0);

    //Get the direction of the step
    int stepX = stepInc;
    int stepY = stepInc;

    if(x0 > xa){
        stepX = -stepInc;
    }
    if(y0 > ya){
        stepY = -stepInc;
    }

    long i;
    long over = 0;

    //Move X axis first
    if(dx > dy){
        for(i = 0; i < dx; i++){
            stepperX.step(stepX);

            //Allows the x and y to finish at the same time
            over += dy;
            if(over >= dx){
                over -= dx;
                stepperY.step(stepY);
            }
            delay(stepDelay);
        }
    }else{
        //Else the Y axis moves first
        for(i = 0; i < dy; i++){
            stepperY.step(stepY);

            //Allows the x and y to finish at the same time
            over += dx;
            if(over >= dy){
                over -= dy;
                stepperX.step(stepX);
            }
            delay(stepDelay);
        }
    }

    if(logging){
        Serial.print("Moving from (");
        Serial.print(x0);
        Serial.print(", ");
        Serial.print(y0);
        Serial.println(")");
    }

    if(logging){
        //Prints the change in the position
        Serial.print("Moving to (");
        Serial.print(dx);
        Serial.print(", ");
        Serial.print(dy);
        Serial.println(")");
    }

    delay(lineDelay);
    xPos = xa;
    yPos = ya;
}

void penUp(){

    //Puts the pen in it up position
    penServo.write(zAxisUP);
    delay(lineDelay);
    zPos = zMax;

    if(logging){
        Serial.println("Pen: Up");
    }
}

void penDown(){

    //Puts the pen down it up position
    penServo.write(zAxisDOWN);
    delay(lineDelay);
    zPos = zMin;

    if(logging){
        Serial.println("Pen: Down");
    }
}

void jogPenUp(){
    zAxisDOWN++;
    penServo.write(zAxisDOWN);
    delay(lineDelay);
}

void jogPenDown(){
    zAxisDOWN--;
    penServo.write(zAxisDOWN);
    delay(lineDelay);
}
