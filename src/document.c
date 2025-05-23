#include "document.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

// Lab 1
Links *LinksInit() {
    Links *links = (Links *)malloc(sizeof(Links));
    links->size = 0;
    links->head = NULL;
    links->tail = NULL;
    return links;
}

Document *document_deserialize(char *path) {
    FILE *f = fopen(path, "r");
    assert(f != NULL);
    
    Document *document = (Document *)malloc(sizeof(Document));
    
    char buffer[262144];
    int bufferSize = 262144;
    int bufferIdx = 0;
    char ch;

    // parse id
    while ((ch = fgetc(f)) != '\n') {
        assert(bufferIdx < bufferSize);
        buffer[bufferIdx++] = ch;
    }
    assert(bufferIdx < bufferSize);
    buffer[bufferIdx++] = '\0';
    document->id = atoi(buffer);

    // parse title
    // TODO
    bufferIdx = 0;
    while((ch = fgetc(f)) != '\n') {
        assert(bufferIdx < bufferSize);
        buffer[bufferIdx++] = ch;
    }
    assert(bufferIdx < bufferSize);
    buffer[bufferIdx++] = '\0';
    document->title = (char *)malloc(sizeof(char) * bufferIdx);
    strcpy(document->title, buffer);

    // parse body
    char linkBuffer[64];
    int linkBufferSize = 64;
    int linkBufferIdx = 0;
    bool parsingLink = false;
    Links *links = LinksInit();
    
    bufferIdx = 0;
    while ((ch = fgetc(f)) != EOF) {
        assert(bufferIdx < bufferSize);
        buffer[bufferIdx++] = ch;
        if (parsingLink) {
            if (ch == ')') { // end of link
                parsingLink = false;
                assert(linkBufferIdx < linkBufferSize);
                linkBuffer[linkBufferIdx++] = '\0';
                int linkId = atoi(linkBuffer);

                // TODO add to links
                Link *link = (Link *)malloc(sizeof(Link));
                link->id = linkId;
                link->next = NULL;
                link->prev = NULL;
                if (links->head == NULL) {
                    links->head = link;
                    links->tail = link;
                } else {
                    links->tail->next = link;
                    link->prev = links->tail;
                    links->tail = link;
                }
                links->size++;
                linkBufferIdx = 0;
            } else if (ch != '(') { // skip first parenthesis of the link
                assert(linkBufferIdx < linkBufferSize);
                linkBuffer[linkBufferIdx++] = ch;
            } 
        } else if (ch == ']') { // found beginning of link id, e.g.: [my link text](123)
          parsingLink = true;
        }
    }
    assert(bufferIdx < bufferSize);
    buffer[bufferIdx++] = '\0';
    
    char *body = (char *)malloc(sizeof(char) * bufferIdx);
    strcpy(body, buffer);

    // TODO
    document->links = links;
    document->body = body;
    document->relevance = 0.0f;
    fclose(f);
    return document;
}

void print_document(Document *document) {
    printf("Document ID: %d\n", document->id);
    printf("Title: %s\n", document->title);
    printf("Body: %s\n", document->body);
    printf("Links: %d\n", document->links->size);
    printf("\n");
}

// Lab 2

// Lab3

// Hashmap function that converts string into index
int hash_function(const char *key, int table_size) {
    int k = 0;
    while (*key) {
        k = k * 31 + *key++; // 31 is a weight constant to avoid collisions
    }
    return k % table_size;
}

// Creates a hashmap of size "size"
HashMap *hashmap_init(int size){
    HashMap *hash = malloc(sizeof(HashMap));
    hash->size = size;
    hash->count = 0;
    hash->buckets = calloc(size,sizeof(LinkedList)); // Allocate size*sizeof(LinkedList)
    return hash;
}

// Add or in case there is of a collision it updates a key in HashMap
void hashmap_set(HashMap *hash, const char *key, void *value) {
    int in = hash_function(key, hash->size); // The index

    if (hash->buckets[in] == NULL) { // It initialize if the slot is free
        hash->buckets[in] = malloc(sizeof(LinkedList));
        hash->buckets[in]->head = NULL;
    }

    Node *insert = hash->buckets[in]->head; 
    while (insert) {  // Check if it already existed 
        if (strcmp(insert->node->key, key) == 0) {
            insert->node->value = value;
            return;
        }
        insert = insert->next;
    }

    Entry *entry = malloc(sizeof(Entry));  // Creates a new entry 
    entry->key = strdup(key);
    entry->value = value;

    Node *new_insert = malloc(sizeof(Node)); // Search the new node 
    new_insert->node = entry;
    new_insert->next = hash->buckets[in]->head;
    hash->buckets[in]->head = new_insert;

    hash->count++; // Counts the entries

    if (!key || !value) {
    printf("⚠️ hashmap_set called with NULL key or value\n");
}
}

// Search the value in Hashmap
void *hashmap_get(HashMap *hash, const char *key) {
    int in = hash_function(key, hash->size);
    LinkedList *bucket = hash->buckets[in];
    if (!bucket) return NULL; // Checks if the value exists

    Node *n = bucket->head;
    while (n) { // It returns the value when found 
        if (n->node && n->node->key && strcmp(n->node->key, key) == 0) 
        {
        return n->node->value;
        }
        n = n->next;
    }
    return NULL;
}

// Frees all hashmap
void hashmap_free(HashMap *hash) {
    for (int i = 0; i < hash->size; i++) {
        LinkedList *bucket = hash->buckets[i];
        if (bucket) {
            Node *n = bucket->head;
            while (n) {
                Node *temp = n;
                free(temp->node->key);
                free(temp->node);
                n = n->next;
                free(temp);
            }
            free(bucket);
        }
    }
    free(hash->buckets);
    free(hash);   
}

// Adds Doc IDs to the links 
void add_doc_id(Links *links, int doc_id) {
    Link *current = links->head;
    while (current) {
        if (current->id == doc_id) 
        return; 
        current = current->next;
    }
    LinksAppend(links, doc_id);
}

// Maps each word to an index 
// TODO: fix according to the new structure
HashMap *reverse_index(DocumentList *doc_list, int hash_size) {
    HashMap *index = hashmap_init(hash_size); // Initialize Hash
    Document *doc = doc_list->head;

    while (doc) {
        WordNode *word = doc->title.head;
        while (word) {
            Links *postings = hashmap_get(index, word->word);
            if (!postings) {
                postings = malloc(sizeof(Links));
                *postings = LinksInit();
            }
            add_doc_id(postings, doc->id);
            hashmap_set(index, word->word, postings);
            word = word->next;
        }

        word = doc->body.head;
        while (word) {
            if (strcmp(word->word, "\n") != 0) {
                Links *postings = hashmap_get(index, word->word);
                if (!postings) {
                    postings = malloc(sizeof(Links));
                    *postings = LinksInit();
                }
                add_doc_id(postings, doc->id);
                hashmap_set(index, word->word, postings);
            }
            word = word->next;
        }

        doc = doc->next;
    }

    return index;
}

// Search for the word in the docs
void search_term(HashMap *index, const char *term) {
    Links *postings = hashmap_get(index, term);
    if (!postings) {
        printf("Term '%s' not found.\n", term);
        return;
    }

    printf("Term '%s' found in documents: ", term);
    LinksPrint(postings);
}
