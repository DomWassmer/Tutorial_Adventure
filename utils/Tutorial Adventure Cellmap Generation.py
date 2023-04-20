import random

cellSize = 16

spriteIndices = []
spriteRotations = []

randomSeedLevel1 = 2402
random.seed(randomSeedLevel1)

def printCellInfo(i_spriteIndices, i_spriteRotations):
	print("Sprite indices for level 1:\n[")
	for i in range(cellSize):
		if i == cellSize - 1:
			print(str(i_spriteIndices[i * 8 + 0]) + ", " \
			+ str(i_spriteIndices[i * 8 + 1]) + ", " \
			+ str(i_spriteIndices[i * 8 + 2]) + ", " \
			+ str(i_spriteIndices[i * 8 + 3]) + ", " \
			+ str(i_spriteIndices[i * 8 + 4]) + ", " \
			+ str(i_spriteIndices[i * 8 + 5]) + ", " \
			+ str(i_spriteIndices[i * 8 + 6]) + ", " \
			+ str(i_spriteIndices[i * 8 + 7]) + ", " \
			+ str(i_spriteIndices[i * 8 + 8]) + ", " \
			+ str(i_spriteIndices[i * 8 + 9]) + ", " \
			+ str(i_spriteIndices[i * 8 + 10]) + ", " \
			+ str(i_spriteIndices[i * 8 + 11]) + ", " \
			+ str(i_spriteIndices[i * 8 + 12]) + ", " \
			+ str(i_spriteIndices[i * 8 + 13]) + ", " \
			+ str(i_spriteIndices[i * 8 + 14]) + ", " \
			+ str(i_spriteIndices[i * 8 + 15]))
			continue
		print(str(i_spriteIndices[i * 8 + 0]) + ", " \
		+ str(i_spriteIndices[i * 8 + 1]) + ", " \
		+ str(i_spriteIndices[i * 8 + 2]) + ", " \
		+ str(i_spriteIndices[i * 8 + 3]) + ", " \
		+ str(i_spriteIndices[i * 8 + 4]) + ", " \
		+ str(i_spriteIndices[i * 8 + 5]) + ", " \
		+ str(i_spriteIndices[i * 8 + 6]) + ", " \
		+ str(i_spriteIndices[i * 8 + 7]) + ", " \
		+ str(i_spriteIndices[i * 8 + 8]) + ", " \
		+ str(i_spriteIndices[i * 8 + 9]) + ", " \
		+ str(i_spriteIndices[i * 8 + 10]) + ", " \
		+ str(i_spriteIndices[i * 8 + 11]) + ", " \
		+ str(i_spriteIndices[i * 8 + 12]) + ", " \
		+ str(i_spriteIndices[i * 8 + 13]) + ", " \
		+ str(i_spriteIndices[i * 8 + 14]) + ", " \
		+ str(i_spriteIndices[i * 8 + 15]) + ", ")

	print("]\n\n\n\nSprite rotations for level 1:\n[")
	for i in range(cellSize):
		if i == cellSize - 1:
			print(str(i_spriteRotations[i * 8 + 0]) + ", " \
			+ str(i_spriteRotations[i * 8 + 1]) + ", " \
			+ str(i_spriteRotations[i * 8 + 2]) + ", " \
			+ str(i_spriteRotations[i * 8 + 3]) + ", " \
			+ str(i_spriteRotations[i * 8 + 4]) + ", " \
			+ str(i_spriteRotations[i * 8 + 5]) + ", " \
			+ str(i_spriteRotations[i * 8 + 6]) + ", " \
			+ str(i_spriteRotations[i * 8 + 7]) + ", " \
			+ str(i_spriteRotations[i * 8 + 8]) + ", " \
			+ str(i_spriteRotations[i * 8 + 9]) + ", " \
			+ str(i_spriteRotations[i * 8 + 10]) + ", " \
			+ str(i_spriteRotations[i * 8 + 11]) + ", " \
			+ str(i_spriteRotations[i * 8 + 12]) + ", " \
			+ str(i_spriteRotations[i * 8 + 13]) + ", " \
			+ str(i_spriteRotations[i * 8 + 14]) + ", " \
			+ str(i_spriteRotations[i * 8 + 15]))
			continue
		print(str(i_spriteRotations[i * 8 + 0]) + ", " \
		+ str(i_spriteRotations[i * 8 + 1]) + ", " \
		+ str(i_spriteRotations[i * 8 + 2]) + ", " \
		+ str(i_spriteRotations[i * 8 + 3]) + ", " \
		+ str(i_spriteRotations[i * 8 + 4]) + ", " \
		+ str(i_spriteRotations[i * 8 + 5]) + ", " \
		+ str(i_spriteRotations[i * 8 + 6]) + ", " \
		+ str(i_spriteRotations[i * 8 + 7]) + ", " \
		+ str(i_spriteRotations[i * 8 + 8]) + ", " \
		+ str(i_spriteRotations[i * 8 + 9]) + ", " \
		+ str(i_spriteRotations[i * 8 + 10]) + ", " \
		+ str(i_spriteRotations[i * 8 + 11]) + ", " \
		+ str(i_spriteRotations[i * 8 + 12]) + ", " \
		+ str(i_spriteRotations[i * 8 + 13]) + ", " \
		+ str(i_spriteRotations[i * 8 + 14]) + ", " \
		+ str(i_spriteRotations[i * 8 + 15]) + ", ")
	print("]")

def distributeSpriteIndex(i_number):
	if i_number < 0.4:
		return 0
	if i_number < 0.5:
		return 1
	if i_number < 0.6:
		return 2
	if i_number < 0.7:
		return 3
	if i_number < 0.8:
		return 4
	if i_number < 0.9:
		return 5
	return 1

def distributeSpriteOrientation(i_spriteIndex):
	if i_spriteIndex == 0:
		return 0
	return random.randint(0, 3)

for i in range(16 * 16):
	spriteIndex = distributeSpriteIndex(random.random())
	spriteIndices.append(spriteIndex)
	spriteRotations.append(distributeSpriteOrientation(spriteIndex))

printCellInfo(spriteIndices, spriteRotations)