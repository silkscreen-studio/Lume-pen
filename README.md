

# LUME Pen
The most ordinary pen, except that it has electronics stuffed inside, on a flexible PCB. It can even tell the time and double as a pomodoro timer, thanks to a tiny custom 7 segment display, with a capacitive slider to control it easily 

I mainly made this project to challenge myself on integrating electronics in such a tight space (6mm hollow tube) and to design my first Flex PCB. Also, I always thought that integrating a mini segmented display and a capacitive slider into a pen body would look really cool, and it looks exactly as cool as I imagined it. 

With this clock and pomodoro timer included in the tool everyone uses when working or studying, no need for distracting apps and constant checks on a phone!

<br><br>
<img width="983" height="1499" alt="LUME Pen ZINE V1 1" src="https://github.com/user-attachments/assets/b066e538-5ad8-4684-90f4-f7a00f366bf1" />

The Lume Pen body is split in 3 parts, in MJF Nylon and SLA clear resin, to make assembly possible while giving texture to the design.
The flex PCB, made of multiple flaps bent over each other to achieve the 5mm width runs all along the pen body, while keeping space for an half-long BIC® cartridge as well as a rod shaped 4.5mm wide li-ion battery. The segments on the display are shaped from the 30x 0402 LEDs with 3 layers: a small 3D printed spacer (FDM, .2mm nozzle), a thin diffuser sheet and a custom designed light mask to get sharp segments, made from a  precisely laser cut 100 microns thin steel plate, also known as a stencil...

# Product Key features:
 - The Lume pen can tell the time, be used as a Pomodoro timer (configurable timer with multiple sessions possible, alterning 5-60 min of work with a 5 minute break), and... be used as a normal pen too...
 - Sleek design, MJF Nylon & Clear Resin body to see the electronics
 - Capacitive slider to enable gesture control
 - Stylish Segmented Display Font [^1], made from 30 orange LEDs
 - No ON/OFF switch: capacitive CMD button to shut it down, can last a few months on a single charge when turned off
 - Magnetic charger (with pogopins), to charge it easily (from a USB-A port)

# Technical Key features:
## Lume pen:
 - Built around the power efficient `ATtiny1616` MCU (16kB FLASH, 2kB SRAM)
 - 2x I²C ICs for the RTC (`RX8111`) and capacitive touch management (`AW93105`)
 - Onboard `LP5907` 3.0V LDO with low quiscent current (12µA)
 - 3.7V Li-ion rod shaped battery, 25mAh, and its `BQ25170` charging IC
 - interfaces with the charger thanks to 4 gold plated contacts (pogopins on the charger): VIN, GND, UPDI and a mating detect pin.
 - Charlieplexed 30x 0402 orange LEDs 4 digit 7-segment display
 - Flexible PCB, ENIG, with stiffener: 125x40mm² panel with both the LUME and charger PCB
## Magnetic charger:
 - Smart charger with the $0.25 `CH32V003` MCU (16kB FLASH, 2kB SRAM)
 - Load switch (`TPS2553DRVR-1`) with latch off overcurrent and short circuit protection, MCU FAULT assertion, MCU ENABLE,
 - mating detection to enable/disable charging
 - 5 white LEDs to display charging animation, and 1 RED FAULT LED




# Schematic:
## Lume Pen schematic
<img width="2432" height="1151" alt="LUME_pen_schematic_V1 0" src="SCHEMATICs/SCH_LUME_SCH_1-P1_2026-06-28.svg" />


## Charger shematic

<img width="2444" height="1484" alt="LUME_pen_charger_schematic_V1 0" src="SCHEMATICs/SCH_Charger_SCH_1-P1_2026-06-28.svg" />


# PCB Layout:
## LUME flexible Panel:
I decided to panelize it with 2 Lume variant, where I have a line pattern silkscreen on one variant, and a full white pour on the other one, on all the visible parts. Not knowing what the yellow coverlay would look like in real life, I could now see which one looks better. Ordering this dual panel is only $6 more expensive ($40.42 for 5 panel, so 10 Lume PCBs) than the single panel ($34.14 for 5 panel, so 5 Lume PCBs). Why not directly choose a white (or black) coverlay? As it adds a $8 fee, it would have been $42.14 for the single panel (5 Lume PCBs).

For $40, I have 10 Lume pen PCBs (5 of each variant), with PI stiffener and ENIG finish.

<img width="2418" height="1239" alt="LUME PANEL 3D" src="PCB/LUME PANEL 3D.png" />
<img width="2824" height="932" alt="LUME PANEL Top 2" src="PCB/LUME PANEL Top 2.png" />
<img width="2824" height="952" alt="LUME PANEL Bottom 2" src="PCB/LUME PANEL Bottom 2.png" />


<img width="2824" height="952" alt="LUME LAYOUT PANEL TOP" src="PCB/LUME LAYOUT TOP.png" />
<img width="2824" height="952" alt="LUME LAYOUT PANEL  BOTTOM" src="PCB/LUME LAYOUT BOTTOM.png" />

## Debug friendly FR4 board

Before ordering the flexible PCB, I'll validate the schematic and test the firmware with a Debug-friendly board, on a cheap FR4 board. With its many test points, separated block design allowing easy cut traces and bodge wires, debugging selection switches and UART communication will make the debugging process a lot easier:

<img width="1967" height="1456" alt="Debug friendly FR4 3D" src="https://github.com/user-attachments/assets/e7fa400f-2be5-4859-83ed-bbff7da3f6f6" />

<img width="1800" height="1375" alt="Debug friendly FR4 3D back" src="https://github.com/user-attachments/assets/806b5b81-2c45-4fd1-982c-bc36c6f03cb4" />
<img width="1772" height="1482" alt="Debug friendly FR4 layout" src="https://github.com/user-attachments/assets/60dcf7fe-ba7e-4d32-8754-e6496f3b1ee7" />

I'm using my dual UPDI programmer and Serial communication debug tool I made for AngstromIO [^2], which explains the specific 'daugther board' footprint, but any UPDI programmer and USB to UART would work perfectly too.


# Firmware:
## Lume pen firmware (ATtiny1616): 
The ATtiny1616 is programmed with the Arduino IDE. I'm using 2 custom made libraries as I needed a lightweight library for the RX8111 (only implementing the clock feature) and a library for the poorly documented AW93105 IC. I used the help of Claude AI to code 2 really basic libraries, especially for the AW93105, and I'll then later built from that a usable library with all the features implemented once I've tried this basis on the Debug-friendly PCB. So I kept these first library as simple as possible to make debugging easier, and this is why the sliding and other gesture aren't implemented yet, so the main MCU code is thus just a sketch, that won't behave properly out of the box (since it can't react to non implemented gesture). The Charlieplexed display and other logic should be working independently though.
The code (thus still a W.I.P) is available in /Firmware/LUME_PEN

## Charger firmware (CH32V003):
The CH32V003 is programmed with the Mounriver Studio IDE. The code (untested for now), that handles power management, Fault handling, LED animation (PWM), and mating detection is available in /Firmware/CHARGER/main.c


<br><br>
# Design Renders
<br><br>
<img width="2880" height="1259" alt="Render 1" src="https://github.com/user-attachments/assets/7e453c96-0787-4052-a269-8704c4139a6d" />
<img width="2880" height="1512" alt="Render 2" src="https://github.com/user-attachments/assets/34635aea-6ece-4486-aa97-3c482565c643" />
<img width="2880" height="1259" alt="Render 3" src="https://github.com/user-attachments/assets/28e8ce2e-f858-49df-b030-4878f5851ab3" />
<img width="2880" height="1259" alt="Render 4" src="https://github.com/user-attachments/assets/d4d0da5d-9906-4f21-bb0e-d4500e93aae7" />
<img width="2880" height="1259" alt="Render 10" src="https://github.com/user-attachments/assets/36f019f9-5e92-438f-9005-3a07b3b9be4f" />
<br><br>
## ... and under the shell:


<img width="2880" height="1259" alt="Render 5" src="https://github.com/user-attachments/assets/732c7524-5485-484f-b550-69218bcfbcdc" />
<img width="2880" height="1512" alt="Render 6" src="https://github.com/user-attachments/assets/9a6a7f48-b2b2-4820-99a2-326f520f1eab" />
<img width="2880" height="1259" alt="Render 7" src="https://github.com/user-attachments/assets/722ea8fb-fbdc-4f01-9662-332cd07d4496" />
<img width="2880" height="1259" alt="Render 8" src="https://github.com/user-attachments/assets/cf3e2754-47ae-496f-b252-df0eab0365c9" />
<img width="2880" height="1259" alt="Render 9" src="https://github.com/user-attachments/assets/5aabd0e1-7ce6-4dcb-8704-b8f1e9440766" />

## finally, the magnetic charger:

<img width="2880" height="1472" alt="Rd2" src="https://github.com/user-attachments/assets/71a73cf6-86f8-4200-bcc9-42707662e00f" />

<img width="2880" height="1472" alt="Rd3" src="https://github.com/user-attachments/assets/5c14674c-89a4-4c08-b59d-c76b6f70fee0" />

# BOM:

| Product | Qty | Part | Value | LCSC Part number | Price/unit | minimum Price [MOQ] | Extended description | Link | Price for 2 pens 1 debug boards & 2 chargers combined |
| --- | --- | --- | --- | --- | --- | --- | --- | ---  | --- |
| LUME PEN | 1 | ATTINY1616-MNR |  | C507118 | $1.09 | $1.09 | Main MCU | https://www.lcsc.com/product-detail/C507118.html | $3.29 |
| LUME PEN | 1 | TI LP5907SNX-3.0/NOPB | 3.0V | C182423 | $0.1967 | $0.98 [5] | LDO | https://www.lcsc.com/product-detail/C182423.html | $0.98 |
| LUME PEN | 1 | BQ25170DSGR |  | C5208201 | $0.59 | $0.59 | Li ion battery charger IC | https://www.lcsc.com/product-detail/C5208201.html | $1.77 |
| LUME PEN | 1 | EPSON RX8111CE-B |  | C2929261 | $1.1739 | $1.1739 | RTC | https://www.lcsc.com/product-detail/C2929261.html | $3.52 |
| LUME PEN | 1 | AW93105DNR |  | C5241002 | $0.5438 | $0.5438 | Capacitive touch IC (5 channels) | https://www.lcsc.com/product-detail/C5241002.html | $1.63 |
| LUME PEN | 1 | LESD5D5.0CT1G |  | C5199850 | $0.0166 | $0.83 [50] | TVS diode | https://www.lcsc.com/product-detail/C5199850.html | $0.83 |
| LUME PEN | 30 | XL-1005UOC | Vf: 2.0V | C965791 | $0.0142 | $0.71 [50] | 0402 Orange LED (display) | https://www.lcsc.com/product-detail/C965791.html | $1.42 |
| LUME PEN | 31 | XL-1005SURC | Vf: 2.0V | C25503345 | $0.0083 | $0.42 [50] | 0402 Red LED (display (alternate color) and /PG charging) | https://www.lcsc.com/product-detail/C25503345.html | $0.42 |
| LUME PEN | 5 | CL05A104KA5NNNC | 100nF | C100072 | $0.0063 | $0.63 [100] | 0402 MLCC | https://www.lcsc.com/product-detail/C100072.html | $0.63 |
| LUME PEN | 3 | CL05A105KO5NNNC | 1µF | C29266 | $0.0102 | $1.02 [100] | 0402 MLCC | https://www.lcsc.com/product-detail/C29266.html | $1.02 |
| LUME PEN | 2 | GRM155R61E475ME15D | 4.7µF | C2858031 | $0.15 | $1.51 [10] | 0402 MLCC | https://www.lcsc.com/product-detail/C2858031.html | $1.51 |
| LUME PEN | 1 | XL-1005UBC | Vf: 3.0V | C22355736 | $0.008 | $0.40 [50] | 0402 Blue LED (STAT Charging) | https://www.lcsc.com/product-detail/C22355736.html | $0.40 |
| LUME PEN | 2 | CRCW04024K70FKED | 4.7k | C482193 | $0.0037 | $0.37 [100] | 0402 Resistor | https://www.lcsc.com/product-detail/C482193.html | $0.37 |
| LUME PEN | 3 | CRCW040220K0FKED | 20k | C482096 | $0.0043 | $0.43 [100] | 0402 Resistor | https://www.lcsc.com/product-detail/C482096.html | $0.43 |
| LUME PEN | 1 | CRCW040224K0FKED | 24k | C295000 | $0.0042 | $0.42 [100] | 0402 Resistor | https://www.lcsc.com/product-detail/C295000.html | $0.42 |
| LUME PEN | 1 | CRCW040227K0FKEDC | 27k | C1730799 | $0.0052 | $0.52 [100] | 0402 Resistor | https://www.lcsc.com/product-detail/C1730799.html | $0.52 |
| LUME PEN | 3 | CRCW040210K0JNED | 10k | C191984 | $0.0035 | $0.35 [100] | 0402 Resistor | https://www.lcsc.com/product-detail/C191984.html | $0.35 |
| LUME PEN | 1 | CRCW0402100KFKEDC | 100k | C844712 | $0.0063 | $0.63 [100] | 0402 Resistor | https://www.lcsc.com/product-detail/C844712.html | $0.63 |
| LUME PEN | 1 | CRCW0402200KFKED | 200k | C482093 | $0.0046 | $0.46 [100] | 0402 Resistor | https://www.lcsc.com/product-detail/C482093.html | $0.46 |
| LUME PEN | 6 | CRCW0402680RFKED | 680 | C482224 | $0.0047 | $0.47 [100] | 0402 Resistor | https://www.lcsc.com/product-detail/C482224.html | $0.47 |
| LUME PEN | 5 | CRCW04020000Z0ED | 0 | C190119 | $0.0043 | $0.43 [100] | 0402 shunt Resistor (capacitive tuning; other values needed for tuning) | https://www.lcsc.com/product-detail/C190119.html | $0.43 |
|  |  |  |  |  |  |  |  |  |  |
| CHARGER | 5 | XL-1005UWC | Vf: 2.8V | C20613596 | $0.0112 | $0.56 [50] | 0402 White LED | https://www.lcsc.com/product-detail/C20613596.html | $0.56 |
| CHARGER | 1 | XL-1005SURC | Vf: 2.0V | C25503345 | $0.0083 | $0.42 [50] | 0402 Red LED | https://www.lcsc.com/product-detail/C25503345.html | / |
| CHARGER | 2 | CRCW040220K0FKED | 20k | C482096 | $0.0043 | $0.43 [100] | 0402 Resistor | https://www.lcsc.com/product-detail/C482096.html | / |
| CHARGER | 1 | CRCW0402100KFKEDC | 100k | C844712 | $0.0063 | $0.63 [100] | 0402 Resistor | https://www.lcsc.com/product-detail/C844712.html | / |
| CHARGER | 1 | CRCW040210K0JNED | 10k | C191984 | $0.0035 | $0.35 [100] | 0402 Resistor | https://www.lcsc.com/product-detail/C191984.html | / |
| CHARGER | 3 | CL05A104KA5NNNC | 100nF | C100072 | $0.0063 | $0.63 [100] | 0402 MLCC | https://www.lcsc.com/product-detail/C100072.html | / |
| CHARGER | 6 | CRCW0402330RFKED | 330 | C71626 | $0.0042 | $0.42 [100] | 0402 Resistor | https://www.lcsc.com/product-detail/C71626.html | $0.42 |
| CHARGER | 1 | CL05A106MP8NUB8 | 10µF | C307415 | $0.15 | $0.73 [5] | 0402 MLCC | https://www.lcsc.com/product-detail/C307415.html | $0.73 |
| CHARGER | 1 | TPS2553DRVR-1 |  | C411878 | $0.39 | $1.94 [5] | Load switch IC | https://www.lcsc.com/product-detail/C411878.html | $1.94 |
| CHARGER | 1 | CH32V003F4U6 |  | C5299908 | $0.32 | $1.58 [5] | Main MCU | https://www.lcsc.com/product-detail/C5299908.html | $1.58 |
| CHARGER | 1 | LESD5D5.0CT1G |  | C5199850 | $0.0168 | $0.84 [50] | TVS diode | https://www.lcsc.com/product-detail/C5199850.html | / |
| CHARGER | 1 | RB520SM-40T2R |  | C253514 | $0.03 | $0.56 [20] | Schottky diode | https://www.lcsc.com/product-detail/C253514.html | $0.56 |
| CHARGER | 4 | BWCD-L4.5W2.0H2.3 |  | C2826546 | $0.07 | $0.36 [5] | Pogopin | https://www.lcsc.com/product-detail/C2826546.html | $0.72 |
| CHARGER | 1 | section of USB cable |  |  |  |  | can be any USB cable; that got the ends stripped |  | / |
|  |  |  |  |  |  |  |  |  |  |
| ADDITIONAL PARTS (DEBUG PCB) | 1 | LESD5D5.0CT1G |  | C5199850 | $0.0166 | $0.83 [50] | TVS diode | https://www.lcsc.com/product-detail/C5199850.html | / |
| ADDITIONAL PARTS (DEBUG PCB) | 1 | RB520SM-40T2R |  | C253514 | $0.03 | $0.56 [20] | Schottky diode | https://www.lcsc.com/product-detail/C253514.html | / |
| ADDITIONAL PARTS (DEBUG PCB) | 2 | MK-12C02-G015 |  | C2911519 | $0.014 | $0.67 [5] | Slide Switch SPDT | https://www.lcsc.com/product-detail/C2911519.html | $0.67 |
| ADDITIONAL PARTS (DEBUG PCB) | 2 | HC-PM254-8.5H-1x3PW-02A |  | C22373927 | $0.09 | $0.46 [5] | 90° 2.54mm 1x3 HEADERS | https://www.lcsc.com/product-detail/C22373927.html | $0.46 |
| ADDITIONAL PARTS (DEBUG PCB) | 5 | FR4 Debug PCB |  |  | $2.00 | $2.00 |  | https://jlcpcb.com | $2.00 |
| ADDITIONAL PARTS (CHARGER) |  | Epoxy |  |  |  |  | mixed with white acrilyc paint (light diffusing) |  | - |
|  |  |  |  |  |  |  |  |  |  |
| GENERAL | 5 | Flex PCB PANEL |  |  | $41.42 | $41.42 |  | https://jlcpcb.com | $41.42 |
| GENERAL | - | 3D printed parts |  |  | $4.87 | $4.87 | see 3D-CAD/3D-Printed-PARTS | https://jlc3dp.com | $9.74 |
| GENERAL | 1 | Stencil |  |  | $3.07 | $3.07 |  | https://jlcpcb.com | $3.07
| GENERAL | 2 | neodymium magnets 5x5x5mm (cube) 10pcs |  |  |  | $4.53 | ($2.54 + $1.99 shipping) | https://fr.aliexpress.com/item/1005011587371110.html | $4.53 |
|  |  |  |  |  |  |  |  |  |  |
|  |  |  |  |  |  |  |  |  |  |
| SHIPPING (first order FR4 debug only + stencil) |  | JLCPCB |  |  | $1.50 x 2 |  |  |  | $3.00 |
| SHIPPING (second order FPC + 3D prints) |  | JLCPCB & 3DP |  |  | $1.50 |  |  |  | $1.50 |
| SHIPPING (first order: components) |  | LCSC (GDSL) |  |  | $10.20 |  |  |  | $10.20 |
|  |  |  |  |  |  |  |  |  |  |
| TOTAL (first order debug only) |  |  |  |  |  |  |  |  | $47.53 |
| TOTAL (second order FPC + 3D prints) |  |  |  |  |  |  |  |  | $52.66 |
|  |  |  |  |  |  |  |  |  |  |
| PROJECT GRAND TOTAL |  |  |  |  |  |  |  |  | $104.72 |




[^1]: Inspired from the **Seamless** font, a design by Michiel de Boer (_Pozy_): https://www.youtube.com/watch?v=RTB5XhjbgZA&t=6s and https://www.michieldb.nl/other/segments/
[^2]: more info on my other repository: https://github.com/Dieu-de-l-elec/AngstromIO-devboard
