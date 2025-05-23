#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "document.h"

#define DATASET_SIZE 12 + 1

int main() {
  DocumentList *documents = (DocumentList *)malloc(sizeof(DocumentList));
  for (int i = 0; i < DATASET_SIZE; i++) {
    char document_name[100];
    sprintf(document_name, "../datasets/wikipedia12/%d.txt", i);
    Document *document = document_deserialize(document_name);
    if(documents->head == NULL) {
      documents->head = document;
      documents->tail = document;
    } else {
      documents->tail->next = document;
      document->prev = documents->tail;
      documents->tail = document;
    }
  }
  printf("Dataset loaded\n");
  int document_id = -1;
  printf("Enter the document ID to print: ");
  scanf("%d", &document_id);
  Document *curr_document = documents->head;
  while (true) {
    if (curr_document->id == document_id) break;
    curr_document = curr_document->next;
    assert(curr_document != NULL);
  }
  print_document(curr_document);
}