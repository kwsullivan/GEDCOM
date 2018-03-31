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
	char* errMsg = NULL;
	GEDCOMerror err = createGEDCOM(argv[1], &gedObj);
	
	if(err.type == OK) {
		printf(GRN);
		errMsg = printError(err);
		printf("%s", errMsg);
	}
	else {
		printf(RED);
		errMsg = printError(err);
		printf("%s", errMsg);
	}
	printf(RESET);

	char* ITJ = indivsToJSON(argv[1]);
	printf("%s\n", ITJ);

	char* FTJ = fileToJSON("../", argv[1]);
	printf("%s\n", FTJ);

	//err = writeGEDCOM("test.ged", gedObj);

	//**************************************************
	//**************************************************
/*
	Individual* person = NULL;
	Node* indivIter = gedObj->individuals.head;
	while(indivIter != NULL) {
		person = (Individual*)indivIter->data;
		if(strcmp(person->givenName, "William") == 0) {
			break;
		}
		indivIter = indivIter->next;
	}

	
	char* moreIndivs = gListToJSON(desc);
	printf("%s\n", moreIndivs);

	char* string = indToJSON(person);
	printf("%s\n", string);
	Individual * test = JSONtoInd("{\"givenName\":\"William\",\"surname\":\"Shakespeare\"}");
	printIndividual(test);
	char* fToJSON = fileToJSON("./", argv[1]);
	printf("%s\n", fToJSON);
*/
	if(err.type == OK) {
		//printf("%s", printGEDCOM(gedObj));
		deleteGEDCOM(gedObj);
	}
	return 0;
}