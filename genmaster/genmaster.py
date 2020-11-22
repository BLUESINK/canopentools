import os

def generate(output_folderpath):
	root_filepath = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'root')

	os.system('autogenerator ' + root_filepath + ' ' + output_folderpath)