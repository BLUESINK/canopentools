{%- set OD = data['child']['OD']['child'] -%}
#ifndef __CANOPEN_OD_ACCESS_H__
#define __CANOPEN_OD_ACCESS_H__

#include "OD.h"

{% for _,index in OD.items() %}
{%- for _,subindex in index['child'].items() %}{% set bitlen = subindex.data['DataTypeStr'][1] %}
{%- if subindex.data['DataTypeStr'][2] == 'Basic' %}
{%- if subindex.data['AccessType'] == 'rw' %}
extern {{ subindex.data['DataTypeStr'][0] }} OD_0x{{ index.name }}_{{ subindex.name }}_rw[2]; 		// [{{subindex.data['AccessType']}}] {{ index.data['ParameterName'] }} - {{ subindex.data['ParameterName']}}
#define OD_0x{{ index.name }}_{{ subindex.name }}				OD_0x{{ index.name }}_{{ subindex.name }}_rw[0]
#define OD_0x{{ index.name }}_{{ subindex.name }}_buf			OD_0x{{ index.name }}_{{ subindex.name }}_rw[1]
{%- else %}
extern {{ subindex.data['DataTypeStr'][0] }} OD_0x{{ index.name }}_{{ subindex.name }}; 			// [{{subindex.data['AccessType']}}] {{ index.data['ParameterName'] }} - {{ subindex.data['ParameterName']}}
{%- endif %}
{%- elif subindex.data['DataTypeStr'][2] == 'Extended' %}
{%- if subindex.data['AccessType'] == 'rw' %}
{{ subindex.data['DataTypeStr'][0] }} OD_0x{{ index.name }}_{{ subindex.name }}_rw[2][{{ (bitlen/8)|int }}]; 		// [{{subindex.data['AccessType']}}] {{ index.data['ParameterName'] }} - {{ subindex.data['ParameterName']}}
#define OD_0x{{ index.name }}_{{ subindex.name }}				OD_0x{{ index.name }}_{{ subindex.name }}_rw[0]
#define OD_0x{{ index.name }}_{{ subindex.name }}_buf			OD_0x{{ index.name }}_{{ subindex.name }}_rw[1]
{%- else %}
extern {{ subindex.data['DataTypeStr'][0] }} OD_0x{{ index.name }}_{{ subindex.name }}[{{ (bitlen/8)|int }}]; 		// [{{subindex.data['AccessType']}}] {{ index.data['ParameterName'] }} - {{ subindex.data['ParameterName']}}
{%- endif %}
{%- endif %}
{%- endfor %}
{% endfor %}

#endif
