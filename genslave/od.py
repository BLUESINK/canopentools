import re
from . import eds
import copy

dataType = { # CiA 302 v4.0.2 Table 44
	'0x0001' : ['bool', 1, 'Basic', 'BOOLEAN'],
	'0x0002' : ['int8_t', 8, 'Basic', 'INTEGER8'],
	'0x0003' : ['int16_t', 16, 'Basic', 'INTEGER16'],
	'0x0004' : ['int32_t', 32, 'Basic', 'INTEGER32'],
	'0x0005' : ['uint8_t', 8, 'Basic', 'UNSIGNED8'],
	'0x0006' : ['uint16_t', 16, 'Basic', 'UNSIGNED16'],
	'0x0007' : ['uint32_t', 32, 'Basic', 'UNSIGNED32'],
	'0x0008' : ['float', 32, 'Basic', 'REAL32'],
	'0x0009' : ['uint8_t', 0, 'Extended', 'VISIBLE_STRING'],
	'0x000A' : ['uint8_t', 0, 'Extended', 'OCTET_STRING'],
	'0x000B' : ['uint16_t', 0, 'Extended', 'UNICODE_STRING'],
}

def postproc(rootItem) :

	odItem = rootItem.findChild("OD")
	if len(odItem) == 0 :
		return rootItem

	for index in odItem[0].childList :

		# [[ Add index (hex string to int) ]]
		index.data['index'] = int(index.name, 16)

		# [[ If it doesn't have child, copy to 0 ]]
		if len(index.childList) == 0 :
			index.addChild(copy.deepcopy(index))
			index.childList[0].name = '0'

		for subindex in index.childList :

			# [[ Change subindex name ]]
			subindex.name = subindex.name.zfill(2)

			# [[ Add Access type string ]]
			subindex.data['AccessTypeStr'] = ''
			
			if subindex.data['AccessType'] == 'ro':
				subindex.data['AccessTypeStr'] = 'OD_ACCESS_READ'
			elif subindex.data['AccessType'] == 'wo':
				subindex.data['AccessTypeStr'] = 'OD_ACCESS_WRITE'
			elif subindex.data['AccessType'] == 'rw':
				subindex.data['AccessTypeStr'] = 'OD_ACCESS_READ | OD_ACCESS_WRITE'
			elif subindex.data['AccessType'] == 'const':
				subindex.data['AccessTypeStr'] = 'OD_ACCESS_READ'			

			# [[ Add PDO Mappable string ]]
			if subindex.data['PDOMapping'] == '1':
				subindex.data['AccessTypeStr'] = subindex.data['AccessTypeStr'] + ' | OD_ACCESS_MAPPABLE'

			# [[ Add Data type string ]]
			if subindex.data['DataType'] in dataType:
				subindex.data['DataTypeStr'] = copy.deepcopy(dataType[subindex.data['DataType']])

				if subindex.data['DataTypeStr'][2] == 'Extended':
					if 'DefaultValue' in subindex.data :
						subindex.data['DataTypeStr'][1] = len(subindex.data['DefaultValue']) * 8
						subindex.data['DefaultValue'] = '"' + subindex.data['DefaultValue'] + '"'

			# [[ $NODEID ]]
			if 'DefaultValue' in subindex.data :
				subindex.data['DefaultValue'] = subindex.data['DefaultValue'].replace('$','')

	return rootItem


if __name__ == "__main__":

	# For test
	rootItem = eds.load('test.eds')
	rootItem = postproc(rootItem)
	rootItem.show()