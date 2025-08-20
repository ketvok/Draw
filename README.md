# Draw
Draw is a Microsoft Paint-like drawing application built using C++ and MFC.
> **This application is a work in progress.** Features may change, and bugs may exist.

<img width="970" height="889" alt="MyDrawing" src="https://github.com/user-attachments/assets/904aed66-bdd2-4fdb-87e6-a3544dd901bb" />

## Features
- Draw freely on a resizable canvas

- Choose different colors and sizes for your drawing tools

- Five tools to choose from:
  - Pen - basic freehand writing and drawing
  - Eraser - drawing with background color
  - Brush - draw smooth lines
  - Fill - fill an area with color
  - Color Picker - pick a color from any part of the canvas to be used with other tools

- Canvas resizing via drag handle at the bottom-right corner

- Print support with scaling to printable area while maintaining aspect ratio

- Save and open images in different file formats (PNG, JPEG, BMP, GIF, TIFF)

## Build Instructions

1. Open the solution in **Visual Studio**
2. Select the **Release** configuration
3. Press **Ctrl + F5** to build and run the application
4. After building, you can find `Draw.exe` in the `Release/` folder.  
   This executable is self-contained — you can copy it to another Windows PC and run it directly without installation.