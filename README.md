# Mosquito
Implementation of a homekit motion detector running on esp32

# Wiring Plan
   +-------------------------------+
   |                               |
+--+------------------------+      |
|  V5                       |      |
|                           |      |
|     ESP32 NodeMCU         |      |
|                           |      |
|         21           GND  |      |
+----------+------------+---+      |
           |            |          |
           |  +---------+          |
           |  |                    |
           +------+   +------------+
              |   |   |
          +---+---+---+-----+
          |   hc--sr501     |
          |                 |
          |3     - +   - +  |
          |2    +---+ +---+ |
          |1    | 1 | | 2 | |
          +---+-+---+-+-+-+-+
              |         |
              +-+     +-+
                +-----+

## Motion Detector
Set the jumper on the left to 2&3, so the signal keeps high while movement continues.
Turn the left potentiometer clockwise to increase the sensing range up to 7 meters.
Turn the right potentiometer clockwise to increase the on time after no movement up to 5 minutes.
