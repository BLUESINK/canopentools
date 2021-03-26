from . import eds
from . import od
import sys
import os
import datetime
import json
import argparse
import git

def generate(eds_filepath, output_filepath, show):

	# Read EDS File
	rootItem = eds.load(eds_filepath)
	rootItem = od.postproc(rootItem)

	# Create data.json for autogeneration
	root_filepath = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'root')
	f = open(os.path.join(root_filepath, 'data.json'), 'w+')
	
	if show :
		rootItem.show()

	# Create dict
	data = rootItem.makeDict()

	repo = git.Repo(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
	data['version'] = '0x' + repo.head.object.hexsha[:8].upper()

	f.write(json.dumps(data))
	f.close()

	# Autogen
	os.system('autogenerator ' + root_filepath + ' ' + output_filepath)

	date = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

if __name__ == "__main__":

	parser = argparse.ArgumentParser(description='BLUESINK CANOpen Slave Stack code generator')
	parser.add_argument('In', type=str, help='Location of CANOpen EDS File')
	parser.add_argument('Out', type=str, help='Destination folder path of CANOpen Slave Stack Code')

	args = parser.parse_args()

	generate(args.In, args.Out)