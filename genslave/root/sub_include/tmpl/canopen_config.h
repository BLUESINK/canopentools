{%- set INFO = data['child']['INFO']['child'] -%}
#ifndef __CANOPEN_CONFIG_H__
#define __CANOPEN_CONFIG_H__

#define VERSION			{{ data['version'] }}

#define NrOfRXPDO 		{{ INFO.DeviceInfo['data'].NrOfRXPDO }}
#define NrOfTXPDO 		{{ INFO.DeviceInfo['data'].NrOfTXPDO }}

#define CO_FIFO_NUM  	10

#endif