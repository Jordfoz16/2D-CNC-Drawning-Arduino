# 2D CNC Drawning Arduino

## Creating G-Code:
### Using Inkscape
<b>Step 1 -</b> Download and install Ink Scape from https://inkscape.org/

<b>Step 2 -</b> Change document size to maximum drawing size of the CNC Drawning (e.g. 40mm x 40mm).

<b>Step 3 -</b> Draw your design or import a SVG file.

<b>Step 4 -</b> Convert into paths. This is done in the path menu in the top menu bar.

<b>Step 5 -</b> Once done, Save as a 'Desktop Cutting Plotter' file extenstion '.dxf'. It will then ask for a base unit which should be 'mm'

### Using DXF2GCODE:
<b>Step 1 -</b> Download and install DXF2GCODE from https://sourceforge.net/projects/dxf2gcode/

<b>Step 2 -</b> Before opening replace the config file with the one provided in the DXF2GCODE Config folder. The location of the config folder is C:\Users\"USERNAME"\.config. Delete the dxf2gcode folder and replace with the dxf2gcode folder that I have provided

<b>Step 3 -</b> Open DXF2GCODE and open the .dxf file that you created.

<b>Step 4 -</b> Click the export button in the menu bar. Then click optimize and export shapes.

<b>Step 5 -</b> Open the exported G-code with a text editor and remove the top 4 lines of comments. This will break the arduino program because its longer than the maximum length aloud.

<b>Step 6 -</b> The G-Code should now be ready to use.

## Resources:
### Software:
Arduino: https://www.arduino.cc/en/main/software

Processing: https://processing.org/download/

Ink Scape: https://inkscape.org/

DXF2GCODE: https://sourceforge.net/projects/dxf2gcode/


### Inspired by: 
Using code from this project but I have modified the code to suit this project https://www.instructables.com/id/DIY-MINI-CNC-DRAWING-MACHINE/

