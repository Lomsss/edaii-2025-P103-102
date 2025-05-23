#ifndef DOCUMENT_H
#define DOCUMENT_H

typedef struct LinkNode {
    int id;
    struct LinkNode *next;
    struct LinkNode *prev;
} Link;

typedef struct {
    int size;
    Link *head;
    Link *tail;
} Links;

typedef struct Document {
    int id;
    char *title;
    char *body;
    Links *links;
    float relevance;
    struct Document *next;
    struct Document *prev;
} Document;

typedef struct {
    Document *head;
    Document *tail;
} DocumentList;

Links* LinksInit();
Document* document_deserialize(char* path);
void print_document(Document *document);

#endif