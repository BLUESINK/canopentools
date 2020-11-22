import re
import copy
from . import tree as t

def load(filename):
	# Create Root Tree
	rootItem = t.tree()

	# Read EDS File
	f = open(filename, 'r')
	text = f.read().split('\n')
	f.close()

	li = list()
	index = -1

	# Parse eds file to tree list
	section = re.compile('^\[([\w\W]+)\]$')
	item = re.compile('^[\w\s]+=[\w\W]+$')

	for elem in text :
		if section.match(elem) : # Find section
			# Add new tree element
			li.append(t.tree(elem[1:-1]))
			index = index + 1

		if item.match(elem) : # Find item
			if index == -1 :
				continue

			[key, value] = elem.split("=", 1)
			li[index].data[key] = value


	# Reconstruct Object Dictionary elements
	od_index_item = re.compile('^([A-F]|[0-9]){4}$')
	id_subindex_item = re.compile('([A-F]|[0-9]){4}sub([A-F]|[0-9])+$')

	odTree = t.tree("OD")
	infoTree = t.tree("INFO")

	for lli in li :
		if od_index_item.match(lli.name) :
			odTree.addChild(lli)

		elif id_subindex_item.match(lli.name):
			[index, subIndex] = lli.name.split("sub", 1)
			x = odTree.findChild(index)

			if len(x) != 0 :
				lli.name = subIndex
				x[0].addChild(lli)
		else :
			infoTree.addChild(lli)


	rootItem.addChild(odTree)
	rootItem.addChild(infoTree)

	return rootItem

if __name__ == "__main__":

	# For test
	load('test.eds').show()