#define _CRT_SECURE_NO_WARNINGS
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
struct trienode {
	int num;
	char let;
	struct trienode** children;
} trienode;
void trieInsert(struct trienode* root, char* word)
{
	int ok = 0;
	if (root->num == 0) {
		root->children = (struct trienode**)malloc(sizeof(struct trienode*));
		root->num = 1;
		struct trienode* node = (struct trienode*)malloc(sizeof(trienode));
		node->let = word[0];
		root->children[0] = node;
		node->num = 0;
		node->children = NULL;
		ok = 1;
		if (word[0] != '\0')
			trieInsert(node, word + 1);
		return;
	}
	if (word[0] == '\0')
		return;
	for (int i = 0; i < root->num; i++) {
		if (word[0] == root->children[i]->let) {
			ok = 1;
			trieInsert(root->children[i], word + 1);
			break;
		}
	}
	if (ok == 0) {
		root->num++;
		int copy = root->num;
		root->children = (struct trienode**)realloc(root->children, copy * sizeof(struct trienode*));
		struct trienode* node = (struct trienode*)malloc(sizeof(trienode));
		node->let = word[0];
		root->children[root->num - 1] = node;
		node->num = 0;
		node->children = NULL;
		trieInsert(node, word + 1);
	}
}
void triePrint(struct trienode* root)
{
	if (root->let != '\0') {
		printf("%c", root->let);
	}
	if (root->let == '\0') {
		printf(" ");
		return;
	}
	for (int i = 0; i < root->num; i++)
		triePrint(root->children[i]);
}
int trieSearch(struct trienode* root, char* word)
{
	if ( root->let=='\0') {
		return 1;
	}
	int ok = 0;
	for (int i = 0; i < root->num; i++) {
		if (root->children[i]->let == word[0]) {
			printf("%c\n",root->let);
			trieSearch(root->children[i], word + 1);
			ok = 1;
			break;
		}
	}
	if (ok == 0) {
		printf("%d\n", root->let);
		printf("Not found!");
		return 0;
	}
}
int main()
{
	struct trienode* root;
	root = (struct trienode*)malloc(sizeof(struct trienode));
	root->let = '@';
	root->num = 0;
	root->children = NULL;
	FILE* f = fopen("words.in", "r");
	char words[100];
	int i = 0;
	while (!feof(f)) {
		fscanf(f, "%s", words);
		for (int i = 0; words[i]; i++) {
			words[i] = tolower(words[i]);
		}
		trieInsert(root, words);
	}
	for (int j = 0; j < 5000; j++) {
		scanf("%s", words);
		/*for (int i = 0; i<words[i]; i++) {
			while (strchr(",.;:[]{}()'*-/", words[i]) != NULL) {
				if (i == strlen(words))
					break;
				char* p = words + i;
				for (int i = 0; i < strlen(p); i++) {
					p[i] = p[i + 1];
				}
			}
			words[i] = tolower(words[i]);
		}*/
		if (trieSearch(root, words) == 0)
			printf("\n***%s***\n", words);
		else
			printf("found\n");
	}
	//triePrint(root);
}
