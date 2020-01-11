# GPS Lap Timer
### Use a GPS connected to a com port to time consecutive laps

This racing lap timer receives data from a GPS connected to a comm port. It extracts time, location and fix information from the NMEA RMC sentence. When initiated the program waits in a loop until the GPS acquires a valid fix. Then simply establish a start/finish line by pressing the ENTER key. Subsequent passing of the start/finish line will display a time for each lap. A basic data file of several laps is available for testing purposes (FILE_INPUT). The final version of this program intended to run on an Arduino based uC device.

This is a work in progress.
