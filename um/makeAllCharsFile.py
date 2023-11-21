from random import randrange

with open("readAllChars.0", "wb") as allCharsFile:
        for i in range(0, 256):
                allCharsFile.write(chr(i).encode("charmap"))


NUM_CHARACTERS = 10000
with open("readRandomChars.0", "wb") as randCharsFile:
        for i in range(0, NUM_CHARACTERS):
                randCharsFile.write(chr(randrange(0, 256)).encode("charmap"))