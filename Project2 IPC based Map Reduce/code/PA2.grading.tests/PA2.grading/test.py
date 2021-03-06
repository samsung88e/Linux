import multiprocessing as mp
import json
import os
import sys

def validate(testcase):
	expectedPath = "grading/expected/" + testcase
	outputPath = "output/FinalData/"
	outputResult = {}

	with open(expectedPath, 'r') as f:
		expectedResult = json.load(f)

	for _, _, files in os.walk(outputPath):
		for file in files:
			if file.endswith(".txt"):
				with open(outputPath+file) as f:
					for line in f:
						key, value = line.strip().split(' ')
						# Remove .txt
						key = file[:-4]
						outputResult[key] = value

	if expectedResult == outputResult:
		return True
	else:
		s = set(expectedResult) - set(outputResult)
		if len(s) != 0:
			print("Key difference : expected - observed")
			print(s)
		
		s = set(outputResult) - set(expectedResult)
		if len(s) != 0:
			print("Key difference : observed - expected")
			print(s)

		valueDiff = False
		for key in expectedResult.keys():
			if key in outputResult.keys() and expectedResult[key] != outputResult[key]:
				if valueDiff == False:
					valueDiff = True
					print("Value difference: expected val - observed")
				if expectedResult[key] != outputResult[key]:
					print(key, " expected: ", expectedResult[key], " but observed: ", outputResult[key])
		return False

def runCode(cmds):
	os.system(cmds)

def procTerm(cmds, testcase):
	p = mp.Process(target=runCode, args=(cmds,))
	p.start()
	p.join(20)
	if p.is_alive():
		print("Terminating")
		p.terminate()
		p.join()
	return validate(testcase)

if __name__ == '__main__':
	os.system('make clean')
	os.system('make')
	basicCMD = "./mapreduce 6 3 grading/"
	score = 0

	if len(sys.argv) > 1:
		args = sys.argv[1:]
	else:
		args = ["T0", "T1", "T2", "T3", "T4", "T5", "T6", "T7"]
	for arg in args:
		print("Testcase " + arg + " starts")
		cmd = basicCMD + arg + "> /dev/null"
		try:
			isPassed = procTerm(cmd, arg)
		except Exception as e:
			print(e)
			print("Program terminated due to error or timeout!!!")
		if isPassed:
			print("Pass test " + arg)
			score += 1.0
		print("================================================")

	
	# T8 Min number
	print("Testcase T8 starts")
	cmd = "./mapreduce 1 1 grading/T3 > /dev/null"
	try:
		isPassed = procTerm(cmd, "T3")
	except Exception as e:
		print(e)
		print("Program terminated due to error or timeout!!!")
	if isPassed:
		print("Pass test T8")
		score += 1.0
	print("================================================")

	# T9 Max number
	print("Testcase T9 starts")
	cmd = "./mapreduce 20 20 grading/T3 > /dev/null"
	try:
		isPassed = procTerm(cmd, "T3")
	except Exception as e:
		print(e)
		print("Program terminated due to error or timeout!!!")
	if isPassed:
		print("Pass test T9")
		score += 1.0
	print("================================================")

#	# T10 Intermediate number
#	print("Testcase T10 starts")
#	cmd = "./mapreduce 10 10 grading/T3 > /dev/null"
#	try:
#		isPassed = procTerm(cmd, "T3")
#	except Exception as e:
#		print(e)
#		print("Program terminated due to error or timeout!!!")
#	if isPassed:
#		print("Pass test T10")
#		score += 1.0
#	print("================================================")

	print("Auto gradinging ended....")
	print("Total score: {}/10.0\n\n".format(score))

	# Argument checks
#	print("Argument check (manual inspection -- 0.5 pts)")
#	basicCMD = "./mapreduce "
#	cmds = ["10", "6 3", "4 grading/T3"]
#	isPassed = True
#	for cmd in cmds:
#		c = basicCMD + cmd
#		try:
#			isPassed = procTerm(c, "T3")
#		except Exception as e:
#			print(e)
#			print("Program terminated due to error or timeout!!!")
#			isPassed = False
#		if not isPassed:
#			break
#	print("================================================")

