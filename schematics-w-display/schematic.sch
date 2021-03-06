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
L Connector:Conn_01x21_Female J1
U 1 1 604CADD6
P 5450 4100
F 0 "J1" H 5300 2900 50  0000 L CNN
F 1 "Conn_01x21_Female_ESP" H 4950 3000 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x21_P2.54mm_Vertical" H 5450 4100 50  0001 C CNN
F 3 "~" H 5450 4100 50  0001 C CNN
	1    5450 4100
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Female J2
U 1 1 604D4A59
P 4750 2400
F 0 "J2" V 4688 2012 50  0000 R CNN
F 1 "Conn_01x06_Female_BME280" V 4597 2012 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 4750 2400 50  0001 C CNN
F 3 "~" H 4750 2400 50  0001 C CNN
	1    4750 2400
	0    -1   -1   0   
$EndComp
Text Label 4400 3300 0    50   ~ 0
SDO
Text Label 4400 3400 0    50   ~ 0
CSB
Text Label 4400 3500 0    50   ~ 0
SDA
NoConn ~ 5250 5100
NoConn ~ 5250 5000
NoConn ~ 5250 4900
NoConn ~ 5250 4800
NoConn ~ 5250 4700
NoConn ~ 5250 4600
NoConn ~ 5250 4500
NoConn ~ 5250 4400
NoConn ~ 5250 4300
NoConn ~ 5250 4200
NoConn ~ 5250 4100
NoConn ~ 5250 4000
NoConn ~ 5250 3900
NoConn ~ 5250 3800
NoConn ~ 5250 3700
NoConn ~ 5250 3400
NoConn ~ 5250 3300
NoConn ~ 4400 3800
NoConn ~ 4400 5000
NoConn ~ 4400 4900
NoConn ~ 4400 4200
NoConn ~ 4400 4100
NoConn ~ 4400 4000
NoConn ~ 4400 3900
$Comp
L Connector:Conn_01x21_Female J3
U 1 1 604CC464
P 4200 4100
F 0 "J3" H 4092 2875 50  0000 C CNN
F 1 "Conn_01x21_Female_ESP" H 4092 2966 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x21_P2.54mm_Vertical" H 4200 4100 50  0001 C CNN
F 3 "~" H 4200 4100 50  0001 C CNN
	1    4200 4100
	-1   0    0    1   
$EndComp
Text Label 4400 3600 0    50   ~ 0
SCL
Wire Wire Line
	4400 3300 4750 3300
Wire Wire Line
	4750 3300 4750 2600
Wire Wire Line
	4400 3400 4850 3400
Wire Wire Line
	4850 3400 4850 2600
Wire Wire Line
	4400 3500 4950 3500
Wire Wire Line
	4950 3500 4950 2600
Wire Wire Line
	4400 3600 5050 3600
Wire Wire Line
	5050 3600 5050 2600
Text GLabel 4400 5100 2    50   Input ~ 0
GND
Text GLabel 4650 2600 3    50   Input ~ 0
GND
Text GLabel 5250 3100 1    50   Input ~ 0
GND
$Comp
L Connector:Conn_01x08_Female J4
U 1 1 60EB76B2
P 3250 3950
F 0 "J4" H 3142 3325 50  0000 C CNN
F 1 "Conn_01x08_Female_Display" H 3142 3416 50  0000 C CNN
F 2 "Connector_JST:JST_PH_S8B-PH-K_1x08_P2.00mm_Horizontal" H 3250 3950 50  0001 C CNN
F 3 "~" H 3250 3950 50  0001 C CNN
	1    3250 3950
	1    0    0    -1  
$EndComp
Text GLabel 3550 3750 2    50   Input ~ 0
GND
Wire Wire Line
	3500 4350 3500 4800
Wire Wire Line
	3500 4800 4400 4800
Wire Wire Line
	3600 4250 3600 4700
Wire Wire Line
	3600 4700 4400 4700
Wire Wire Line
	3700 4150 3700 4600
Wire Wire Line
	3700 4600 4400 4600
Wire Wire Line
	3800 4050 3800 4500
Wire Wire Line
	3800 4500 4400 4500
Wire Wire Line
	3900 3950 3900 4400
Wire Wire Line
	3900 4400 4400 4400
Wire Wire Line
	4000 3850 4000 4300
Wire Wire Line
	4000 4300 4400 4300
Text Label 3250 3850 0    50   ~ 0
MOSI
Text Label 3250 3950 0    50   ~ 0
CLK
Text Label 3250 4050 0    50   ~ 0
CS
Text Label 3250 4150 0    50   ~ 0
DC
Text Label 3250 4250 0    50   ~ 0
RST
Text Label 3250 4350 0    50   ~ 0
BUSY
Wire Wire Line
	3050 3750 3550 3750
Wire Wire Line
	3050 3850 4000 3850
Wire Wire Line
	3050 3950 3900 3950
Wire Wire Line
	3050 4050 3800 4050
Wire Wire Line
	3050 4150 3700 4150
Wire Wire Line
	3050 4250 3600 4250
Wire Wire Line
	3050 4350 3500 4350
$Comp
L Switch:SW_Push SW1
U 1 1 610E97B1
P 3450 3200
F 0 "SW1" H 3450 3485 50  0000 C CNN
F 1 "Bootloader_Push" H 3450 3394 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 3450 3400 50  0001 C CNN
F 3 "~" H 3450 3400 50  0001 C CNN
	1    3450 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 3200 4400 3200
Text GLabel 3250 3200 0    50   Input ~ 0
GND
$Comp
L Switch:SW_Push SW2
U 1 1 610ECECB
P 5900 3200
F 0 "SW2" H 5900 3485 50  0000 C CNN
F 1 "RST_Push" H 5900 3394 50  0000 C CNN
F 2 "Button_Switch_THT:SW_Tactile_SKHH_Angled" H 5900 3400 50  0001 C CNN
F 3 "~" H 5900 3400 50  0001 C CNN
	1    5900 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 3200 5700 3200
Text GLabel 6100 3200 2    50   Input ~ 0
GND
Text GLabel 3550 3650 2    50   Input ~ 0
3v3
Wire Wire Line
	3050 3650 3550 3650
Text GLabel 4550 2600 3    50   Input ~ 0
3v3
Text GLabel 4400 3100 2    50   Input ~ 0
3v3
Text GLabel 6350 3650 0    50   Input ~ 0
3v3
Wire Wire Line
	5250 3500 5750 3500
Text Label 5500 3500 0    50   ~ 0
RX-TX
Text Label 5500 3600 0    50   ~ 0
TX-RX
Wire Wire Line
	5250 3600 5650 3600
NoConn ~ 6350 3750
NoConn ~ 6350 4150
$Comp
L Connector:Conn_01x06_Female J5
U 1 1 62A72A06
P 6550 3850
F 0 "J5" H 6578 3826 50  0000 L CNN
F 1 "Conn_01x06_Female_RS232_USB" H 6578 3735 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 6550 3850 50  0001 C CNN
F 3 "~" H 6550 3850 50  0001 C CNN
	1    6550 3850
	1    0    0    -1  
$EndComp
Text GLabel 6350 4050 0    50   Input ~ 0
GND
Wire Wire Line
	6350 3950 5750 3950
Wire Wire Line
	5750 3500 5750 3950
Wire Wire Line
	6350 3850 5650 3850
Wire Wire Line
	5650 3850 5650 3600
NoConn ~ 4400 3700
$EndSCHEMATC
