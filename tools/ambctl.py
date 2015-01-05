#!/usr/bin/python

import argparse
import dbus
import sys
import json
import gobject
import fileinput
import termios, fcntl, os
import glib
import curses.ascii
from dbus.mainloop.glib import DBusGMainLoop

class bcolors:
		HEADER = '\x1b[95m'
		OKBLUE = '\x1b[94m'
		OKGREEN = '\x1b[92m'
		WARNING = '\x1b[93m'
		FAIL = '\x1b[91m'
		ENDC = '\x1b[0m'
		GREEN = '\x1b[32m'
		WHITE = '\x1b[37m'
		BLUE = '\x1b[34m'

def help():
		help = ("Available commands:\n"
						+bcolors.HEADER+ "help" +bcolors.WHITE+ "           Prints help data\n"
						+bcolors.HEADER+ "list" +bcolors.WHITE+ "           List supported ObjectNames\n"
						+bcolors.HEADER+ "get" +bcolors.WHITE+ "            Get properties from an ObjectName\n"
						+bcolors.HEADER+ "listen" +bcolors.WHITE+ "         Listen for changes on an ObjectName\n"
						+bcolors.HEADER+ "set" +bcolors.WHITE+ "            Set a property for an ObjectName\n"
						+bcolors.HEADER+ "getHistory" +bcolors.WHITE+ "     Get logged data within a time range\n"
						+bcolors.HEADER+ "quit" +bcolors.WHITE+ "           Exit ambctl\n")
		return help

def changed(interface, properties, invalidated):
	print json.dumps(properties, indent=2)

def processCommand(command, commandArgs, noMain=True):

	if command == 'help':
			print help()
			return 1

	dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
	bus = dbus.SystemBus()
	try:
			managerObject = bus.get_object("org.automotive.message.broker", "/");
			managerInterface = dbus.Interface(managerObject, "org.automotive.Manager")
	except:
			print "Error connecting to AMB.  is AMB running?"
			return 1

	if command == "list" :
		supportedList = managerInterface.List()
		for objectName in supportedList:
			print objectName
		return 1
	elif command == "get":
		if commandArgs[0] == "help":
			print "ObjectName [ObjectName...]"
			return 1

		for objectName in commandArgs:
			objects = managerInterface.FindObject(objectName)
			print objectName
			for o in objects:
				propertiesInterface = dbus.Interface(bus.get_object("org.automotive.message.broker", o),"org.freedesktop.DBus.Properties")
				print json.dumps(propertiesInterface.GetAll("org.automotive."+objectName), indent=2)
		return 1
	elif command == "listen":
		if len(commandArgs) == 0:
			commandArgs = ['help']
		if commandArgs[0] == "help":
			print "ObjectName [ObjectName...]"
			return 1
		for objectName in commandArgs:
			objects = managerInterface.FindObject(objectName)
			for o in objects:
				bus.add_signal_receiver(changed,
					dbus_interface="org.freedesktop.DBus.Properties",
					signal_name="PropertiesChanged", path=o)
		if noMain != True:
				try:
						main_loop = gobject.MainLoop(None, False)
						main_loop.run()
				except KeyboardInterrupt:
						return 1
	elif command == "set":
		if len(commandArgs) == 0:
			commandArgs = ['help']
		if len(commandArgs) and commandArgs[0] == "help":
			print "ObjectName PropertyName VALUE [ZONE]"
			return 1
		if len(commandArgs) < 3:
			print "set requires more arguments (see set help)"
			return 1
		objectName = commandArgs[0]
		propertyName = commandArgs[1]
		value = commandArgs[2]
		zone = 0
		if len(commandArgs) == 4:
			zone = int(commandArgs[3])
		object = managerInterface.FindObjectForZone(objectName, zone)
		propertiesInterface = dbus.Interface(bus.get_object("org.automotive.message.broker", object),"org.freedesktop.DBus.Properties")
		property = propertiesInterface.Get("org.automotive."+objectName, propertyName)
		if property.__class__ == dbus.Boolean:
			value = value.lower() == "true"
		realValue = property.__class__(value)
		propertiesInterface.Set("org.automotive."+objectName, propertyName, realValue)
		property = propertiesInterface.Get("org.automotive."+objectName, propertyName)
		if property == realValue:
			print propertyName + " = ", property
		else:
			print "Error setting property"
		return 1
	elif command == "getHistory":
		if len(commandArgs) == 0:
			commandArgs = ['help']
		if commandArgs[0] == "help":
			print "ObjectName [ZONE] [STARTTIME] [ENDTIME] "
			return 1
		if len(commandArgs) < 1:
			print "getHistory requires more arguments (see getHistory help)"
			return 1
		objectName = commandArgs[0]
		start = 1
		if len(commandArgs) >= 3:
			start = float(commandArgs[2])
		end = 9999999999
		if len(commandArgs) >= 4:
			end = float(commandArgs[3])
		zone = 0
		if len(commandArgs) >= 2:
			zone = int(commandArgs[1])
		object = managerInterface.FindObjectForZone(objectName, zone);
		propertiesInterface = dbus.Interface(bus.get_object("org.automotive.message.broker", object),"org.automotive."+objectName)
		print json.dumps(propertiesInterface.GetHistory(start, end), indent=2)
	else:
		print "Invalid command"
	return 1



parser = argparse.ArgumentParser(prog="ambctl", description='Automotive Message Broker DBus client tool', add_help=False)
parser.add_argument('command', metavar='COMMAND [help]', nargs='?', default='stdin', help='amb dbus command')
parser.add_argument('commandArgs', metavar='ARG', nargs='*',
			help='amb dbus command arguments')
parser.add_argument('-h', '--help', help='print help', action='store_true')

args = parser.parse_args()

if args.help:
		parser.print_help()
		print
		print help()
		sys.exit()

if args.command == "stdin":
		class Data:
				history = []
				line = ""
				templine = ""
				promptAmbctl = "[ambctl]"
				promptEnd = "# "
				fullprompt = promptAmbctl + promptEnd
				curpos = 0
				historypos = -1
				def full_line_len(self):
						return len(self.fullprompt) + len(self.line)
				def insert(self, str):
						if self.curpos == len(self.line):
								self.line+=str
								self.curpos = len(self.line)
						else:
								self.line = self.line[:self.curpos] + str + self.line[self.curpos:]
								self.curpos+=1
				def arrow_back(self):
						if self.curpos > 0:
								self.curpos-=1
								return True
						return False

				def arrow_forward(self):
						if self.curpos < len(self.line):
								self.curpos+=1
								return True
						return False

				def back_space(self):
						if self.curpos > 0:
								self.curpos-=1
								self.line = self.line[:self.curpos] + self.line[self.curpos+1:]
								return True
						return False
				def delete(self):
						if self.curpos < len(self.line):
								self.line = self.line[:self.curpos] + self.line[self.curpos+2:]
								return True
						return False

				def save_temp(self):
						if len(self.history)-1 == 0 or len(self.history)-1 != self.historypos:
								return
						self.templine = self.line

				def push(self):
						self.history.append(self.line)
						self.historypos = len(self.history)-1
						self.clear()

				def set(self, str):
						self.line = str
						self.curpos = len(self.line)

				def history_up(self):
						if self.historypos >= 0:
								self.line = self.history[self.historypos]
								if self.historypos != 0:
										self.historypos-=1
								return True
						return False

				def history_down(self):
						if self.historypos >= 0 and self.historypos < len(self.history)-1:
								self.historypos+=1
								self.line = self.history[self.historypos]

						else:
								self.historypos = len(self.history)-1
								self.set(self.templine)

						return True

				def clear(self):
						self.set("")
						templist = ""

		def erase_line():
				sys.stdout.write('\x1b[2K\x1b[80D')

		def cursor_left():
				sys.stdout.write('\x1b[1D')

		def cursor_right():
				sys.stdout.write('\x1b[1C')

		def display_prompt():
				sys.stdout.write(bcolors.OKBLUE+Data.promptAmbctl+bcolors.WHITE+Data.promptEnd);

		def redraw(data):
				erase_line()
				display_prompt()
				sys.stdout.write(data.line)
				cursorpos = len(data.line) - data.curpos
				for x in xrange(cursorpos):
						cursor_left()
				sys.stdout.flush()

		def handle_keyboard(source, cond, data):
						str = source.read()
						#print "char: ", ord(str)

						if len(str) > 1:
								if ord(str[0]) == 27 and ord(str[1]) == 91 and ord(str[2]) == 68: #left arrow
										if data.arrow_back():
												cursor_left()
												sys.stdout.flush()
								elif ord(str[0]) == 27 and ord(str[1]) == 91 and ord(str[2]) == 67: #right arrow
										if data.arrow_forward():
												cursor_right()
												sys.stdout.flush()
								elif ord(str[0]) == 27 and ord(str[1]) == 91 and ord(str[2]) == 70: #end
										while data.arrow_forward():
												cursor_right()
												sys.stdout.flush()
								elif ord(str[0]) == 27 and ord(str[1]) == 91 and ord(str[2]) == 72: #home
										while data.arrow_back():
												cursor_left()
										sys.stdout.flush()
								elif len(str) == 4 and ord(str[0]) == 27 and ord(str[1]) == 91 and ord(str[2]) == 51 and ord(str[3]) == 126: #del
										data.delete()
										redraw(data)
								elif ord(str[0]) == 27 and ord(str[1]) == 91 and ord(str[2]) == 65:
										#up arrow
										data.save_temp()
										data.history_up()
										while data.arrow_forward():
												cursor_right()
										redraw(data)
								elif ord(str[0]) == 27 and ord(str[1]) == 91 and ord(str[2]) == 66:
										#down arrow
										data.history_down()
										while data.arrow_forward():
												cursor_right()
										redraw(data)
						elif ord(str) == 10: #enter
								if data.line == "":
										return True
								print ""
								words = data.line.split(' ')
								if words[0] == "quit":
									termios.tcsetattr(fd, termios.TCSAFLUSH, old)
									fcntl.fcntl(fd, fcntl.F_SETFL, oldflags)
									sys.exit()
								try:
										if len(words) > 1:
												processCommand(words[0], words[1:])
										else:
												processCommand(words[0], [])
								except dbus.exceptions.DBusException, error:
										print error
								except:
										print "Error running command ", sys.exc_info()[0]
								data.push();
								data.clear()
								redraw(data)
						elif ord(str) == 127: #backspace
								data.back_space()
								redraw(data)
						elif curses.ascii.isalnum(ord(str)) or ord(str) == curses.ascii.SP: #regular text
								data.insert(str)
								redraw(data)

						return True
		print "@PROJECT_PRETTY_NAME@ @PROJECT_VERSION@"

		data = Data()
		fd = sys.stdin.fileno()
		old = termios.tcgetattr(fd)
		new = termios.tcgetattr(fd)
		new[3] = new[3] & ~termios.ICANON & ~termios.ECHO
		termios.tcsetattr(fd, termios.TCSANOW, new)

		oldflags = fcntl.fcntl(fd, fcntl.F_GETFL)
		fcntl.fcntl(fd, fcntl.F_SETFL, oldflags | os.O_NONBLOCK)

		io_stdin = glib.IOChannel(fd)
		io_stdin.add_watch(glib.IO_IN, handle_keyboard, data)

		try:
				erase_line()
				display_prompt()
				sys.stdout.flush()
				main_loop = gobject.MainLoop(None, False)
				main_loop.run()
		except KeyboardInterrupt:
				sys.exit()
		finally:
				termios.tcsetattr(fd, termios.TCSAFLUSH, old)
				fcntl.fcntl(fd, fcntl.F_SETFL, oldflags)
				sys.exit()

else:
	try:
		processCommand(args.command, args.commandArgs, False)
	except dbus.exceptions.DBusException, error:
		print error
