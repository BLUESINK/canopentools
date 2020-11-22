class tree:
	def __init__(self, name=''):
		self.childList = list()
		self.data = dict()
		self.name = name

	def addChild(self, child):
		self.childList.append(child)

	def findChild(self, name):
		x = list()
		for child in self.childList:
			if child.name == name :
				x.append(child)
		return x

	def removeChild(self, name):
		x = self.findChild(name)
		for child in x:
			del self.childList[self.childList.index(child)]

	def show(self, indent=''):
		print('%s[%s]'%(indent, self.name))
		keys = self.data.keys()
		for key in keys :
			print('%s %s = %s'%(indent, key, self.data[key]))

		print('')

		for child in self.childList :
			child.show(indent + '\t')

	def makeDict(self):
		data = dict()
		data['name'] = self.name
		data['data'] = self.data
		data['child'] = dict()
		data['childLen'] = len(self.childList)
		for child in self.childList :
			data['child'][child.name] = child.makeDict()

		return data


if __name__ == "__main__":

	# For Test
	x = tree()
	y = tree("y")

	x.addChild(y)
	x.show()

	x.data['string'] = "Hello I'm x"
	x.data['number'] = 200

	y.data['string'] = "Hello I'm y"
	y.data['number'] = 100

	x.show()

	z = x.makeDict()
	print(z)

	print('')

	x.removeChild("y")
	x.show()
