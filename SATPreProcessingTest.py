#!/usr/bin/python

import sys
import subprocess as sb
import os
import fnmatch
import select
import collections

import config as cfg

Mode = cfg.Mode
algC = cfg.algC
MAX_DEPTH = cfg.MAX_DEPTH

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
fileIO = open(Algos[algC]+"ProfileResult.csv","w")
errFileIO = open(Algos[algC]+"ProfileError.txt","w")
errFileIO.close()
fileIO.close()
fileIO = open(Algos[algC]+"ProfileResult.csv","ab+")
errFileIO = open(Algos[algC]+"ProfileError.txt","ab+")


DataReuslts = collections.defaultdict(list)
ConflictResults = collections.defaultdict(list)
GPResults = collections.defaultdict(list)
fileIO.write("Depth,TimeElapsed AVG, TimeElapsed Max, TimeElapsed Min, Conf Avg,Conf Max,Conf Min,currGPAvg,currGPMax,currGPMin\n")
for file in files:
	#if ".48." in file or ".44." in file:
	#	continue
	#print file
	iRange = 10
	if (algC == 1):
		iRange = 1
	for i in range(iRange):
		try:
			filePath = ""
			if Mode is 0:
				filePath = testPath
			else:
				filePath = productionPath

			for iDepth in xrange(1,MAX_DEPTH+1):	
				cmd = './cs463project3 {2} {3} < .{0}{1}'.format(filePath,file,Algos[algC],iDepth)
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
						#print str(result.rstrip().split('=')[0])
						if str(result.rstrip().split('=')[0]) in "TimeSpent":
							DataReuslts[str(iDepth)].append(float(result.rstrip().split('=')[1]))
						if str(result.rstrip().split('=')[0]) in "Conflicts":
							ConflictResults[str(iDepth)].append(int(result.rstrip().split('=')[1]))
						if str(result.rstrip().split('=')[0]) in "MaxGP":
							GPResults[str(iDepth)].append(int(result.rstrip().split('=')[1]))
		except Exception as e:
			errFileIO.write(file + " is dead because " + str(e))
			pass
		errFileIO.flush()


for iDepth in xrange(1,MAX_DEPTH+1):

	currConf = ConflictResults[str(iDepth)]
	#print currConf
	currConflictAvg = sum(currConf)/len(currConf)
	currMaxConf = max(currConf)
	currMinConf = min(currConf)

	currTimeCollection = DataReuslts[str(iDepth)]
	#print currTimeCollection
	currTimeAvg = sum(currTimeCollection)/len(currTimeCollection)
	currTimeMax = max(currTimeCollection)
	currTimeMin = min(currTimeCollection)

	currGP = GPResults[str(iDepth)]
	currGPAvg = sum(currGP)/len(currGP)
	currGPMax = max(currGP)
	currGPMin = min(currGP)

	fileIO.write("{0},{1},{2},{3},{4},{5},{6},{7},{8},{9}\n".format(iDepth,
		currTimeAvg,currTimeMax,currTimeMin,
		currConflictAvg,currMaxConf,currMinConf,
		currGPAvg,currGPMax,currGPMin)
	)

errFileIO.close()
fileIO.close()
#print currSubProcess
#for l in currSubProcess.stdout.readline():
#	print l
#	sys.stdout.flush()
