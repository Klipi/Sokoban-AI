#!/usr/bin/env python

import sys
from subprocess import Popen, PIPE, STDOUT, call
from os import listdir
import time
import string
import threading

timeout = 15
verbose = False

def checkMap(file, logfile):

	logfile.write("Solving " + file + ":\n")
	# Get solution
	t0 = time.time()
	t = 0

	process = Popen(["./sokoban -timeout " + str(timeout) + " -notime < " + file], shell = True, stdout = PIPE)
	path = process.stdout.read();
	if "Timeout" in path:
		print "Solver timeout"
		logfile.write("Solver timeout\n\n")
		return 1

	solveTime = str(time.time() - t0)
	print "Map took " + solveTime + " seconds to solve."
	print "Path is: " + path

	with open(file, 'r') as f:
		map = f.read().splitlines()

	# Parse map
	listmap = []
	for line in map:
		listmap.append(list(line))
		if '@' in line:
			playerX = line.index('@')
			playerY = map.index(line)
		if '+' in line:
			playerX = line.index('@')
			playerY = map.index(line)

	dirDict = {	'U': [0, -1],
				'R': [1, 0],
				'D': [0, 1],
				'L': [-1, 0]}
	#print listmap
	for dir in path:
		if verbose:
			print "Moving " + dir
		move = dirDict.get(dir, [0, 0])
		newX = playerX + move[0]
		newY = playerY + move[1]
		newX2 = newX + move[0]
		newY2 = newY + move[1]

		if listmap[newY][newX] == '#':
			if verbose:
				print "Hit wall"
			continue

		if listmap[newY][newX] in ['$', '*']:
			if listmap[newY2][newX2] in ['$', '*', '#']:
				if verbose:
					print "Can't move box"
				continue
			else:
				listmap[playerY][playerX] = ' ' if listmap[playerY][playerX] == '@' else '.'
				playerX = newX
				playerY = newY
				listmap[playerY][playerX] = '@' if listmap[playerY][playerX] == '$' else '+'
				listmap[newY2][newX2] = '$' if listmap[newY2][newX2] == ' ' else '*'
				continue

		listmap[playerY][playerX] = ' ' if listmap[playerY][playerX] == '@' else '.'
		playerX = newX
		playerY = newY
		listmap[newY][newX] = '@' if listmap[newY][newX] == ' ' else '+'

		if verbose:
			for line in listmap:
				print string.join(line)

	ret = 0
	for line in listmap:
		print string.join(line)
		if '$' in line:
			ret = 2
	if ret == 0:
		print "Map solved correctly"
		logfile.write("Map solved correctly in " + solveTime + " seconds.\n\n")
	else:
		print "Map solved incorrectly"
		logfile.write("Map solved incorrectly.\n\n")

	return ret
def main(args):
	# Check all maps in folder
	folder = args[1]
	log = open(folder + "/sokoban.log", 'w')
	log.write("Solving maps in " + folder + " using timeout " + str(timeout) + " seconds\n\n")
	counts = [0, 0, 0]
	for f in listdir(folder):
		if ".in" in f:
			print f
			ret = checkMap(folder + f, log)
			counts[ret] += 1

	log.write(str(counts[0]) + " correct, " + str(counts[1]) + " timeouts, " + str(counts[2]) + " wrong answers")
	log.close()

if __name__=='__main__':
	sys.exit(main(sys.argv))
