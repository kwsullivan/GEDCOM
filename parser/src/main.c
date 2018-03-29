#include <stdio.h>
#include "LinkedListAPI.h"
#include "GEDCOMparser.h"
#include "GEDCOMutilities.h"
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define RESET "\x1B[0m"


int main(int argc, char**argv) {
	GEDCOMobject* gedObj;
	printf("FILE %s\n", argv[1]);

	GEDCOMerror err = createGEDCOM(argv[1], &gedObj);
	
	if(err.type == OK) {
		printf(GRN);
		printf("%s", printError(err));
	}
	else {
		printf(RED);
		printf("%s", printError(err));
	}
	printf(RESET);

	err = writeGEDCOM("../write/test.ged", gedObj);

	//**************************************************
	//**************************************************



	Individual* person = NULL;
	Node* indivIter = gedObj->individuals.head;
	while(indivIter != NULL) {
		person = (Individual*)indivIter->data;
		if(strcmp(person->givenName, "William") == 0) {
			break;
		}
		indivIter = indivIter->next;
	}

	List desc = getDescendantListN(gedObj, person, 2);
	
	char* moreIndivs = gListToJSON(desc);
	printf("%s\n", moreIndivs);
	/***********
	char* string = indToJSON(person);
	Individual * test = JSONtoInd("{\"givenName\":\"William\",\"surname\":\"Shakespeare\"}");
    if (strcmp(string, "{\"givenName\":\"William\",\"surname\":\"Shakespeare\"}") == 0){
    	printf("THEY ARE EQUAL\n");
    }
	printIndividual(person);
	List ancestors = getAncestorListN(gedObj, person, 2);
	printf("LIST LENGTH IS %d\n", ancestors.length);
	Node* ansIter = ancestors.head;
	while(ansIter != NULL) {
		List * parents = (List*)ansIter->data;
		printList(*parents);
		ansIter = ansIter->next;
	}

***********/
	//printList(descN);
    /***************
    List events = initializeList(&printEvent, &deleteEvent, &compareEvents);
	List otherFields = initializeList(&printField, &deleteField, &compareFields);
    Individual* person = createIndividual("John", "Shakespeare", events, otherFields);
    List desc = getDescendants(gedObj, person);
    printList(desc);
    ******************/

	if(err.type == OK) {
		//printf("%s", printGEDCOM(gedObj));
		deleteGEDCOM(gedObj);
	}
	return 0;
}