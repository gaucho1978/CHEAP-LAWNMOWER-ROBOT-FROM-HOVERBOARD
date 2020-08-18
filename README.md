You can control this lawnmower robot with a rc radiocontrol like a big rover, or let it automatically take care of your garden.
You could also replace the radiocontrol with bluetooth receiver conneted on the slave board serial line (reference about this on Florian repository)

The main feature of this lawnmower, with reference to other lawnmower robot products, is that it is able to do its work without a perimeter wire. Compared to other cheap products, it has bigger battery so it is able to take care of really big garden.

Every 24 hours it cuts the garden grass then it returns to the charging station. 

It uses proximity sensors to avoid obstacles and for safety against blade touching (they reveal the distance of the robot from the ground in case someone tries to lift it, in order to stop the blade).

It moves with reference to the start point (the place where the ground station is) knowing which path to follow (i'm planning to save inside the robot the garden morphology or the path) in order to avoid the need of perimeter wire. 
The project is in progress. 

On 12 may 2019 it can only move with rc commands cause iM waiting to receive the sensors and the chassis.


------------------------------
#### MECHANICAL CONSIDERATIONS

The mechanical project is located in the folder LawnMowerMechanicalProject.
These are some images (the base is aluminium and the cover in polycarbonate):
![otter](https://github.com/gaucho1978/CHEAP-LAWNMOWER-ROBOT-FROM-HOVERBOARD/blob/master/LawnmowerMechanicalProject/3D%20PROJECT%20-%20INVENTOR%202014/pictures/bottom1.png)
![otter](https://github.com/gaucho1978/CHEAP-LAWNMOWER-ROBOT-FROM-HOVERBOARD/blob/master/LawnmowerMechanicalProject/3D%20PROJECT%20-%20INVENTOR%202014/pictures/side.png)
![otter](https://github.com/gaucho1978/CHEAP-LAWNMOWER-ROBOT-FROM-HOVERBOARD/blob/master/LawnmowerMechanicalProject/3D%20PROJECT%20-%20INVENTOR%202014/pictures/prospective.png)
![otter](https://github.com/gaucho1978/CHEAP-LAWNMOWER-ROBOT-FROM-HOVERBOARD/blob/master/LawnmowerMechanicalProject/3D%20PROJECT%20-%20INVENTOR%202014/pictures/prospective2.png)
![otter](https://github.com/gaucho1978/CHEAP-LAWNMOWER-ROBOT-FROM-HOVERBOARD/blob/master/LawnmowerMechanicalProject/3D%20PROJECT%20-%20INVENTOR%202014/pictures/front.png)


- You will find files for manufacturing alluminium plate and polycarbonate cover in the folder:
   LawnMowerMEchanicalProject/FILES FOR MANUFACTURING/PDF FORMAT
- You will fing files for manufacturing plastic supports with 3d printer in the folder:
   LawnMowerMEchanicalProject/FILES FOR MANUFACTURING/3DPrinted supports
- The blade support shall be 3d printed. look ad detailed mechanical drawing to see how to assemble 3dprinted parts.


------------------------------
#### BILL OF MATERIAL - TOTAL COST around 160 EURO
50 EURO - USED HOVERBOARD - https://www.mediaworld.it/product/p-742077/i-bike-kidplay-streetboard-one-hoverboard-6-5?ds_rl=1250284&ds_rl=1250284&gclid=CjwKCAjwiN_mBRBBEiwA9N-e_qMfsYSRb5w4XxsWQmA6IEDfbXj_eAiy6rWeXzdMz_envhganzewVxoCpBMQAvD_BwE&gclsrc=aw.ds

20 EURO - TURNIGY 9X RADIOCONTROL OR ANY OTHER TYPE OF RC WITH PWM OUTPUT WITH AT LEAST 2 CHANNELS (ONE FOR SPEED AND ONE FOR STEERING)

6 EURO - STLINK V2 PROGRAMMER (TO FLASH HOVERBOARD FIRMWARE) - https://www.ebay.it/itm/352654456738?ViewItem=&item=352654456738

5 EURO - FIVE PROXIMITY SENSORS HC-SR04 - https://www.ebay.it/itm/312495306039

2 EURO - Arduino nano(clone): https://www.ebay.it/itm/322913230315

1 EURO - DCDC(FOR ARDUINO AND/OR BLADE MOTOR): https://www.ebay.it/itm/122201239217

30 EURO - MOTORS FOR BLADES (I REUSED SOME 24V FAN, powering them with modified notebook AC/DC)

30 EURO - 3D PRINTED SUPPORTS FOR BEARING, (I PRINTED THEM FOR FREE WITH MY PRINTER)

1 EURO - FOUR BLADES FOR CUTTER (I FOUND THEM IN A LOCAL HARDWARE STORE (OBI))

10 EURO - POLYCARBONATE OR PLEXIGLASS COVER (I FOUND PIECE OF POLYCARBONATE FOR FREE )

10 EURO - ALUMINIUM OR IRON BASE (I FOUND ALUMINIUM PLATE FOR FREE)

5 EURO - SCREWS AND BOLTS

5 EURO - TWO ROTATING WHEELS 66mm HEIGHT (I FOUND THEM IN A LOCAL CHINESE SHOP)

2 EURO - PANIC BUTTON - https://www.ebay.it/itm/Rosso-fungo-Cap-1-NO-1-NC-ferma-emergenza-Pulsante-interruttore-DPST-660V-10A/123270871363?ssPageName=STRK%3AMEBIDX%3AIT&_trksid=p2057872.m2749.l2649



------------------------------
#### HOVERBOARD HARDWARE
The hoverboard with 2 boards uses processor GD32F130C8 (instead of STM32F103 used on Niklas hoverboard) 
![otter](https://github.com/gaucho1978/CHEAP-LAWNMOWER-ROBOT-FROM-HOVERBOARD/blob/master/HoverboardPCBFirmware/images/Hardware_Overview_small.png )


The hoverboard hardware has two main boards, which are different equipped. They are connected via USART. Additionally there are some LED PCB connected at X1 and X2 which signalize the battery state and the error state. There is an programming connector for ST-Link/V2 and they break out GND, USART/I2C, 5V on a second pinhead.

The reverse-engineered schematics of the mainboards can be found in the folder HoverboardSchematics

In order to use  GPIO represented as "not used" on the hoverboard schematic, you need to solder some zero ohm resistors (or jumpers) (see schematic).

I soldered resistive dividers directly on the pcb as shown on the following image (while on the dwg lawnmower schematic, the resistors are represented as external components). this way the connection to the rc receiver becomes simple as few wires:
![otter](https://github.com/gaucho1978/CHEAP-LAWNMOWER-ROBOT-FROM-HOVERBOARD/blob/master/HoverboardPCBFirmware/images/weldings.png)

I REUSED THE HOVERBOARD HARDWARE AS IT IS. I DIDN'T MODIFIED THE FRAME, I JUST ROTATED IT 180 DEGREES.


------------------------------
#### HOVERBOARD FIRMWARE
I started my code from Florian Repository ( https://github.com/flo199213/Hoverboard-Firmware-Hack-Gen2 )

Florian rewrote the firmware for hoverboards composed by 2 boards, starting from Niklas repository ( https://github.com/NiklasFauth/hoverboard-firmware-hack/ )

Niklas rewrote the firmware for hoverboards composed by a big board and sensor boards.

First thing that I've done was to add PWM input to control the robot with a RC receiver, in order to let me do all the functional checks, before to implement automatic navigation algorithm.
I used turnigy rc receiver directly connected to the board, on the unused gpio.

Second thing that i've done was to add PWM output in order to control a ESC connected to a motor. The motor is connected to a blade. 

Every time the robot moves, the blade starts to rotate at constant speed.

The following image shows how the 3 phases changes during rotation. 

Note: A complete rotation of the phases is not a complete rotation of the wheel since there are many inductors inside the motor. 
![otter](https://github.com/gaucho1978/CHEAP-LAWNMOWER-ROBOT-FROM-HOVERBOARD/blob/master/HoverboardPCBFirmware/images/Raumzeigerdiagramm.png )

FIRMWARE STRUCTURE:

-THE FILE "DEFINES" AND "CONFIG" CONTAINS MAIN PARAMETERS 

-THE FILE "IT" CONTAINS TIMING FUNCTIONS AND INTERRUPTS (PERIODIC CHECKS,  SERIAL LINE INTERRUPTS, SYSTEM TICKS)

-THE FILE "BDLC" CONTROLS THE HOVERBOARD MOTORS

-THE FILE "COMMSSTEERINGPWM" RECEIVES THE RC PWM SIGNALS TO MOVE THE HOVERBOARD IN SPEED AND DIRECTION.

-THE FILE "COMMSMASTERSLAVE" SENDS AND RECEIVE DATA BETWEEN MASTER AND SLAVE BOARD

-THE FILE "LED" CONTROLS THE GPIO OUTPUT GENERALLY CONNETED TO LEDS TO SHOW BATTERY STATUS ETC,ETC.

-THE FILE "COMMSACTUATOR"GENERATES THE PWM USED TO CONTROL THE MOTOR CONNECTED TO THE BLADE.

-THE FILE "MAIN" IS THE MAIN LOOP OF THE HOVERBOARD FIRMWARE THAT CALLS ALL THE ROUTINES IN SEQUENCE.



------------------------------
#### FLASHING
The firmware is built with Keil (free up to 32KByte). To build the firmware, open the Keil project file which is includes in repository. On the board, close to ARM processor, there is a debugging header with GND, 3V3, SWDIO and SWCLK. Connect GND, SWDIO and SWCLK to your SWD programmer, like the ST-Link V2.

- If you never flashed your mainboard before, the controller is locked. To unlock the flash, use STM32 ST-LINK Utility or openOCD.
- To flash the processor, use the STM32 ST-LINK Utility as well, ST-Flash utility or Keil by itself.
- Hold the powerbutton while flashing the firmware, as the controller releases the power latch and switches itself off during flashing

NOTE: I USE TO UNPLUG THE HOVERBOARD BATTERY AND TO PLUG THE STLINK PROGRAMMER (INCLUDED 3,3VOLT PIN), THEN TO FLASH THE FW USING KEIL. THIS WAY YOU DON'T NEED TO LEAVE PRESSED THE POWER BUTTON.

NOTE2: ONE BOARD IS MASTER THE OTHER IS SLAVE. BEFORE TO FLASH THE FW, SET THE FIRMWARE AS MASTER OR SLAVE IN THE CONFIG.H AND RECOMPILE THE FIRMWARE.

------------------------------
#### COMMUNITY
there is a Telegram group as a free discussion platform about Hoverboards MODS. You can find it here: https://t.me/joinchat/BHWO_RKu2LT5ZxEkvUB8uw


------------------------------
#### NEXT THINGS THAT I WILL DO: 
- add sensors and interlocks to algorithm,
- create a docking station for automatic recharge, 
- realize the mechanical structure, 
- implement safety for automatic stopping blade 
- implement automatic navigation algorithm.

