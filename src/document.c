#include "document.h"

WordList WordListInit() {
    WordList list;
    list.head = NULL;
    list.tail = NULL;
    return list;
}

void WordListAppend(WordList *list, const char *word) {
    WordNode *node = (WordNode *)malloc(sizeof(WordNode));
    node->word = strdup(word);
    node->next = NULL;
    node->prev = list->tail;

    if (list->tail) {
        list->tail->next = node;
    } else {
        list->head = node;
    }
    list->tail = node;
}

void WordListFree(WordList *list) {
    WordNode *current = list->head;
    while (current) {
        WordNode *next = current->next;
        free(current->word);
        free(current);
        current = next;
    }
    list->head = NULL;
    list->tail = NULL;
}

void WordListPrint(WordList *list) {
    WordNode *current = list->head;
    while (current) {
        printf("%s ", current->word);
        current = current->next;
    }
    printf("\n");
}

Links LinksInit() {
    Links links;
    links.head = NULL;
    links.tail = NULL;
    return links;
}

void LinksAppend(Links *links, int id) {
    LinkNode *node = (LinkNode *)malloc(sizeof(LinkNode));
    node->id = id;
    node->next = NULL;
    node->prev = links->tail;

    if (links->tail) {
        links->tail->next = node;
    } else {
        links->head = node;
    }
    links->tail = node;
}

void LinksFree(Links *links) {
    LinkNode *current = links->head;
    while (current) {
        LinkNode *next = current->next;
        free(current);
        current = next;
    }
    links->head = NULL;
    links->tail = NULL;
}

void LinksPrint(Links *links) {
    LinkNode *current = links->head;
    printf("[");
    while (current) {
        printf("%d", current->id);
        if (current->next) printf(", ");
        current = current->next;
    }
    printf("]\n");
}

Document *DocumentInit() {
    Document *doc = (Document *)malloc(sizeof(Document));
    doc->id = 0;
    doc->title = WordListInit();
    doc->body = WordListInit();
    doc->links = LinksInit();
    doc->next = NULL;
    doc->prev = NULL;
    return doc;
}

void DocumentFree(Document *doc) {
    if (!doc) return;
    WordListFree(&doc->title);
    WordListFree(&doc->body);
    LinksFree(&doc->links);
    free(doc);
}

void DocumentPrint(Document *doc) {
    printf("Document ID: %d\n", doc->id);
    printf("Title: ");
    WordListPrint(&doc->title);
    printf("Body: ");
    WordListPrint(&doc->body);
    printf("Links: ");
    LinksPrint(&doc->links);
    printf("\n");
}

DocumentList DocumentListInit() {
    DocumentList list;
    list.head = NULL;
    list.tail = NULL;
    return list;
}

void DocumentListAppend(DocumentList *list, Document *doc) {
    if (!doc) return;
    
    doc->next = NULL;
    doc->prev = list->tail;

    if (list->tail) {
        list->tail->next = doc;
    } else {
        list->head = doc;
    }
    list->tail = doc;
}

void DocumentListFree(DocumentList *list) {
    Document *current = list->head;
    while (current) {
        Document *next = current->next;
        DocumentFree(current);
        current = next;
    }
    list->head = NULL;
    list->tail = NULL;
}

void DocumentListPrint(DocumentList *list) {
    Document *current = list->head;
    while (current) {
        DocumentPrint(current);
        current = current->next;
    }
}

Document *document_desserialize(char *path) {
    FILE *f = fopen(path, "r");
    assert(f != NULL);
    
    Document *document = DocumentInit();
    
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
    bufferIdx = 0;
    while ((ch = fgetc(f)) != '\n' && ch != EOF) {
        if (ch == ' ') {
            buffer[bufferIdx] = '\0';
            if (bufferIdx > 0) {
                WordListAppend(&document->title, buffer);
            }
            bufferIdx = 0;
        } else {
            assert(bufferIdx < bufferSize);
            buffer[bufferIdx++] = ch;
        }
    }
    // Add last word
    if (bufferIdx > 0) {
        buffer[bufferIdx] = '\0';
        WordListAppend(&document->title, buffer);
    }

    // parse body and links
    char linkBuffer[64];
    int linkBufferSize = 64;
    int linkBufferIdx = 0;
    bool parsingLink = false;
    bool inWord = false;
    
    bufferIdx = 0;
    while ((ch = fgetc(f)) != EOF) {
        if (parsingLink) {
            if (ch == ')') { // end of link
                parsingLink = false;
                assert(linkBufferIdx < linkBufferSize);
                linkBuffer[linkBufferIdx++] = '\0';
                int linkId = atoi(linkBuffer);
                LinksAppend(&document->links, linkId);
                linkBufferIdx = 0;
            } else if (ch != '(') { // skip first parenthesis of the link
                assert(linkBufferIdx < linkBufferSize);
                linkBuffer[linkBufferIdx++] = ch;
            } 
        } else if (ch == ']') { // found beginning of link id, e.g.: [my link text](123)
            parsingLink = true;
        } else if (ch == ' ' || ch == '\n') {
            if (inWord) {
                buffer[bufferIdx] = '\0';
                if (bufferIdx > 0) {
                    WordListAppend(&document->body, buffer);
                }
                bufferIdx = 0;
                inWord = false;
            }
            if (ch == '\n') {
                WordListAppend(&document->body, "\n");
            }
        } else {
            assert(bufferIdx < bufferSize);
            buffer[bufferIdx++] = ch;
            inWord = true;
        }
    }
    // Add last word if any
    if (inWord && bufferIdx > 0) {
        buffer[bufferIdx] = '\0';
        WordListAppend(&document->body, buffer);
    }

    fclose(f);
    return document;
}

DocumentList load_all_documents(const char *folder_name, int dataset_size) {
    DocumentList list = DocumentListInit();
   for (int i = 0; i <= dataset_size; i++) {
    char document_name[64]; 
    sprintf(document_name, "../datasets/%s/%d.txt", folder_name, i);
    Document *doc = document_desserialize(document_name);
    DocumentListAppend(&list, doc);
  }
  return list;
}

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
    LinkNode *current = links->head;
    while (current) {
        if (current->id == doc_id) 
        return; 
        current = current->next;
    }
    LinksAppend(links, doc_id);
}

// Maps each word to an index 
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