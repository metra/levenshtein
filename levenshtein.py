#!/usr/bin/env python
import sys
import re

# adapted from Peter Norvig's spell corrector
# http://norvig.com/spell-correct.html

alphabet = 'abcdefghijklmnopqrstuvwxyz'

def words(text):
	return re.findall('[a-z]+', text.lower())

def edit(word):
	splits = [(word[:i], word[i:]) for i in range(len(word) + 1)]
	deletes = [a + b[1:] for a, b in splits if b]
	replaces = [a + c + b[1:] for a,b in splits for c in alphabet if b]
	inserts = [a + c + b for a, b in splits for c in alphabet]
	return frozenset(deletes + replaces + inserts)

dictionary = frozenset(words(file('/var/tmp/twl06.txt').read()))
sentence = words(file(sys.argv[1]).read())

total_edit_count = 0
for word in sentence:
	if word in dictionary:
		continue
		
	edits = set()
	edits.add(word)
	edit_count = 1
	while True:
		next_edits = set()
		for e in edits:
			next_sub_edits = edit(e)
			if not next_sub_edits.isdisjoint(dictionary):
				break
			next_edits.update(next_sub_edits)
		else:
			edit_count += 1
			edits = next_edits
			continue
		total_edit_count += edit_count
		break
	
sys.stdout.write('%d\n' % total_edit_count)