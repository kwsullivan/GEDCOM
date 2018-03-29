#include "GEDCOMutilities.h"
//************************************************************************************************************
char* eventTypes[] = {  "ADOP", "BIRT", "BAPM", "BARM", "BASM", "BLES", "BURI", "CENS",
                        "CHR", "CHRA", "CONF", "CREM", "DEAT", "EMIG", "FCOM", "GRAD",
                        "IMMI", "NATU", "ORDN", "RETI", "PROB", "WILL", "EVEN"};

char* famTypes[] = {   "ANUL", "CENS", "DIV", "DIVF", "ENGA",
                        "MARR", "MARB", "MARC", "MARL", "MARS", "EVEN"};

void testLine(int type) {
    printf("GETS TO =================================> %d\n", type);
}

GEDCOMinput* createGEDCOMinput(int line, int level, char* tag, char* XREF_ID, char* value) {

    GEDCOMinput * input = malloc(sizeof(GEDCOMinput)+1);

    input->line = line;
    input->level = level;

    input->tag = malloc(sizeof(char)*strlen(tag)+1);
    strcpy(input->tag, tag);
    input->tag[strlen(input->tag)] = '\0';

    input->XREF_ID = malloc(sizeof(char)*strlen(XREF_ID)+1);
    strcpy(input->XREF_ID, XREF_ID);
    input->XREF_ID[strlen(input->XREF_ID)] = '\0';

    input->value = malloc(sizeof(char)*strlen(value)+1);
    strcpy(input->value, value);
    input->value[strlen(input->value)] = '\0';
    return input;
}

char* getLine(GEDCOMinput * l) {
    printf("%d|L:%d\nXREF:'%s'\nTAG:%s\nVALUE:%s",l->line,l->level,l->XREF_ID, l->tag, l->value);
    return "\n";
}

char* printLine(void* toBePrinted) {

    return getLine(toBePrinted);
}

void deleteLine(void* toBeDeleted) {
    GEDCOMinput* del = (GEDCOMinput*)toBeDeleted;
        free(del);
}

int compareLines(const void* first, const void* second) {
    GEDCOMinput* f = (GEDCOMinput*)first;
    GEDCOMinput* s = (GEDCOMinput*)second;
    return(strcmp(f->tag, s->tag));
}
//************************************************************************************************************

char* printWPerson(void* toBePrinted) {
    char* output = malloc(sizeof(char) * 50);
    writePerson* print = (writePerson*)toBePrinted;
    sprintf(output, "%s %s\n\n", print->person->givenName, print->indiTag);
    printf("INDI: %s\nS COUNT: %d\n", print->indiTag, print->spouseCount);
    if(print->childTag != NULL) {
        printf("CHIL: %s\n", print->childTag);
    }
    if(print->spouseTag != NULL) {
        for(int i=0;i<print->spouseCount;i++) {
            printf("SPOUSE TAG %s\n", print->spouseTag[i]);
        }
    }

    return output;
}
void deleteWPerson(void* toBeDeleted) {

}
int compareWPeople(const void* first, const void* second) {
    return 0;
}
void printList(List list) {
    Node* print = list.head;
    while(print != NULL) {
        printf("%s", list.printData(print->data));
        print = print->next;
    }
}
bool fileCheckError(char* fileName, FILE* file) {

    if(file == NULL) {
        return false;
    }
    if(strcmp(fileName, "") == 0) {
        return false;
    }
    if(strstr(fileName, ".ged") != NULL) {
        return true;
    }
    return false;
}

bool levelCheckError(char** GEDCOMpointer, int numLines, GEDCOMerror* err) {
    char* levelParse;
    char* GEDcopy;
    int currLevel = 0;
    int oldLevel = -1;
    for(int i=0;i<numLines;i++) {
        GEDcopy = malloc(sizeof(char)*strlen(GEDCOMpointer[i]));
        strcpy(GEDcopy, GEDCOMpointer[i]);
        levelParse = strtok(GEDcopy, " ");

        if(isdigit(levelParse[0])) {
            currLevel = atoi(&levelParse[0]);
        }
        else {
            err->type = INV_RECORD;
            err->line = i+1;
            return false;
        }

        if(i == 0) {
            if(currLevel != 0) {
                err->type = INV_HEADER;
                err->line = i;
                return false;
            }
        }
        if(currLevel != oldLevel && currLevel > oldLevel+1 && currLevel != 0) {
            err->type = INV_RECORD;
            err->line = i+1;
            return false;
        }
        oldLevel = currLevel;
        free(GEDcopy);
    }
    return true;
}
bool tagCheckError(char** GEDCOMpointer, int numLines, GEDCOMerror* err) {
    char* tag;
    char* GEDcopy;
    int foundHEAD, foundSUBM, foundTRLR;
    foundHEAD = foundSUBM = foundTRLR = 0;

    for(int i=0;i<numLines;i++) {
        GEDcopy = malloc(sizeof(char)*strlen(GEDCOMpointer[i]));
        strcpy(GEDcopy, GEDCOMpointer[i]);
        tag = strtok(GEDcopy, " ");
        tag = strtok(NULL, " ");
        if(tag[0] == '@') {
            tag = strtok(NULL, " ");
        }
        if(strcmp(tag, "HEAD") == 0) {
            foundHEAD++;
        }

        if(strcmp(tag, "TRLR") == 0) {
            foundTRLR++;
        }
    }

    if(foundHEAD != 1 || foundTRLR != 1) {
        err->type = INV_GEDCOM;
        err->line = -1;
        return false;
    }

    return true;
}

GEDCOMobject* createObject() {
    GEDCOMobject* obj = malloc(sizeof(GEDCOMobject)+1);
    obj->families = initializeList(&printFamily, &deleteFamily, &compareFamilies);
    obj->individuals = initializeList(&printIndividual, &deleteIndividual, &compareIndividuals);
    obj->header = NULL;
    obj->submitter = NULL;
    return obj;
}

writePerson* createWPerson(Individual* person, char* indiTag) {
    writePerson* indiv = malloc(sizeof(writePerson)+1);
    indiv->person = person;
    indiv->indiTag = malloc(sizeof(char)*strlen(indiTag)+1);
    strcpy(indiv->indiTag, indiTag);
    indiv->indiTag[strlen(indiv->indiTag)] = '\0';
    indiv->spouseTag = NULL;
    indiv->spouseCount = 0;
    indiv->childTag = NULL;
    return indiv;
}

void addFamWPerson(writePerson* person, char* famTag, tagType type) {

    int count = person->spouseCount;
    if(count == 0) {
        person->spouseTag = malloc(sizeof(char*));
    }
    else {
        person->spouseTag = realloc(person->spouseTag, sizeof(char*)*(count+1));
    }
    switch(type) {
        case HUSB:
            person->spouseTag[count] = malloc(sizeof(char)* strlen(famTag)+1);
            strcpy(person->spouseTag[count], famTag);
            person->spouseTag[count][strlen(person->spouseTag[count])] = '\0';
            person->spouseCount++;
            person->gender = MALE;
            break;
        case WIFE:
            person->spouseTag[count] = malloc(sizeof(char)* strlen(famTag)+1);
            strcpy(person->spouseTag[count], famTag);
            person->spouseTag[count][strlen(person->spouseTag[count])] = '\0';
            person->spouseCount++;
            person->gender = FEMALE;
            break;
        case CHIL:
            person->childTag = malloc(sizeof(char)* strlen(famTag)+1);
            strcpy(person->childTag, famTag);
            person->childTag[strlen(person->childTag)] = '\0';
            break;
        default:
            break;
    }
}

Event* createEvent(char* type, char* date, char* place) {

    Event* event = malloc(sizeof(Event)+1);
    if(date == NULL) {
        event->date = malloc(sizeof(char)*2);
        strcpy(event->date, "");
    }
    else {
        event->date = malloc(sizeof(char)*strlen(date)+1);
        strcpy(event->date, date);
        event->date[strlen(date)] = '\0';
    }
    if(place == NULL) {
        event->place = malloc(sizeof(char)*2);
        strcpy(event->place, "");
    }
    else {
        event->place = malloc(sizeof(char)*strlen(place)+1);
        strcpy(event->place, place);
        event->place[strlen(place)] = '\0';
    }


    strcpy(event->type, type);
    return event;
}
Field* createField(char* tag, char* value) {
    Field* field = malloc(sizeof(Field)+1);
    field->tag = malloc(sizeof(char)*strlen(tag)+1);
    strcpy(field->tag, tag);
    field->tag[strlen(field->tag)] = '\0';
    field->value = malloc(sizeof(char)*strlen(value)+1);
    strcpy(field->value, value);
    field->value[strlen(field->value)] = '\0';
    return field;
}

List* createList(char* (*printFunction)(void* toBePrinted),void (*deleteFunction)(void* toBeDeleted),int (*compareFunction)(const void* first,const void* second)){
    List* tmpList = malloc(sizeof(List)+1);
    
    tmpList->head = NULL;
    tmpList->tail = NULL;
    tmpList->length = 0;
    tmpList->deleteData = deleteFunction;
    tmpList->compare = compareFunction;
    tmpList->printData = printFunction;
    
    return tmpList;
}

Individual* createIndividual(char* firstName, char* lastName, List events, List otherFields) {

    Individual* indiv = malloc(sizeof(Individual)+1);
    indiv->givenName = malloc(sizeof(char)*strlen(firstName)+1);
    indiv->surname = malloc(sizeof(char)*strlen(lastName)+1);

    strcpy(indiv->givenName,firstName);
    indiv->givenName[strlen(indiv->givenName)] = '\0';

    strcpy(indiv->surname,lastName);
    indiv->surname[strlen(indiv->surname)] = '\0';

    indiv->events = events;
    indiv->families = initializeList(&printFamily, &deleteFamily, &compareFamilies);
    indiv->otherFields = otherFields;
    return indiv;
}
Family* createFamily(Individual* wife, Individual* husband, List childList, List events, List otherFields) {
    Family* family = malloc(sizeof(Family)+1);
    family->wife = wife;
    family->husband = husband;
    
    family->children = childList;
    family->events = events;
    family->otherFields = otherFields;
    return family;
}

Family* getFamily(const Individual* person) {
    Node* directFamIter = person->families.head;
    while(directFamIter != NULL) {
        Family * directIndivFam = (Family*)directFamIter->data;
        Individual* husb = (Individual*)directIndivFam->husband;
        Individual* wife = (Individual*)directIndivFam->wife;

        if(compareIndividuals(person, husb) == 0 || compareIndividuals(person, wife) == 0) {
            return directIndivFam;
        }
        directFamIter = directFamIter->next;
    }
    return NULL;
}
Header* parseHeader(Header* header, Node* temp, GEDCOMerror* err) {

    int foundSOUR, foundGEDV, foundSET, foundSUBM;
    foundSOUR = foundGEDV = foundSET = foundSUBM = 0;
    header = malloc(sizeof(Header)+1);
    header->otherFields = initializeList(&printField, &deleteField, &compareFields);
    Field* storeField;
    temp = temp->next;
    GEDCOMinput* parsedData = (GEDCOMinput*)temp->data;

    while(parsedData->level != 0) {
        parsedData = (GEDCOMinput*)temp->data;

        if(parsedData->level == 1) {
            if(strcmp(parsedData->tag, "SOUR") == 0) {
                strcpy(header->source, parsedData->value); //SOUR -----------------
                header->source[strlen(parsedData->value)] = '\0';
                foundSOUR = 1;
                temp = temp->next;
                parsedData = (GEDCOMinput*)temp->data;

                // ALL OTHER EVENTS IN HEADER ---------------
                while(parsedData->level > 1) {

                    storeField = createField(parsedData->tag, parsedData->value);
                    insertBack(&header->otherFields, storeField);
                    
                    temp = temp->next;
                    parsedData = (GEDCOMinput*)temp->data;

                }
                // END OTHER EVENTS IN HEADER ---------------
            }
            else if(strcmp(parsedData->tag, "GEDC") == 0) {
                parsedData = (GEDCOMinput*)temp->next->data;
                while(parsedData->level > 1) {
                    if(strcmp(parsedData->tag, "VERS") == 0) {
                        if(isdigit(*parsedData->value)) {
                            float version = atof(parsedData->value);
                            header->gedcVersion = version; //VERSION ----------
                            foundGEDV = 1;
                        }
                        else {
                            err->line = parsedData->line;
                            return NULL;
                        }
                    }
                    else {
                        storeField = createField(parsedData->tag, parsedData->value);
                        insertBack(&header->otherFields, storeField);
                    }
                    temp = temp->next;
                    parsedData = (GEDCOMinput*)temp->data;
                }
            }
            else if(strcmp(parsedData->tag, "CHAR") == 0) {
                if(strcmp(parsedData->value, "ANSEL") == 0) {
                    header->encoding = ANSEL;
                    
                }
                else if(strcmp(parsedData->value, "UTF-8") == 0) {
                    header->encoding = UTF8;
                    
                }
                else if(strcmp(parsedData->value, "UNICODE") == 0) {
                    header->encoding = UNICODE;
                    
                }
                else if(strcmp(parsedData->value, "ASCII") == 0) {
                    header->encoding = ASCII;
                }
                else {
                    header->encoding = ASCII;
                }
                foundSET = 1;
                temp = temp->next;
            }
            else if(strcmp(parsedData->tag, "SUBM") == 0) {
                storeField = createField(parsedData->tag, parsedData->value);
                insertFront(&header->otherFields, storeField);
                foundSUBM = 1;
                temp = temp->next;
            }
            else {
                if(strcmp(parsedData->value, "") == 0) {
                    err->line = parsedData->line;
                    return NULL;
                }
                storeField = createField(parsedData->tag, parsedData->value);
                insertBack(&header->otherFields, storeField);
                temp = temp->next;
            }
            
        }
        else if(parsedData->level > 1) {
            storeField = createField(parsedData->tag, parsedData->value);
            insertBack(&header->otherFields, storeField);
            temp = temp->next;
        }

    }
    if(!foundSOUR || !foundGEDV || !foundSET || !foundSUBM) {
        err->line = parsedData->line;
        return NULL;
    }
    return header;
}

Individual* parseIndividual(Individual* individual, Node* temp, GEDCOMerror* err) {
    char* givenName = NULL;
    char* surname = NULL;
    char* eventTag = NULL;
    
    List events = initializeList(&printEvent, &deleteEvent, &compareEvents);
    List otherFields = initializeList(&printField, &deleteField, &compareFields);
    int foundDate, foundPlace;
    foundDate = foundPlace = 0;
    int foundMatch = 0;
    GEDCOMinput* parsedData = (GEDCOMinput*)temp->data;

    if(strcmp(parsedData->tag, "INDI") == 0) {
        Field * storeField = createField(parsedData->tag, parsedData->XREF_ID);
        insertFront(&otherFields, storeField);
        temp = temp->next;
    }

    parsedData = (GEDCOMinput*)temp->data;
    while(parsedData->level != 0) {
        if(strcmp(parsedData->tag, "NAME") == 0) {
            if(strcmp(parsedData->value, "")!= 0) {
                givenName = strtok(parsedData->value, "/");
                surname = strtok(NULL, "/");

            }
            else {
                givenName = malloc(sizeof(char)+1);
                surname = malloc(sizeof(char)+1);
                strcpy(givenName, "");
                strcpy(surname, "");
            }
            if(givenName == NULL) {
                givenName = malloc(sizeof(char)+1);
                strcpy(givenName, "");
            }
            if(surname == NULL) {
                surname = malloc(sizeof(char)+1);
                strcpy(surname, "");
            }
            givenName[strlen(givenName)-1] = '\0';
            surname[strlen(surname)] = '\0';
        }
        else {
        
            for(int i=0;i<numINDIEvents;i++) {
                if(strcmp(eventTypes[i], parsedData->tag) == 0) {
                    eventTag = malloc(sizeof(parsedData->tag+1));
                    foundMatch = 1;
                    strcpy(eventTag, parsedData->tag);
                    //eventTag[strlen(eventTag)] = '\0';
                    break;
                }
                else {
                    eventTag = malloc(sizeof(char)*2);
                    strcpy(eventTag, "");
                    //eventTag[strlen(eventTag)] = '\0';
                }
            }
        }
        if(foundMatch == 1) {
            temp = temp->next;
            parsedData = (GEDCOMinput*)temp->data;
            char* date = NULL;
            char* place = NULL;
            while(parsedData->level > 1) {
                if(strcmp(parsedData->tag, "DATE") == 0) {
                    date = malloc(sizeof(parsedData->value)+1);
                    strcpy(date, parsedData->value);
                    date[strlen(date)] = '\0';
                    foundDate = 1;
                }
                else if(strcmp(parsedData->tag, "PLAC") == 0) {
                    place = malloc(sizeof(parsedData->value)+1);
                    strcpy(place, parsedData->value);
                    place[strlen(place)] = '\0';
                    foundPlace = 1;
                }
                temp = temp->next;
                parsedData = (GEDCOMinput*)temp->data;
            }
            if(foundDate == 0) {
                date = malloc(sizeof(char)*2);
                strcpy(date, "");
                //date[1] = '\0';
            }
            if(foundPlace == 0) {
                place = malloc(sizeof(char)*2);
                strcpy(place, "");
                //place[1] = '\0';
            }

            Event * indiEvent = createEvent(eventTag, date, place);
            insertBack(&events, indiEvent);
            foundMatch = 0;
            //strcpy(eventTag, "");
            /*
            if(place != NULL) {
                free(place);
                place = NULL;
            }
            if(date != NULL) {
                free(date);
                date = NULL;
            }
            if(eventTag != NULL) {
                free(eventTag);
                eventTag = NULL;
            }
            */
        }
        else {
            Field * storeField = createField(parsedData->tag, parsedData->value);
            insertBack(&otherFields, storeField);
            temp = temp->next;
            parsedData = (GEDCOMinput*)temp->data;
        }
        
    }
    individual = createIndividual(givenName, surname, events, otherFields);
    return individual;
}

Family* parseFamily(Family* family, Node* temp, GEDCOMobject** obj, GEDCOMerror* err) {
    
    temp = temp->next;
    GEDCOMinput* parsedData = (GEDCOMinput*)temp->data;
    char* famXref = malloc(sizeof(parsedData->XREF_ID)+1);
    strcpy(famXref, parsedData->XREF_ID);
    Individual* husband = NULL;
    Individual* wife = NULL;
    List childList = initializeList(&printIndividual, &deleteIndividual, &compareIndividuals);
    List famEvents = initializeList(&printEvent, &deleteEvent, &compareEvents);
    List otherFields = initializeList(&printField, &deleteField, &compareFields);
    char* eventTag = malloc(sizeof(char)*5);
    int foundDate, foundPlace;
    foundDate = foundPlace = 0;
    char* date = NULL;
    char* place = NULL;

    while(parsedData->level != 0) {
        Node* indivIter = (*obj)->individuals.head;
        while(indivIter != NULL) {
            Individual * indiv = (Individual*)indivIter->data;

            Node * otherIter = indiv->otherFields.head;
            if(otherIter == NULL) {
                err->type = INV_GEDCOM;
                err->line = parsedData->line;
            }
            Field * indivXref = (Field*)getFromFront(indiv->otherFields);
            if(strcmp(parsedData->value, indivXref->value) == 0) {
                if(strcmp(parsedData->tag, "HUSB") == 0) {
                    husband = indiv;
                    break;
                }
                else if(strcmp(parsedData->tag, "WIFE") == 0) {
                    wife = indiv;
                    break;
                }
                else if(strcmp(parsedData->tag, "CHIL") == 0) {
                    insertBack(&childList, indiv);
                    break;
                }
                temp = temp->next;
                parsedData = (GEDCOMinput*)temp->data;
            }
            indivIter = indivIter->next;
        }
        if(strcmp(parsedData->tag, "HUSB")!= 0 && strcmp(parsedData->tag, "WIFE")!= 0 && strcmp(parsedData->tag, "CHIL")!= 0) {
            for(int i=0;i<numFAMEvents;i++) {
                if(strcmp(famTypes[i], parsedData->tag) == 0) {
                    strcpy(eventTag, parsedData->tag);
                    break;
                }
                else {
                    strcpy(eventTag, "");
                }
            }
            if(strcmp(eventTag, "") != 0) {
                temp = temp->next;
                parsedData = (GEDCOMinput*)temp->data;
                while(parsedData->level > 1) {
                    if(strcmp(parsedData->tag, "DATE") == 0) {
                        date = malloc(sizeof(parsedData->value)+1);
                        strcpy(date, parsedData->value);
                        foundDate = 1;
                    }
                    else if(strcmp(parsedData->tag, "PLAC") == 0) {
                        place = malloc(sizeof(parsedData->value)+1);
                        strcpy(place, parsedData->value);
                        foundPlace = 1;
                    }
                    temp = temp->next;
                    parsedData = (GEDCOMinput*)temp->data;

                    /* This is necessary to iterate temp to previous
                     * to ensure there is no double call to temp = temp->next
                     */
                    if(parsedData->level < 2) {
                        temp = temp->previous;
                    }
                }

                if(!foundDate) {
                    date = NULL;
                }
                if(!foundPlace) {
                    place = NULL;
                }
                Event * famEventStruct = createEvent(eventTag, date, place);
                insertBack(&famEvents, famEventStruct);
                strcpy(eventTag, "");
            }
            else {
                Field * insertField = createField(parsedData->tag, parsedData->value);
                insertBack(&otherFields, insertField);
            }
        }
        temp = temp->next;
        parsedData = (GEDCOMinput*)temp->data;
    }

    family = createFamily(wife, husband, childList, famEvents, otherFields);

    if(family->wife != NULL) {
        insertBack(&family->wife->families, family);
    }
    
    if(family->husband != NULL) {
        insertBack(&family->husband->families, family);
    }

    Node* childIter = childList.head;
    while(childIter != NULL) {
        Individual * child = (Individual*)childIter->data;
        insertBack(&child->families, family);
        childIter = childIter->next;
    }
    return family;
}

Submitter* parseSubmitter(Header* header, Submitter* submitter, Node* temp, GEDCOMerror* err) {
    Field* subIDField = (Field*)header->otherFields.head->data;
    GEDCOMinput* parsedData = (GEDCOMinput*)temp->data;
    submitter = malloc(sizeof(Submitter)+sizeof(char)*256);
    submitter->otherFields = initializeList(&printField, &deleteField, &compareFields);
    int foundName, foundAddr;
    foundName = foundAddr = 0;
    if(strcmp(subIDField->value, parsedData->XREF_ID) == 0) {

        temp = temp->next;
        parsedData = (GEDCOMinput*)temp->data;

        while(parsedData->level > 0) {
            if(temp == NULL) {
                return NULL;
            }
            if(strcmp(parsedData->tag, "NAME") == 0) {
                strcpy(submitter->submitterName, parsedData->value);
                foundName = 1;
            }
            else if(strcmp(parsedData->tag, "ADDR") == 0) {
                strcpy(submitter->address, parsedData->value);
                foundAddr = 1;
            }
            else {
                if(strcmp(parsedData->value, "") == 0) {
                    err->line = parsedData->line;
                    return NULL;
                }
                else {
                    Field * insertField = createField(parsedData->tag, parsedData->value);
                    insertBack(&submitter->otherFields, insertField);
                }
            }
            temp = temp->next;
            if(temp == NULL) {
                err->line = parsedData->line;
                return NULL;
            }
            parsedData = (GEDCOMinput*)temp->data;
        }
    }
    else {
        err->line = parsedData->line;
        return NULL;
    }
    if(!foundName) {
        err->line = parsedData->line;
        return NULL;
    }
    if(!foundAddr) {
        strcpy(submitter->address, "");
    }

    return submitter;
}
void clearListNode(List* list){
    
    if (list == NULL){
        return;
    }
    
    if (list->head == NULL && list->tail == NULL){
        return;
    }
    
    Node* tmp = NULL;
    
    while (list->head != NULL){
        tmp = list->head;
        list->head = list->head->next;
        if(tmp != NULL) {
            free(tmp);
        }
    }
    
    list->head = NULL;
    list->tail = NULL;
}

bool findIndividual(const void* first,const void* second) {
    if(first == NULL || second == NULL) {
        return false;
    }
    bool result;
    Individual* f = (Individual*)first;
    Individual* s = (Individual*)second;
    char* p1 = malloc(sizeof(char)*(strlen(f->givenName)+strlen(f->surname)+1));
    char* p2 = malloc(sizeof(char)*(strlen(s->givenName)+strlen(s->surname)+1));
    strcat(p1, f->givenName);
    strcat(p1, f->surname);
    strcat(p2, s->givenName);
    strcat(p2, s->surname);
    if(strcmp(p1, p2) == 0) {
        result = true;
    }
    else {
        result = false;
    }
    //free(p1);
    //free(p2);

    return result;
}

void writeLine(int level, char* tag, char* value, FILE* fptr) {
    char line[300];
    if(value == NULL) {
        sprintf(line, "%d %s\n", level, tag);
        fwrite(line, strlen(line), 1, fptr);
    }
    else if(strcmp(value, "") == 0) {
        sprintf(line, "%d %s\n", level, tag);
        fwrite(line, strlen(line), 1, fptr);
    }
    else {
        sprintf(line, "%d %s %s\n", level, tag, value);
        fwrite(line, strlen(line), 1, fptr);
    }
}

void writeHeader(char* submTag, const GEDCOMobject* obj, FILE* fptr) {

    writeLine(0, "HEAD", NULL, fptr);
    writeLine(1, "SOUR", obj->header->source, fptr);

    Node* other = obj->header->otherFields.head;
    while(other != NULL) {
        Field* field = (Field*)other->data;
        if(strcmp(field->tag, "NAME") == 0) {
            writeLine(2, field->tag, field->value, fptr);
        }
        else if(strcmp(field->tag, "VERS") == 0) {
            writeLine(2, field->tag, field->value, fptr);
        }
        else if(strcmp(field->tag, "DATE") == 0) {
            writeLine(1, field->tag, field->value, fptr);
        }
        other = other->next;
    }
/**************
    other = obj->header->otherFields.head;
    while(other != NULL) {
        Field* field = (Field*)other->data;
        if(strcmp(field->tag, "NAME") != 0 && (strcmp(field->tag, "VERS") != 0) && (strcmp(field->tag, "FORM") != 0)) {
            writeLine(1, field->tag, field->value, fptr);
        }
        other = other->next;
    }
***************/
    writeLine(1, "GEDC", NULL, fptr);

    char line[300];
    sprintf(line, "%d %s %.1f\n", 2, "VERS", obj->header->gedcVersion);
    fwrite(line, strlen(line), 1, fptr);

    writeLine(2, "FORM", "LINEAGE-LINKED", fptr);
    char encoding[10];
    switch(obj->header->encoding) {
        case ANSEL:
            strcpy(encoding, "ANSEL");
            break;
        case UTF8:
        strcpy(encoding, "UTF-8");
            break;
        case UNICODE:
        strcpy(encoding, "UNICODE");
            break;
        case ASCII:
        strcpy(encoding, "ASCII");
            break;

    }

    writeLine(1, "CHAR", encoding, fptr);
    writeLine(1, "SUBM", submTag, fptr);
}

void writeSubmitter(const GEDCOMobject* obj,char* submTag, FILE* fptr) {

    writeLine(0, submTag, "SUBM", fptr);
    writeLine(1, "NAME", obj->submitter->submitterName, fptr);

        if(strcmp(obj->submitter->address, "")!= 0) {
            writeLine(1, "ADDR", obj->submitter->address, fptr);
        }
    

}

void writeIndividuals(List taggedIndivs, FILE* fptr) {

    Node* indivIter = taggedIndivs.head;
    while(indivIter != NULL) {
        writePerson* current = (writePerson*)indivIter->data;
        writeLine(0, current->indiTag, "INDI", fptr);
        char fullName[100];

        if(strcmp(current->person->surname, "") == 0) {
            sprintf(fullName, "%s //", current->person->givenName);
        }
        else {
            sprintf(fullName, "%s /%s/", current->person->givenName, current->person->surname);
        }
        
        fullName[strlen(fullName)] = '\0';

        writeLine(1, "NAME", fullName, fptr);
        /*********
        if(strcmp(current->person->givenName, "") != 0) {
            writeLine(2, "GIVN", current->person->givenName, fptr);
        }
        if(strcmp(current->person->surname, "") != 0) {
            writeLine(2, "SURN", current->person->surname, fptr);
        }
        ************/
        Node* other = current->person->otherFields.head;
        while(other != NULL) {
            Field* field = (Field*)other->data;
            if(strcmp(field->tag, "SEX") == 0) {
                writeLine(1, field->tag, field->value, fptr);
                break;
            }
            other = other->next;
        }

        Node* eventIter = current->person->events.head;
        while(eventIter != NULL) {
            Event* event = (Event*)eventIter->data;
            writeLine(1, event->type, "", fptr);
            if(strcmp(event->date, "")!= 0) {
                writeLine(2, "DATE", event->date, fptr);
            }
            if(strcmp(event->place, "")!= 0) {
                writeLine(2, "PLAC", event->place, fptr);
            }
            eventIter = eventIter->next;
        }
        if(current->childTag != NULL) {
            /*
            printf("writing child tag for\n");
            printIndividual(current->person);
            */
            writeLine(1, "FAMC", current->childTag, fptr);
        }
        if(current->spouseTag != NULL) {
            /*
            printf("writing spouse tag for\n");
            printIndividual(current->person);
            */
            for(int i=0;i<current->spouseCount;i++) {
                writeLine(1, "FAMS", current->spouseTag[i], fptr);
            }
        }
    
        indivIter = indivIter->next;
    }
}

void writeFamilies(List taggedIndivs,const GEDCOMobject* obj, int numFams, FILE* fptr) {
    char** famIDS = malloc(sizeof(char*)*numFams);
    for(int i=0;i<numFams;i++) {
        famIDS[i] = malloc(sizeof(char)*8);
        sprintf(famIDS[i], "@F%04d@", i+1);
    }
    
    Node* famIter = obj->families.head;
    for(int i=0;i<numFams;i++) {
        writeLine(0, famIDS[i], "FAM", fptr);
        Node* indivIter = taggedIndivs.head;
        while(indivIter != NULL) {
            writePerson* current = (writePerson*)indivIter->data;
            for(int j=0;j<current->spouseCount;j++) {
                if(strcmp(current->spouseTag[j], famIDS[i]) == 0) {
                    if(current->gender == MALE) {
                        writeLine(1, "HUSB", current->indiTag, fptr);
                    }
                }
            }
            
            indivIter = indivIter->next;
        }

        indivIter = taggedIndivs.head;
        while(indivIter != NULL) {
            writePerson* current = (writePerson*)indivIter->data;

            for(int j=0;j<current->spouseCount;j++) {
                if(strcmp(current->spouseTag[j], famIDS[i]) == 0) {
                        if(current->gender == FEMALE) {
                        writeLine(1, "WIFE", current->indiTag, fptr);
                    }
                }
            }
            
            indivIter = indivIter->next;
        }

        Family* currFam = (Family*)famIter->data;
        Node* currEvent = currFam->events.head;
        while(currEvent != NULL) {
            Event* event = (Event*)currEvent->data;
            writeLine(1, event->type, "", fptr);
            if(strcmp(event->date, "")!= 0) {
                writeLine(2, "DATE", event->date, fptr);
            }
            if(strcmp(event->place, "")!= 0) {
                writeLine(2, "PLAC", event->place, fptr);
            }
            currEvent = currEvent->next;
        }

        indivIter = taggedIndivs.head;
        while(indivIter != NULL) {
            writePerson* current = (writePerson*)indivIter->data;
            if(current->childTag != NULL) {
                if(strcmp(current->childTag, famIDS[i]) == 0) {
                    writeLine(1, "CHIL", current->indiTag, fptr);
                }
            }

            indivIter = indivIter->next;
        }
        famIter = famIter->next;
    }
}

void populateDescendants(List* list, const Individual* person) {
    Node* famIter = person->families.head;

    if(famIter == NULL) {
        printf("no fam\n");
        return;
    }
    Family* checkChildren = (Family*)famIter->data;
    Node* childIter = checkChildren->children.head;
    if(childIter == NULL) {
        return;
    }
    else {
        while(famIter != NULL) {
            Family* family = (Family*)famIter->data;
            Individual* husb = (Individual*)family->husband;
            Individual* wife = (Individual*)family->wife;

            if((husb == person) || (wife == person)) {
                Node* childIter = family->children.head;
                    while(childIter != NULL) {
                        Individual* getChild = (Individual*)childIter->data;
                        List events = initializeList(&printEvent, &deleteEvent, &compareEvents);
                        List otherFields = initializeList(&printField, &deleteField, &compareFields);
                        Individual* child = createIndividual(getChild->givenName, getChild->surname, events, otherFields);
                        insertBack(list, child);
                        populateDescendants(list, getChild);
                        childIter = childIter->next;
                    }
            }
            famIter = famIter->next;
        }
    }
}
