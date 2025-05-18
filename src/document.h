#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

typedef struct WordNode {
    char *word;
    struct WordNode *next;
    struct WordNode *prev;
} WordNode;

typedef struct {
    WordNode *head;
    WordNode *tail;
} WordList;

typedef struct LinkNode {
    int id;
    struct LinkNode *next;
    struct LinkNode *prev;
} LinkNode;

typedef struct {
    LinkNode *head;
    LinkNode *tail;
} Links;

typedef struct Document {
    int id;
    WordList title;
    WordList body;
    Links links;
    struct Document *next;
    struct Document *prev;
} Document;

typedef struct {
    Document *head;
    Document *tail;
} DocumentList;

WordList WordListInit();
void WordListAppend(WordList *list, const char *word);
void WordListFree(WordList *list);
void WordListPrint(WordList *list);

Links LinksInit();
void LinksAppend(Links *links, int id);
void LinksFree(Links *links);
void LinksPrint(Links *links);

Document *DocumentInit();
void DocumentFree(Document *doc);
void DocumentPrint(Document *doc);

DocumentList DocumentListInit();
void DocumentListAppend(DocumentList *list, Document *doc);
void DocumentListFree(DocumentList *list);
void DocumentListPrint(DocumentList *list);

Document *document_desserialize(char *path);
DocumentList load_all_documents(const char *folder_name, int dataset_size);

// Lab3

// HashMap
typedef struct {
    char *key;
    void *value;
} Entry;  // Entry in hashmap

typedef struct Node{
    Entry *node;
    struct Node *next;
}Node; // Every bucket has more Nodes in case of colision at the same point 

typedef struct {
    Node *head;
}LinkedList; // A list made for collisions 

typedef struct {
    int size;   // Nr of buckets                
    int count;   // Nr of elements              
    LinkedList **buckets;     // array of pointers to the lists
} HashMap;

int hash_function(const char *key, int table_size);
HashMap *hashmap_init(int size);
void hashmap_set(HashMap *hash, const char *key, void *value);
void *hashmap_get(HashMap *hash, const char *key);
void hashmap_free(HashMap *hash);
void add_doc_id(Links *links, int doc_id);
HashMap *reverse_index(DocumentList *doc_list, int hash_size);
void search_term(HashMap *index, const char *term);

#endif