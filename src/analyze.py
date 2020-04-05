import r2pipe, os, sys, resource, json 

if len(sys.argv) < 2:
	print("Please specify a file name.")
	exit(1)

class Function():
	def __init__(self):
		self.offset = 0
		self.size = 0
		self.signature = ""
		self.ispure = False
		self.name = ""
		self.cc = 0
		self.bits = 64
		self.aliases = list()

class Class():
	def __init__(self):
		self.base_classes = list()
		self.methods = list()
		self.name = ""

def FindVTableByOffset(vtables, offset):
	for tbl in vtables:
		if tbl["offset"] == offset: return tbl
	return None

def FindFunctionByOffset(fntbl, offset):
	try: 
		for fn in fntbl:
			for xref in fn["datarefs"]:
				if xref == offset:
					function = {}
					function["offset"] = fn["offset"]
					function["name"] = fn["name"]
					function["size"] = fn["size"]
					function["ispure"] = fn["is-pure"]
					function["signature"] = fn["signature"]
					function["cc"] = fn["cc"]
					function["bits"] = fn["bits"]
					function["aliases"] = []
					return function 
	except:
		return None
	return None 

r2 = r2pipe.open(sys.argv[1])

print("Initial analysis")

# Do a full analysis on the binary 
# We will do the following:
# aaaa
# aavt 
# aavr 
print("Running aaa")
r2.cmd("aaaa")
print("Running aavt")
r2.cmd("aavt")
r2.cmd("aad")
print("Running aavr")
r2.cmd("aavr")
print("Running aad")
#r2.cmd("aad")

# Get the class list
classes = r2.cmdj("aclj")

# Get the vtable list
vtables = r2.cmdj("avj")

# Get the list of functions 
functions = r2.cmdj("aflj")

# Import table
imports = r2.cmdj("iij")

_classes = []

for _class in classes:
	classdesc = {}
	classdesc["name"] = _class["name"]
	classdesc["base_classes"] = []
	classdesc["methods"] = []

	print("Resolving " + classdesc["name"])

	# Find bases 
	for base in _class["bases"]:
		classdesc["base_classes"].append(base["name"])

	for tbl in _class["vtables"]:
		vtbl = FindVTableByOffset(vtables, tbl["addr"])
		if vtbl is None: continue
		for method in vtbl["methods"]:
			func = FindFunctionByOffset(functions, method["offset"])
			if func is None: continue
			classdesc["methods"].append(func)

	for method in _class["methods"]:
		offset = method["addr"]
		func = FindFunctionByOffset(functions, offset)
		if func is None:
			function = {}
			function["offset"] = 0
			function["name"] = method["name"] + "_unresolved"
			function["size"] = 0
			function["ispure"] = True 
			function["signature"] = ""
			function["cc"] = "amd64"
			function["bits"] = "64"
			function["aliases"] = []
			#classdesc["methods"].append(function)
		else: 
			func["aliases"].append(method["name"])
			#classdesc["methods"].append(func)

	_classes.append(classdesc)

# Dump the classes to json
with open("out.json", "w") as fs: 
	json.dump(_classes, fs, indent=4)


print("Finished")