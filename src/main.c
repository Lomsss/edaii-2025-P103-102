#include <stdio.h>
#include "document.h"

#define DATASET_SIZE 12

int main() {
  load_all_documents("wikipedia12", DATASET_SIZE);
  printf("What document do you want to print? (ID range 0-%d): ", DATASET_SIZE);
  return 0;
}