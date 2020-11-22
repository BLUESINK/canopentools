import json
import os
from shutil import copyfile, rmtree

from genslave import eds, od 

def generate(eds_filepath, desc_folderpath, output_folderpath, show):

	# Create Root folder
	root_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'root_tmp')
	template_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'template')
	if os.path.isdir(root_path) :
		rmtree(root_path)
	os.mkdir(root_path)
	os.mkdir(os.path.join(root_path, 'tmpl'))
	copyfile(os.path.join(template_path,'od.doc'), os.path.join(root_path,'tmpl', 'od.doc'))

	# Read EDS File
	rootItem = eds.load(eds_filepath)
	rootItem = od.postproc(rootItem)
	

	# create prefix from file name
	prefix = os.path.basename(eds_filepath).replace('.', '_').replace('-', '_').lower()
	
	data = rootItem.makeDict()
	data['prefix'] = prefix
	f = open(os.path.join(root_path, 'data.json'), 'w+')
	f.write(json.dumps(data))
	f.close()

	if show :
		rootItem.show()

	OD = data['child']['OD']['child']
	for key in OD.keys():
		# create expand file
		subfolder_path = os.path.join(root_path, 'sub_' + key)
		os.mkdir(subfolder_path)
		open(os.path.join(subfolder_path, 'expand'), 'w').close()

		index = OD[key]
		index['prefix'] = prefix

		# Add Index description
		desc_path = os.path.join(desc_folderpath, 'key.txt')
		if os.path.isfile(desc_path) :
			f = open(desc_path, 'r')
			index['description'] = f.read()
			f.close()

		# Add SubIndex description
		for subkey in index['child'].keys() :
			subdesc_path = os.path.join(desc_folderpath, key + '_' + subkey + '.txt')
			if os.path.isfile(subdesc_path) :
				f = open(subdesc_path, 'r')
				index['child'][subkey]['description'] = f.read()
				f.close()

		# Create data.json in sub folder
		f = open(os.path.join(subfolder_path,'data.json'), 'w')
		f.write(json.dumps(index))
		f.close()

		os.mkdir(os.path.join(subfolder_path, 'tmpl'))
		copyfile(os.path.join(template_path, 'od_index.doc'), os.path.join(subfolder_path, 'tmpl', 'od_'+key+'.doc'))

	# Generate!
	os.system('autogenerator ' + root_path + ' ' + output_folderpath)