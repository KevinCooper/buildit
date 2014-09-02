//===-- dictionary.c ---------------------------------------------*- C -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------===//

/* Given a char*, returns a substring that starts at the first
 alphabet character and ends at the last alphabet character, i.e. it
 strips off beginning or ending quotes, punctuation, etc. */
#include "dictionary.h"

char *strip(char **word) {
	char *start = *word;
	int len = strlen(start);
	char *end = start + len - 1;

	while ((start < end) && (!isalpha(start[0])))
		start++;

	while ((end > start) && (!isalpha(end[0])))
		end--;

	if (start > end)
		return NULL;

	end[1] = '\0';
	*word = start;

	return start;
}

/* Given a binary search tree (sorted alphabetically by the word at
 each node), and a new word, inserts the word at the appropriate
 place in the tree.  */

void insert(tree_node *root, char *word) {
	if (root == NULL) {
		root = (tree_node *) malloc(sizeof(tree_node));
		root->word = word;
		root->left = NULL;
		root->right = NULL;
		return;
	}

	int compare_value = strcmp(word, root->word);

	if (compare_value == 0)
		return;

	if (compare_value < 0) {
		if (root->left != NULL)
			insert(root->left, word);
		else {
			tree_node *new_node = (tree_node *) malloc(sizeof(tree_node));
			new_node->word = strdup(word);
			new_node->left = NULL;
			new_node->right = NULL;
			root->left = new_node;
		}
	} else {
		if (root->right != NULL)
			insert(root->right, word);
		else {
			tree_node *new_node = (tree_node *) malloc(sizeof(tree_node));
			new_node->word = strdup(word);
			new_node->left = NULL;
			new_node->right = NULL;
			root->right = new_node;
		}
	}
}

/* Given a binary search tree and a word, search for the word
 in the binary search tree.  */

int find_word(tree_node *dictionary, char *word) {
	if (!word || !dictionary)
		return 0;

	int compare_value = strcmp(word, dictionary->word);

	if (compare_value == 0)
		return 1;
	else if (compare_value < 0)
		return find_word(dictionary->left, word);
	else
		return find_word(dictionary->right, word);
}

/* Print out the words in the binary search tree, in sorted order.  */

void print_tree(tree_node *dictionary) {
	if (!dictionary)
		return;

	if (dictionary->left)
		print_tree(dictionary->left);

	printf("%s,", dictionary->word);

	if (dictionary->right)
		print_tree(dictionary->right);
}

