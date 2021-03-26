{%- set INFO = data['child']['INFO']['child'] -%}
#ifndef __CANOPEN_CONFIG_H__
#define __CANOPEN_CONFIG_H__

#define VERSION			{{ data['version'] }}

#define NrOfRXPDO 		{{ INFO.DeviceInfo['data'].NrOfRXPDO }}
#define NrOfTXPDO 		{{ INFO.DeviceInfo['data'].NrOfTXPDO }}

#define CO_FIFO_NUM  	10

#define NMT     		0x00
#define SYNC    		0x01
#define EMCY    		0x01
#define TIME    		0x02

#define txPDO1  		0x03
#define rxPDO1  		0x04
#define txPDO2  		0x05
#define rxPDO2  		0x06
#define txPDO3  		0x07
#define rxPDO3  		0x08
#define txPDO4  		0x09
#define rxPDO4  		0x0A

#define txSDO   		0x0B
#define rxSDO   		0x0C

#define HTBT   			0x0E

#endif