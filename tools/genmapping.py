#!/usr/bin/python

import os
import argparse

parser = argparse.ArgumentParser(description='Process DBus mappings.')
parser.add_argument('mappingFiles', metavar='N', nargs='+',
			help='dbus headers to find mappings in')
parser.add_argument('--output', dest='output',
			help='output file to write idl to')
args = parser.parse_args()
print "parsing"
print args.mappingFiles

interfaces = []

class Member:
		ambName = ""
		memberName = ""

		def __repr__(self):
			return "Member"
		def toString(self):
			return "{" + self.ambName + " => " + self.memberName + "}"
		def toIdl(self):
			return '  const DOMString ' + self.ambName + ' = "' + self.memberName + '";\n'

class Interface:
		def __init__(self):
			self.name = ""
			self.members = []
		def __repr__(self):
			return "Interface('" + name + "')"
		def toString(self):
			output = self.name + ":"
			for member in self.members:
				output += member.toString() + ","
			return output
		def toIdl(self):
			output = "interface " + self.name + " {\n"
			for member in self.members:
				output += member.toIdl()
			output += "\n};\n"
			return output

for input in args.mappingFiles:
	try: file = open(input)
	except IOError:
			print "Failed to open " + input
	with file:
			for line in file:
					i = line.find("DBusSink(\"");
					if i != -1:
							interface = Interface()
							ifaceNameBeg = line.find('("')
							ifaceNameEnd = line.find('",')
							interface.name = line[ifaceNameBeg+2 : ifaceNameEnd]
							interfaces.append(interface)
					wantPropertyVariant = 'wantPropertyVariant('
					i = line.find(wantPropertyVariant)
					if i!= -1:
							member = Member()
							ambNameEnd = line.find(', "')-2
							member.ambName = line[i+len(wantPropertyVariant) : i + ambNameEnd].replace("VehicleProperty::", "")
							memberNameBeg = line.find(', "')+3
							memberNameEnd = line.find('",')
							member.memberName = line[memberNameBeg : memberNameEnd]
							interfaces[-1].members.append(member)
			file.close()

try: outputFile = open(args.output, 'w')
except IOError:
		print "Error could not open output file: " + args.output
with outputFile:
	header =("/*!\n"
		" * \\name AMB to AMB-DBus Mapping Tables\n"
		" * \\file " + os.path.basename(args.output) + "\n"
		" * \\brief This describes the AMB internal property names to AMB DBus interface property names\n"
		" * AMB internal property names are designed to be flat variable names (ie, 'ConvertableRoofStatus').  The DBus\n"
		" * properties however follow the naming scheme defined in the W3C automotive business group vehicle <a href='http://w3c.github.io/automotive-bg/data_spec.html'>data specification</a>\n"
		" * The pattern each interface is 'const DOMString AMBProperty = DBusProperty' where 'AMBProperty' is the internal name and 'DBusProperty' is the DBus property name")
	header += " */\n\n"
	outputFile.write(header)
	for iface in interfaces:
		outputFile.write(iface.toIdl())
		outputFile.write("\n")
	outputFile.close()



