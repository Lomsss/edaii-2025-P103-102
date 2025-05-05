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