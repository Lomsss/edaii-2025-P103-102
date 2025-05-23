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