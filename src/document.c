#include "document.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

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



