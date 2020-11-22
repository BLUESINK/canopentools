{%- set OD = data['child']['OD']['child'] -%}
#ifndef __OD_DEF_H__
#define __OD_DEF_H__

#include "OD.h"
{% for _,index in OD.items() %}
/******************************************************************************/
//0x{{ index.name }} - {{ index.data['ParameterName'] }}
/******************************************************************************/
{%- for _,subindex in index['child'].items() %}{% set bitlen = subindex.data['DataTypeStr'][1] %}
{%- if subindex.data['DataTypeStr'][2] == 'Basic' %}
{%- if subindex.data['AccessType'] == 'rw' %}
{{ subindex.data['DataTypeStr'][0] }} OD_0x{{ index.name }}_{{ subindex.name }}_rw[2];
{%- else %}
{{ subindex.data['DataTypeStr'][0] }} OD_0x{{ index.name }}_{{ subindex.name }};
{%- endif %}
{%- elif subindex.data['DataTypeStr'][2] == 'Extended' %}
{%- if subindex.data['AccessType'] == 'rw' %}
{{ subindex.data['DataTypeStr'][0] }} OD_0x{{ index.name }}_{{ subindex.name }}_rw[2][{{ (bitlen/8)|int }}];
{%- else %}
{{ subindex.data['DataTypeStr'][0] }} OD_0x{{ index.name }}_{{ subindex.name }}[{{ (bitlen/8)|int }}];
{%- endif %}
{%- endif %}
{%- endfor %}
OD_SUBINDEX OD_SUBINDEX_0x{{ index.name }}[] = {
  {%- for _,subindex in index['child'].items() %}
  {%- if subindex.data['DataTypeStr'][2] == "Basic" %}
  {%- if subindex.data['AccessType'] == 'rw' %}
  { {{ subindex.data['DataTypeStr'][1] }}, {{ subindex.data['AccessTypeStr'] }}, 0, OD_0x{{ index.name }}_{{ subindex.name }}_rw }, // {{ subindex.data['ParameterName'] }}
  {%- else %}
  { {{ subindex.data['DataTypeStr'][1] }}, {{ subindex.data['AccessTypeStr'] }}, 0, &OD_0x{{ index.name }}_{{ subindex.name }} }, // {{ subindex.data['ParameterName'] }}
  {%- endif %}
  {%- elif subindex.data['DataTypeStr'][2] == "Extended" %}
  {%- if subindex.data['AccessType'] == 'rw' %}
  { {{ subindex.data['DataTypeStr'][1] }}, {{ subindex.data['AccessTypeStr'] }}, 0, &OD_0x{{ index.name }}_{{ subindex.name }}_rw[0] }, // {{ subindex.data['ParameterName'] }}
  {%- else %}
  { {{ subindex.data['DataTypeStr'][1] }}, {{ subindex.data['AccessTypeStr'] }}, 0, OD_0x{{ index.name }}_{{ subindex.name }} }, // {{ subindex.data['ParameterName'] }}
  {%- endif %}
  {%- endif %}
  {%- endfor %}
};
OD_INDEX OD_0x{{ index.name }} = { {{ index.childLen }}, OD_SUBINDEX_0x{{ index.name }} };
{% endfor %}

/******************************************************************************/
//Final Object Dictionary
/******************************************************************************/
#define ODIndexNum    {{ data['child']['OD'].childLen }}
uint32_t ODIndex[] = {
	{%- for _,index in OD.items() %}
	0x{{ index.name }},
	{%- endfor %}
};

OD_INDEX* ODInstance[] = {
	{%- for _,index in OD.items() %}
	&OD_0x{{ index.name }},
	{%- endfor %}
};

OD ObjDic = {ODIndex, ODInstance};

#endif