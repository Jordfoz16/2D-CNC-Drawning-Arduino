# 2D CNC Drawning Arduino

## Creating G-Code:
### Using Inkscape
#### Step 1
Download and install Ink Scape from https://inkscape.org/
#### Step 2
Change document size to maximum drawing size of the CNC Drawning (e.g. 40mm x 40mm).
#### Step 3
Draw your design or import a SVG file.
#### Step 4
Convert into paths. This is done in the path menu in the top menu bar.
#### Step 5
Once done, Save as a 'Desktop Cutting Plotter' file extenstion '.dxf'. It will then ask for a base unit which should be 'mm'

### Using DXF2GCODE:
#### Step 1
Download and install DXF2GCODE from https://sourceforge.net/projects/dxf2gcode/
#### Step 2
Before opening replace the config file with the one provided in the DXF2GCODE Config folder. The location of the config folder is C:\Users\"USERNAME"\.config. Delete the dxf2gcode folder and replace with the dxf2gcode folder that I have provided
#### Step 3
Open DXF2GCODE and open the .dxf file that you created.
#### Step 4
Click the export button in the menu bar. Then click optimize and export shapes.
#### Step 5
Open the exported G-code with a text editor and remove the top 4 lines of comments. This will break the arduino program because its longer than the maximum length aloud.
#### Step 6
The G-Code should now be ready to use.

## Resources:
### Software:
Arduino: https://www.arduino.cc/en/main/software

Processing: https://processing.org/download/

Ink Scape: https://inkscape.org/

DXF2GCODE: https://sourceforge.net/projects/dxf2gcode/


### Inspired by: 
Using code from this project but I have modified the code to suit this project https://www.instructables.com/id/DIY-MINI-CNC-DRAWING-MACHINE/

