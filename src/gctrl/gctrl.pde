import java.awt.event.KeyEvent;
import javax.swing.JOptionPane;
import processing.serial.*;

Serial port = null;

// select and modify the appropriate line for your operating system
// leave as null to use interactive port (press 'p' in the program)
String portname = null;
//String portname = Serial.list()[0]; // Mac OS X
//String portname = "/dev/ttyUSB0"; // Linux
//String portname = "COM6"; // Windows

boolean streaming = false;
float speed = 1;
String[] gcode;
int i = 0;

void openSerialPort()
{
  if (portname == null) return;
  if (port != null) port.stop();
  
  port = new Serial(this, portname, 9600);
  
  port.bufferUntil('\n');
}

void selectSerialPort()
{
  String result = (String) JOptionPane.showInputDialog(frame,
    "Select the serial port that corresponds to your Arduino board.",
    "Select serial port",
    JOptionPane.PLAIN_MESSAGE,
    null,
    Serial.list(),
    0);
    
  if (result != null) {
    portname = result;
    openSerialPort();
  }
}

void setup()
{
  size(500, 250);
  openSerialPort();
}

void draw()
{
  background(0);  
  fill(255);
  int y = 24, dy = 12;
  text("INSTRUCTIONS", 12, y); y += dy;
  text("p: select serial port", 12, y); y += dy;
  text("1: set speed to 0.1mm per jog", 12, y); y += dy;
  text("2: set speed to 1.0mm per jog", 12, y); y += dy;
  text("3: set speed to 10.0mm per jog", 12, y); y += dy;
  text("arrow keys: set home in x-y plane", 12, y); y += dy;
  text("< & >: jog in z axis", 12, y); y += dy;
  text("h: go home", 12, y); y += dy;
  text("g: stream a g-code file", 12, y); y += dy;
  text("x: stop streaming g-code (this is NOT immediate)", 12, y); y += dy;
  y = height - dy;
  text("current jog speed: " + speed + " mm per step", 12, y); y -= dy;
  text("current serial port: " + portname, 12, y); y -= dy;
}

void keyPressed()
{
  if (key == '1') speed = 0.1;
  if (key == '2') speed = 1.0;
  if (key == '3') speed = 10.0;
  
  if (!streaming) {
    if (keyCode == LEFT) port.write("G20 X-" + speed + " Y0.000 Z0.000\n");
    if (keyCode == RIGHT) port.write("G20 X" + speed + " Y0.000 Z0.000\n");
    if (keyCode == UP) port.write("G20 X0.000 Y" + speed + " Z0.000\n");
    if (keyCode == DOWN) port.write("G20 X0.000 Y-" + speed + " Z0.000\n");
    if (keyCode == ',') port.write("M3\n");
    if (keyCode == '.') port.write("M4\n");
    if (key == 'h') port.write("G30\n");
    if (key == '0') openSerialPort();
    if (key == 'p') selectSerialPort();
    if (key == '$') port.write("$$\n");
  }
  
  if (!streaming && key == 'g') {
    gcode = null; i = 0;
    File file = null; 
    println("Loading file...");
    selectInput("Select a file to process:", "fileSelected", file);
  }
  
  if (key == 'x') streaming = false;
}

void fileSelected(File selection) {
  if (selection == null) {
    println("Window was closed or the user hit cancel.");
  } else {
    println("User selected " + selection.getAbsolutePath());
    gcode = loadStrings(selection.getAbsolutePath());
    if (gcode == null) return;
    streaming = true;
    stream();
  }
}

void stream()
{
  if (!streaming) return;
  
  while (true) {
    if (i == gcode.length) {
      streaming = false;
      return;
    }
    
    if (gcode[i].trim().length() == 0) i++;
    else break;
  }
  
  println(gcode[i]);
  port.write(gcode[i] + '\n');
  i++;
}

void serialEvent(Serial p)
{
  String s = p.readStringUntil('\n');
  println(s.trim());
  
  if (s.trim().startsWith("Next Command")) stream();
  if (s.trim().startsWith("error")) stream(); // XXX: really?
}
