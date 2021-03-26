{%- set INFO = data['child']['INFO']['child'] -%}
{%- set OD = data['child']['OD']['child'] -%}
#ifndef __CANOPEN_CONFIG_H__
#define __CANOPEN_CONFIG_H__

#define VERSION			{{ data['version'] }}

#define NrOfRXPDO 		{{ INFO.DeviceInfo['data'].NrOfRXPDO }}
#define NrOfTXPDO 		{{ INFO.DeviceInfo['data'].NrOfTXPDO }}

#define CO_FIFO_NUM  	10

#define HEARTBEAT		{% if "1017" in OD %}1{% else %}0{% endif %}

#endif