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

#endif