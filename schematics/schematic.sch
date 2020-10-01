EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector:Conn_01x18_Female J1
U 1 1 5F4935D2
P 3000 2650
F 0 "J1" V 3050 2950 50  0000 C CNN
F 1 "Left rail FirebeetleESP8266" V 3050 2350 50  0000 C CNN
F 2 "" H 3000 2650 50  0001 C CNN
F 3 "~" H 3000 2650 50  0001 C CNN
	1    3000 2650
	1    0    0    1   
$EndComp
Text Label 3650 3450 0    50   ~ 0
RST
Text Label 2500 1950 0    50   ~ 0
GPIO13
$Comp
L Switch:SW_Push SW1
U 1 1 5F4AB5F8
P 4850 3450
F 0 "SW1" H 4850 3735 50  0000 C CNN
F 1 "NO" H 4850 3644 50  0000 C CNN
F 2 "" H 4850 3650 50  0001 C CNN
F 3 "~" H 4850 3650 50  0001 C CNN
	1    4850 3450
	1    0    0    -1  
$EndComp
$Comp
L Device:CP C1
U 1 1 5F4A933A
P 4000 3450
F 0 "C1" V 4255 3450 50  0000 C CNN
F 1 "100uF" V 4164 3450 50  0000 C CNN
F 2 "" H 4038 3300 50  0001 C CNN
F 3 "~" H 4000 3450 50  0001 C CNN
	1    4000 3450
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3600 3450 3850 3450
$Comp
L Device:R R1
U 1 1 5F4ACA30
P 4350 3100
F 0 "R1" H 4280 3054 50  0000 R CNN
F 1 "5.6k" H 4280 3145 50  0000 R CNN
F 2 "" V 4280 3100 50  0001 C CNN
F 3 "~" H 4350 3100 50  0001 C CNN
	1    4350 3100
	-1   0    0    1   
$EndComp
NoConn ~ 2800 2750
Wire Wire Line
	4350 3450 4350 3250
Wire Wire Line
	4150 3450 4350 3450
Wire Wire Line
	4350 2950 4350 2750
Wire Wire Line
	2400 1950 2800 1950
Wire Wire Line
	2800 3350 2700 3350
Wire Wire Line
	4350 3450 4500 3450
Connection ~ 4350 3450
Wire Wire Line
	4500 3450 4500 3750
Connection ~ 4500 3450
Wire Wire Line
	4500 3450 4650 3450
Text GLabel 4400 3750 0    50   Input ~ 0
GPIO13
Wire Wire Line
	4500 3750 4400 3750
Wire Wire Line
	5050 3450 5150 3450
$Comp
L power:GNDREF #PWR0101
U 1 1 5F4E6A2F
P 5150 3450
F 0 "#PWR0101" H 5150 3200 50  0001 C CNN
F 1 "GNDREF" H 5200 3500 50  0000 C CNN
F 2 "" H 5150 3450 50  0001 C CNN
F 3 "" H 5150 3450 50  0001 C CNN
	1    5150 3450
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR0103
U 1 1 5F4E90EE
P 4350 2750
F 0 "#PWR0103" H 4350 2600 50  0001 C CNN
F 1 "+3V3" H 4365 2923 50  0000 C CNN
F 2 "" H 4350 2750 50  0001 C CNN
F 3 "" H 4350 2750 50  0001 C CNN
	1    4350 2750
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x18_Female J2
U 1 1 5F49485E
P 3400 2650
F 0 "J2" V 3450 2950 50  0000 C CNN
F 1 "Right rail FirebeetleESP8266" V 3450 2300 50  0000 C CNN
F 2 "" H 3400 2650 50  0001 C CNN
F 3 "~" H 3400 2650 50  0001 C CNN
	1    3400 2650
	-1   0    0    1   
$EndComp
NoConn ~ 2800 2650
NoConn ~ 2800 2550
NoConn ~ 2800 2450
NoConn ~ 2800 2350
NoConn ~ 2800 2250
NoConn ~ 2800 2150
NoConn ~ 2800 2050
NoConn ~ 2800 1850
NoConn ~ 2800 1750
NoConn ~ 2800 2950
NoConn ~ 2800 2850
NoConn ~ 2800 3050
NoConn ~ 2800 3250
NoConn ~ 3600 3350
NoConn ~ 3600 3250
NoConn ~ 3600 3150
NoConn ~ 3600 3050
NoConn ~ 3600 2950
NoConn ~ 3600 2850
NoConn ~ 3600 2750
NoConn ~ 3600 2650
NoConn ~ 3600 2550
NoConn ~ 3600 2450
NoConn ~ 3600 2350
NoConn ~ 3600 2250
NoConn ~ 3600 2150
NoConn ~ 3600 2050
NoConn ~ 3600 1950
NoConn ~ 3600 1850
NoConn ~ 3600 1750
Text GLabel 2400 1950 0    50   Input ~ 0
GPIO13
$Comp
L power:+3V3 #PWR0104
U 1 1 5F4EC730
P 2700 3350
F 0 "#PWR0104" H 2700 3200 50  0001 C CNN
F 1 "+3V3" H 2450 3350 50  0000 L CNN
F 2 "" H 2700 3350 50  0001 C CNN
F 3 "" H 2700 3350 50  0001 C CNN
	1    2700 3350
	1    0    0    -1  
$EndComp
NoConn ~ 2800 3450
NoConn ~ 2050 2950
Wire Wire Line
	2050 2650 2250 2650
$Comp
L power:+3.3V #PWR?
U 1 1 5F56A445
P 2250 2650
F 0 "#PWR?" H 2250 2500 50  0001 C CNN
F 1 "+3.3V" H 2265 2823 50  0000 C CNN
F 2 "" H 2250 2650 50  0001 C CNN
F 3 "" H 2250 2650 50  0001 C CNN
	1    2250 2650
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x05_Female J3
U 1 1 5F567389
P 1850 2850
F 0 "J3" H 1742 2425 50  0000 C CNN
F 1 "bme280" H 1742 2516 50  0000 C CNN
F 2 "" H 1850 2850 50  0001 C CNN
F 3 "~" H 1850 2850 50  0001 C CNN
	1    1850 2850
	-1   0    0    1   
$EndComp
Wire Wire Line
	2250 3150 2800 3150
$Comp
L power:GNDREF #PWR0102
U 1 1 5F4E7867
P 2250 3150
F 0 "#PWR0102" H 2250 2900 50  0001 C CNN
F 1 "GNDREF" H 2200 3000 50  0000 C CNN
F 2 "" H 2250 3150 50  0001 C CNN
F 3 "" H 2250 3150 50  0001 C CNN
	1    2250 3150
	1    0    0    -1  
$EndComp
Text Label 2600 3150 0    50   ~ 0
GND
Wire Wire Line
	2050 2750 2800 2750
Wire Wire Line
	2050 2850 2800 2850
Wire Wire Line
	2050 3050 2250 3050
Wire Wire Line
	2250 3050 2250 3150
Connection ~ 2250 3150
Text Label 2600 2750 0    50   ~ 0
SDA
Text Label 2600 2850 0    50   ~ 0
SCL
Text Label 2050 2750 0    50   ~ 0
SDA
Text Label 2050 2850 0    50   ~ 0
SCL
$EndSCHEMATC
