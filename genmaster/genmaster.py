import os

def change_name(folderpath, filename, old_extend, new_extend):
	filepath = os.path.join(folderpath, filename + old_extend)
	if os.path.isfile(filepath):
		new_filepath = os.path.join(folderpath, filename + new_extend)

		os.rename(filepath, new_filepath)

def generate(output_folderpath, cpp):
	root_filepath = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'root')

	if cpp :
		change_name(output_folderpath, "CANOpen_hw_appl", ".cpp", ".c")
		change_name(output_folderpath, "CANOpen", ".cpp", ".c")

	os.system('autogenerator ' + root_filepath + ' ' + output_folderpath)

	if cpp :
		change_name(output_folderpath, "CANOpen_hw_appl", ".c", ".cpp")
		change_name(output_folderpath, "CANOpen", ".c", ".cpp")