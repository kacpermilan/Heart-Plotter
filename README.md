# Heart Plotter
Heart Plotter is a Windows Application group project, realized as a part of the Dedicated Medical Diagnostic Algorithms university course.
Project was written in C++ and assumed implementing a full analysis of ECG signal, different plots such as detected R peaks, 
displaying calculated parameters in a table and ability to save the results to a .pdf file.
Due to the time constraints (end of academic term) and organizational issues, the project was not implemented in 100% and only part of the aplication is working as intended.

![Picture with main page of the application, consisting of the plot with ECG signal, table with calculated diagnostc data and buttons responsible for saving the plot to .pdf file.](Heart%20Plotter/doc/app.png)

## Application modules
Work was divided by assigning the people to the different modules of the application:
- ECG BASELINE - Filtration of isoelectric line and network/muscle interference. Filter and its parameters are custom.
- R PEAKS - Detection of QRS complexes using Pan-Tompkins and complex Hilbert transformation methods
- HRV1 - Time and frequency analysis of heart rate variability
- HRV2 - Geometric analysis of heart rate variability
- HRV DFA - Trendless tachogram fluctuation analysis
- WAVES - Detection of characteristic points of the ECG signal
- HEART CLASS - Classification of QRS complexes
- ST SEGMENT - Classification of ST segments
- T WAVE ALT - Determination of the T wave alternan
- Application Input/Output
- GUI & Visualization

And three roles focused on planning and organization:
 - First Software Architect (repo's author), responsible for creating the Class Diagram, trobleshotting the issues in the team and merging the working parts of the application.
 - Second Software Architect, responsible for implementing the common service, used as a access Facade to the model parts of the application, and preparing the interfaces.
 - Team Manager

## Class Diagram 
![Picture with the class diagram of the application.](Heart%20Plotter/doc/class_diagram.png)

## Used libraries and technologies
- Qt
- Armandillo
- GSL
- wfdb
