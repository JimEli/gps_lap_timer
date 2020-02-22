# GPS Lap Timer
### Use a GPS connected to a com port to time consecutive laps

Program is a racing lap timer that receives data from a GPS connected to a comm port. It extracts time, location and fix information from the transmitted GPS NMEA RMC sentence (no other GPS sentences utilized). Once the program is initiated, it waits in a loop until the GPS acquires a valid fix. Then the user simply establishes a perpendicular (based upon GPS heading/track) start/finish line by pressing the ENTER key. Subsequently passing the start/finish line will compute and display a time for the completed lap. A basic data file of several example laps is available for testing purposes (FILE_INPUT). The final version of this program intended to run on an Arduino based uC device.

This is a work in progress.
