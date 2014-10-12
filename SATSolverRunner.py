#!/usr/bin/python

import sys
import subprocess as sb
import os
import fnmatch
import select
#hillClimb = 0
#dpll = 1
#Genetics = 2
algC = 1 #choice of algorithm
#Test = 0
#Production = 1
Mode = 0

testPath = "/tests/"
productionPath = "/3cnf_100atoms/"
path = os.path.dirname(os.path.abspath(__file__))
if Mode is 0:
	path = path + testPath
else:
	path = path + productionPath

Algos = ["hillClimb","dpll","Genetics"]
def find(pattern, path):
	result = []
	for root , dirs, files in os.walk(path):
		for name in files:
			if fnmatch.fnmatch(name, pattern):
				result.append(name)
	return result


#files = [f for f in os.listdir(path+"/tests/.") if os.path.isfile(f)]
#print files
files =  find('*.cnf',path)
#cmd = '{0}\\..\\Debug\\cs463project3.exe < "{0}\\test\\10.40.160707067.cnf"'.format(path)
#cmd = './cs463project3 < ./tests/{0}'.format(files[0])
fileIO = open(Algos[algC]+"Result.csv","w")
errFileIO = open(Algos[algC]+"Error.txt","w")
errFileIO.close()
fileIO.close()
fileIO = open(Algos[algC]+"Result.csv","ab+")
errFileIO = open(Algos[algC]+"Error.txt","ab+")
fileIO.write("FileName,SolveState,TimeElapsed,HighestC\n")
for file in files:
	#if ".48." in file or ".44." in file:
	#	continue
	#print file
	for i in range(10):
		try:
			filePath = ""
			if Mode is 0:
				filePath = testPath
			else:
				filePath = productionPath
			cmd = './cs463project3 {2}< .{0}{1}'.format(filePath,file,Algos[algC])
			print cmd
			currSubProcess = sb.Popen(cmd,shell=True,stdin=sb.PIPE,stdout=sb.PIPE,stderr=sb.PIPE)
			#poll_obj = select.poll()
			#poll_obj.register(currSubProcess.stdout,select.POLLIN)
			#time_limit = 0
			#pout = ""
			#while(pout is "" and time_limit < 500.0):
			#	poll_result = poll_obj.poll(0)
			#	if poll_result:
			#		pout = currSubProcess.stdout.readline()
			stdout, stderr = currSubProcess.communicate()
			pout = stdout
			print pout
			print stderr


			results = pout.rstrip().split('\t')
			myRes = file+"run{0}".format(i)
			for result in results:
					myRes += "," + result.rstrip().split('=')[1]
			myRes +="\n"
			fileIO.write(myRes)
		except Exception as e:
			errFileIO.write(file + " is dead because " + str(e))
			pass
		fileIO.flush()
		errFileIO.flush()
errFileIO.close()
fileIO.close()
#print currSubProcess
#for l in currSubProcess.stdout.readline():
#	print l
#	sys.stdout.flush()
