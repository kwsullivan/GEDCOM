#ifndef GEDCOMUTILITIES_H_
#define GEDCOMUTILITIES_H_
#define numINDIEvents 23
#define numFAMEvents 11
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include "LinkedListAPI.h"
#include "GEDCOMparser.h"

#define GRN   "\x1B[32m"
#define RESET "\x1B[0m"

typedef enum tagType {HUSB, WIFE, CHIL}tagType;
typedef enum gender {MALE, FEMALE}gender;
//************************************************************************************************************
typedef struct {
	int line;
	int level;
	char* tag;
	char* XREF_ID; //optional
	char* value; //optional

}GEDCOMinput;
//************************************************************************************************************
typedef struct {
	Individual* person;
	int spouseCount;
	char* indiTag;
	char** spouseTag;
	char* childTag;
	gender gender;
}writePerson;




void testLine(int type);
GEDCOMinput* createGEDCOMinput(int line, int level, char* tag, char* XREF_ID, char* value);
char* getLine(GEDCOMinput * l);
char* printLine(void* toBePrinted);
void deleteLine(void* toBeDeleted);
int compareLines(const void* first, const void* second);

char* printWPerson(void* toBePrinted);
void deleteWPerson(void* toBeDeleted);
int compareWPeople(const void* first, const void* second);

//************************************************************************************************************
char* getTag(char * tag);
//************************************************************************************************************

List fileParser(List* GEDCOMfile, char** linePointer, int totalLines);

GEDCOMobject* createObject();
writePerson* createWPerson(Individual* person, char* indiTag);
void addFamWPerson(writePerson* person, char* famTag, tagType type);
Event* createEvent(char type[5], char date[50], char place[50]);
Field* createField(char* tag, char* value);
List* createList(char* (*printFunction)(void* toBePrinted),void (*deleteFunction)(void* toBeDeleted),int (*compareFunction)(const void* first,const void* second));
Individual* createIndividual(char* firstName, char* lastName, List events, List otherFields);
Family* createFamily(Individual* wife, Individual* husband, List childList, List events, List otherFields);
Family* getFamily(const Individual* person);
void printList(List list);
//****************************Error Handlers******************************************************************
bool fileCheckError(char* fileName, FILE* file);
bool levelCheckError(char** GEDCOMpointer, int numLines, GEDCOMerror* err);
bool tagCheckError(char** GEDCOMpointer, int numLines, GEDCOMerror* err);
Header* parseHeader(Header* header, Node* current, GEDCOMerror* err);
Individual* parseIndividual(Individual* individual, Node* current, GEDCOMerror* err);
Family* parseFamily(Family* family, Node* temp, GEDCOMobject** obj, GEDCOMerror* err);
Submitter* parseSubmitter(Header* header, Submitter* submitter, Node* temp, GEDCOMerror* err);
void clearListNode(List* list);
bool findIndividual(const void* first,const void* second);

void writeLine(int level, char* tag, char* value, FILE* fptr);
void writeHeader(char* submTag, const GEDCOMobject* obj, FILE* fptr);
void writeSubmitter(const GEDCOMobject* obj, char* submTag, FILE* fptr);
void writeIndividuals(List taggedIndivs, FILE* fptr);
void writeFamilies(List taggedIndivs,const GEDCOMobject* obj, int numFams, FILE* fptr);
void populateDescendants(List* list, const Individual* person);
#endif