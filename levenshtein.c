#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>

int getLevenshteinDistance(char *, int, char *, int, int);
int findMin(int *, int);
int readDict();
void readSentence(char *);
int isWhitespace(char);
int calcTotalDistance(int);

char *dict[178700];
char **sentence;
int current_sentence_size = 100;

int main(int argc, char *argv[])
{
	int dict_size = readDict();
	if ((sentence = malloc(current_sentence_size * sizeof(char *))) == NULL) {
		printf("malloc error\n");
		return 1;
	}
	readSentence(argv[1]);	
	
	int totalDistance = calcTotalDistance(dict_size);	
	printf("%d\n", totalDistance);

	return 0;
}

int calcTotalDistance(int dict_size) {
	int i = 0;
	int totalDistance = 0;
	int wordDistance = INT_MAX;
	int j, lDistance, word_size, scrabbleLen;

	while (sentence[i] != NULL) {
		for (j = 0; j < dict_size; j++) {
			word_size = strlen(sentence[i]);
			scrabbleLen = strlen(dict[j]);	
			if (abs(word_size - scrabbleLen) >= wordDistance) {
				continue;
			}
			lDistance = getLevenshteinDistance(sentence[i], word_size, dict[j], scrabbleLen, wordDistance);
			if (lDistance == 0) {
				wordDistance = 0;
				break;
			}
			if (lDistance == -1) {
				continue;
			}
			if (lDistance < wordDistance) {
				wordDistance = lDistance;
			}
		}
		totalDistance += wordDistance;
		wordDistance = INT_MAX;
		i++;
	}
	return totalDistance;
}

void readSentence(char *filename)
{
	int word_size;
	int buf_size = 30;
	int current_word_size = buf_size;
	int word_count = 0;
	int c;
	char *tmp, *word;
	char **tmp_sentence;
	FILE *fp = fopen(filename, "r");
	
	while ((c = fgetc(fp)) != EOF) {
		if (!isalpha(c)) {
			continue;
		}
		if ((word = malloc(buf_size * sizeof(char))) == NULL) {
			fclose(fp);
			return;
		}
		word[0] = toupper(c);
		word_size = 1;
		while ((c = fgetc(fp)) && isalpha(c)) {
			if (current_word_size + 1 == word_size) {
				current_word_size += buf_size;
				if ((tmp = realloc(word, sizeof(char) * current_word_size)) == NULL) {
					fclose(fp);
					return;
				}
				word = tmp;
			}
			word[word_size++] = toupper(c);
		}
		word[word_size] = '\0';
		current_word_size = buf_size;
		if (word_count >= current_sentence_size) {
			current_sentence_size *= 10;
			if ((tmp_sentence = realloc(sentence, sizeof(char *) * current_sentence_size)) == NULL) {
				printf("malloc error\n");
				return;
			}
			sentence = tmp_sentence;	
		}
		sentence[word_count++] = word;
		if (c == EOF) {
			break;
		}
	}
	fclose(fp);
}


int read_dict()
{
	size_t buf_size = 20;
	char *line;
	int dict_size = 0;
	FILE *fp = fopen("/var/tmp/twl06.txt", "r");
	int i;

	if ((line = malloc(buf_size * sizeof(char))) == NULL) {
		fclose(fp);
		return -1;
	}
	while (getline(&line, &buf_size, fp) != -1) {
		if (strlen(line) == 0) {
			continue;
		}
		for (i = 0; isalpha(line[i]); i++) {
			;
		}
		line[i] = '\0';
		dict[dict_size++] = line;
		if ((line = malloc(buf_size * sizeof(char))) == NULL) {
			fclose(fp);
			return -1;
		}
	}
	fclose(fp);
	return dict_size;
}

int getLevenshteinDistance(char *s, int sLen, char *t, int tLen, int lookingFor)
{
	int d[sLen+1][tLen+1];
	int i, j;
	int transforms[3];
	int min;

	for (i = 0; i <= sLen; i++) {
		d[i][0] = i;
	}
	for (j = 0; j <= tLen; j++) {
		d[0][j] = j;
	}	
	for (j = 1; j <= tLen; j++) {
		min = INT_MAX;
		for (i = 1; i <= sLen; i++) {
			if (s[i-1] == t[j-1]) {
				d[i][j] = d[i-1][j-1];
			} else {
				transforms[0] = d[i-1][j] + 1;
				transforms[1] = d[i][j-1] + 1;
				transforms[2] = d[i-1][j-1] + 1;
				d[i][j] = findMin(transforms, 3);
			}
			if (d[i][j] < min) {
				min = d[i][j];
			}
		}
		if (min > lookingFor) {
			return -1;
		}
	}
	return d[sLen][tLen];
}

int findMin(int *n, int nLen) {
	int min = INT_MAX;
	int i;

	for (i = 0; i < nLen; i++) {
		if (n[i] < min) {
			min = n[i];
		}
	}
	return min;
}

