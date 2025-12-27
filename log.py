import os
import sys
import time

# get file path
if len(sys.argv) != 2:
	print('usage: python log.py [file]')
	raise SystemExit
path: str = sys.argv[1]

# file stats
last_update: float = 0.0
last_cursor: int = 0

try:
	# main loop
	while True:
		# get file stats
		try:
			stats = os.stat(path)
		except Exception as exc:
			print("\x1b[38;5;9mError occured:")
			print(exc, end = "\x1b[0m")
			break

		# ignore if file hasn't changed
		if last_update == stats.st_mtime:
			time.sleep(0.05)
			continue

		# print new data
		with open(path, "r") as file:
			# skip printed lines
			file.seek(last_cursor)

			# read new data
			print(file.read(), end = '')
			sys.stdout.flush()

			# store new cursor
			last_cursor = file.tell()

# keyboard interrupt without callstack
except KeyboardInterrupt:
	print("\n\x1b[38;5;12mKeyboard Interrupt\x1b[0m")
	raise SystemExit