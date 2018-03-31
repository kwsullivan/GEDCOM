
#include "GEDCOMparser.h"
#include "GEDCOMutilities.h"

#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define RESET "\x1B[0m"

GEDCOMerror createGEDCOM(char* fileName, GEDCOMobject** obj) {

    FILE * fptr = NULL;
    fptr = fopen(fileName, "r");
    GEDCOMerror err;

    err.type = OK;
    err.line = -1;

    if(fileName == NULL) {
        err.type = INV_FILE;
        err.line = -1;
        return err;
    }
    if(!fileCheckError(fileName, fptr)) {
        err.type = INV_FILE;
        err.line = -1;
        return err;
    }

    char* parseLine = NULL;
    int level = 0;;
    char tag[5] = "";
    char* value = NULL;
    char xref[20];
    int oldLevel = -1;
    int currentLevel = 0;
    
    fseek(fptr, 0, SEEK_END);
    int fileLength = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    char* buffer = malloc(sizeof(char)* fileLength);
    char lineRead[255];

    fread(buffer, sizeof(char), fileLength, fptr);

    int numLines = 0;
    int lineLen = 0;
    int count = 0;
    
    //*********************************************************************************
    //************** START OF FILE READER
    //*********************************************************************************

    for(int i=0;i<fileLength;i++) {
        if(buffer[i] == '\n' || buffer[i] == '\r') {
            if((buffer[i+1] == '\n' || buffer[i+1] == '\r') && (i+1 < fileLength)) {
                i++;
            }
        numLines++;
        }
    }
    if(buffer[fileLength-1] != '\n' && buffer[fileLength-1] != '\r') {
        numLines++;
    }

    char** GEDCOMpointer = malloc(sizeof(char*)*numLines);
    
    for(int i=0;i<fileLength;i++) {
        if(lineLen > 253) {
            err.type = INV_RECORD;
            err.line = count+1;
            (*obj) = NULL;
            obj = NULL;
            return err;
        }
        if(buffer[i] == '\n' || buffer[i] == '\r') {
            if(buffer[i+1] == '\n' || buffer[i+1] == '\r') {
                i++;
            }
            lineRead[lineLen] = '\0';
            GEDCOMpointer[count] = malloc(sizeof(char)*lineLen+1);
            strcpy(GEDCOMpointer[count], lineRead);
            count++;
            lineLen = 0;
        }
        else {
            lineRead[lineLen] = buffer[i];
            lineLen++;
        }
    }

    free(buffer);
/*
    if(GEDCOMpointer[count] == NULL) {
        lineRead[lineLen] = '\0';
        GEDCOMpointer[count] = malloc(sizeof(char)*2);
        strcpy(GEDCOMpointer[count], "\0");
        count++;
        lineLen = 0;
    }
*/
    //*********************************************************************************
    //************** END OF FILE READER
    //*********************************************************************************
/*
    if(!levelCheckError(GEDCOMpointer, numLines, &err)) {
        (*obj) = NULL;
        obj = NULL;
        return err;
    }

    if(!tagCheckError(GEDCOMpointer, numLines, &err)) {
        (*obj) = NULL;
        obj = NULL;
        return err;
    }
    */

    //*********************************************************************************
    //************** START OF FILE PARSER
    //*********************************************************************************

    List GEDCOMlist = initializeList(&printLine, &deleteLine, &compareLines);

     for(int i = 0;i<numLines;i++) {
        //LEVEL PARSE
        parseLine = strtok(GEDCOMpointer[i], " ");
        if(parseLine == NULL) {
            err.type = OTHER_ERROR;
            err.line = i+1;
            return err;
        }
        if(!isdigit(parseLine[0])) {
            err.type = INV_RECORD;
            err.line = i+1;
            free(obj);
            (*obj) = NULL;
            obj = NULL;
            return err;
        }


        currentLevel = atoi(parseLine);
        if(currentLevel != oldLevel && currentLevel > oldLevel+1 && currentLevel != 0) {
            err.type = INV_RECORD;
            err.line = i;
            free(obj);
            (*obj) = NULL;
            obj = NULL;
            return err;
        }
        level = currentLevel;
        oldLevel = currentLevel;
        parseLine = strtok(NULL, " ");

        //XREF && TAG PARSE
        if(parseLine[0] == '@') {
            strcpy(xref, parseLine);
            xref[strlen(xref)] = '\0';

            parseLine = strtok(NULL, " ");

            strcpy(tag, parseLine);
            parseLine = strtok(NULL, "");
            value = malloc(sizeof(char)+2);
            strcpy(value, "");
        }
        //TAG && VALUE PARSE
        else {
            strcpy(tag, parseLine);
            parseLine = strtok(NULL, "\n");
            if(parseLine != NULL) {
                value = malloc(sizeof(char)*strlen(parseLine)+1);
                strcpy(value, parseLine);
                value[strlen(value)] = '\0';
                strcpy(xref, "");
                xref[strlen(xref)] = '\0';
            }
            else {
                value = malloc(sizeof(char)+2);
                strcpy(value, "");
            }
        }

        GEDCOMinput * newParse = NULL; 
        newParse = createGEDCOMinput(i+1,level,tag,xref,value);

        if(value != NULL) {
            free(value);
        }

        insertBack(&GEDCOMlist, newParse);
    }

    for(int i=0;i<numLines;i++) {
        if(GEDCOMpointer[i] != NULL) {
            free(GEDCOMpointer[i]);
        }
    }
    if(GEDCOMpointer != NULL) {
        free(GEDCOMpointer);
    }


    //*********************************************************************************
    //************** END OF FILE PARSER
    //*********************************************************************************
    *obj = createObject();
    Header* header = NULL;
    Individual* individual = NULL;
    Family* family = NULL;
    Submitter* submitter = NULL;
    int foundSUBM = 0;

    Node * temp = GEDCOMlist.head;
    GEDCOMinput* parsedData = (GEDCOMinput*)temp->data;


    //*********************************************************************************
    //************** START OF HEADER PARSER
    //*********************************************************************************
    

    temp = GEDCOMlist.head;
    parsedData = (GEDCOMinput*)temp->data;
    if(strcmp(parsedData->tag, "HEAD") == 0) {
        if(parsedData->level == 0) {
            header = parseHeader(header, temp, &err);
            if(header == NULL) {
                err.type = INV_HEADER;
                (*obj) = NULL;
                obj = NULL;

                return err;
            }
            (*obj)->header = header;

            //Move pointer to next record ----------------------
            temp = GEDCOMlist.head->next;
            parsedData = (GEDCOMinput*)temp->data;
            while(parsedData->level != 0) {
                temp = temp->next;
                parsedData = (GEDCOMinput*)temp->data;
            }
        }
        else {
            err.type = INV_HEADER;
            err.line = parsedData->line;
            (*obj) = NULL;
            obj = NULL;
            return err;
        }
    }

    else {
        err.type = INV_GEDCOM;
        err.line = -1;
        if(obj != NULL) {
            free(obj);
        }
        (*obj) = NULL;
        obj = NULL;
        return err;
    }

    //*********************************************************************************
    //************** END OF HEADER PARSER
    //*********************************************************************************

    //*********************************************************************************
    //************** START REST OF PARSER
    //*********************************************************************************

    parsedData = (GEDCOMinput*)temp->data;

    while(temp != NULL) {
        parsedData = (GEDCOMinput*)temp->data;
        if(strcmp(parsedData->tag, "INDI") == 0) {
            individual = parseIndividual(individual, temp, &err);
            if(individual == NULL) {
                err.type = INV_RECORD;
                free(obj);
                (*obj) = NULL;
                obj = NULL;
                return err;
            }
            insertBack(&(*obj)->individuals, individual);

            //Move pointer to next record ----------------------
            temp = temp->next;
            parsedData = (GEDCOMinput*)temp->data;
            while(parsedData->level != 0) {
                temp = temp->next;
                parsedData = (GEDCOMinput*)temp->data;
            }
        }
        else if(strcmp(parsedData->tag, "FAM") == 0) {
            family = parseFamily(family, temp, obj, &err);
            if(family == NULL) {
                err.type = INV_RECORD;
                free(obj);
                (*obj) = NULL;
                obj = NULL;
                return err;
            }
            insertBack(&(*obj)->families, family);

            //Move pointer to next record ----------------------
            temp = temp->next;
            parsedData = (GEDCOMinput*)temp->data;
            while(parsedData->level != 0) {
                temp = temp->next;
                parsedData = (GEDCOMinput*)temp->data;
            }
        }
        else if(strcmp(parsedData->tag, "SUBM") == 0) {
            foundSUBM = 1;
            (*obj)->submitter = parseSubmitter((*obj)->header, submitter, temp, &err);
            if((*obj)->submitter == NULL) {
                err.type = INV_HEADER;
                free(obj);
                (*obj) = NULL;
                obj = NULL;
                return err;
            }

            (*obj)->header->submitter = (*obj)->submitter;

            //Move pointer to next record ----------------------
            temp = temp->next;
            parsedData = (GEDCOMinput*)temp->data;
            while(parsedData->level != 0) {
                temp = temp->next;
                parsedData = (GEDCOMinput*)temp->data;
            }
        }
        else if(strcmp(parsedData->tag, "TRLR") == 0) {
            break;
        }
        else {
            err.type = INV_RECORD;
            err.line = parsedData->line;
            free(obj);
            (*obj) = NULL;
            obj = NULL;
            return err;
        }
    }

    if(foundSUBM == 0) {
        err.type = INV_GEDCOM;
        err.line = -1;
        (*obj) = NULL;
        obj = NULL;
        return err;
    }


    //*********************************************************************************
    //************** END REST OF PARSER
    //*********************************************************************************

    //*********************************************************************************
    //************** START REMOVE XREF TAGS FROM OTHER FIELDS
    //*********************************************************************************
    
    Node* indivList = (*obj)->individuals.head;

    while(indivList != NULL) {
        Individual* indiv = (Individual*)indivList->data;
        Node* fieldList = indiv->otherFields.head;
        while(fieldList != NULL) {
            Field * checkField = (Field*)fieldList->data;
            if(strcmp(checkField->tag, "INDI") == 0) {
                deleteDataFromList(&indiv->otherFields, checkField);
            }
            else if(strcmp(checkField->tag, "FAMC") == 0) {
                deleteDataFromList(&indiv->otherFields, checkField);
            }
            else if(strcmp(checkField->tag, "FAMS") == 0) {
                deleteDataFromList(&indiv->otherFields, checkField);
            }
            else if(strcmp(checkField->tag, "GIVN") == 0) {
                deleteDataFromList(&indiv->otherFields, checkField);
            }
            else if(strcmp(checkField->tag, "SURN") == 0) {
                deleteDataFromList(&indiv->otherFields, checkField);
            }
            else if(strcmp(checkField->tag, "NAME") == 0) {
                deleteDataFromList(&indiv->otherFields, checkField);
            }
            fieldList = fieldList->next;
        }
        indivList = indivList->next;
    }

    Node* headerIter = (*obj)->header->otherFields.head;
    while(headerIter != NULL) {
        Field* headerCheck = (Field*)headerIter->data;
        if(strcmp("SUBM", headerCheck->tag) == 0) {
            deleteDataFromList(&(*obj)->header->otherFields, headerCheck);
        }
        headerIter = headerIter->next;
    }

    //*********************************************************************************
    //************** END REMOVE XREF TAGS FROM OTHER FIELDS
    //*********************************************************************************

    clearList(&GEDCOMlist);

    fclose(fptr);
    err.type = OK;
    err.line = -1;
    return err;
}

char* printGEDCOM(const GEDCOMobject* obj) {
    
    if(obj == NULL) {
        return "Error: Cannot Print. Object is NULL.\n";
    }
    Node* temp = NULL;
    printf("===================================================\n");
    printf("=================== H E A D E R ===================\n");
    printf("===================================================\n");
    printf("Source: %s\n", obj->header->source);
    printf("Version: %f\n", obj->header->gedcVersion);
    printf("Encoding: ");
    switch(obj->header->encoding) {
        case ANSEL:
            printf("ANSEL");
            break;
        case UTF8:
            printf("UTF8");
            break;
        case UNICODE:
            printf("UNICODE");
            break;
        case ASCII:
            printf("ASCII");
            break;
    }
    printf("\n.......Other Fields.......\n");
    char* headOther = toString(obj->header->otherFields);
    printf("%s", headOther);
    free(headOther);
    printf("================= S U B M I T T E R ===============\n");
    printf("Name: %s\n", obj->header->submitter->submitterName);
    printf("Address: %s\n", obj->header->submitter->address);
    printf("\n.......Other Fields.......\n");
    char* submOther = toString(obj->header->otherFields);
    printf("%s", submOther);
    free(submOther);
    printf("=============== I N D I V I D U A L S =============\n");
    char* tempStr = toString(obj->individuals);
    printf("%s", tempStr);
    free(tempStr);

    printf("=================== F A M I L I E S ===============\n");
        temp = obj->families.head;
        while(temp != NULL) {
        printf("%s", obj->families.printData(temp->data));
        Family* family = (Family*)temp->data;
        
        Node* childIter = family->children.head;
        if(childIter != NULL) {
            printf("______________Children______________\n");
        }
        else {
            printf("No events to display\n");
        }
        while(childIter != NULL) {
            printf("%s", family->children.printData(childIter->data));
            childIter = childIter->next;
        }
        printf("==================================================\n");
        
        Node* eventIter = family->events.head;
        if(eventIter != NULL) {
            printf("_______________Events______________\n");
        }
        else {
            printf("No events to display\n");
        }
        while(eventIter != NULL) {
            printf("%s", family->events.printData(eventIter->data));
            eventIter = eventIter->next;
        }
        printf("==================================================\n");
        temp = temp->next;
    }

    return "ENDING\n";
}

void deleteGEDCOM(GEDCOMobject* obj) {
    if(obj == NULL) {
        return;
    }
    if(obj->submitter != NULL) {

        if(getLength(obj->submitter->otherFields) > 0) {
            clearList(&obj->submitter->otherFields);
        }
         //free(obj->submitter);
        obj->submitter = NULL;
    }

    if(obj->header != NULL) {

        if(getLength(obj->header->otherFields) > 0) {
            clearList(&obj->header->otherFields);
        }
        //free(obj->header);
        obj->header = NULL;
    }

    if(getLength(obj->families) > 0) {
        clearListNode(&obj->families);
    }
    if(getLength(obj->individuals) > 0) {
        clearList(&obj->individuals);
    }
    if(obj != NULL) {
        free(obj);
    }
}

char* printError(GEDCOMerror err) {

    switch(err.type) {
        case OK:
            return "OK";
            break;
        case INV_FILE:
            return "INV_FILE";
            break;
        case INV_GEDCOM:
            return "INV_GEDCOM";
            break;
        case INV_HEADER:
            return "INV_HEADER";
            break;
        case INV_RECORD:
            return "INV_RECORD";
            break;
        case OTHER_ERROR:
            return "OTHER_ERROR";
            break;
        case WRITE_ERROR:
            return "WRITE_ERROR";
            break;
    }
    //char* error = NULL;
    /*
    switch(err.type) {
        case OK:
        error = malloc(sizeof(char)*20);
            sprintf(error, "OK\nLine: %d\n", -1);
            break;
        case INV_FILE:
        error = malloc(sizeof(char)*20);
            sprintf(error, "INV_FILE\nLine: %d\n", err.line);
            break;
        case INV_GEDCOM:
        error = malloc(sizeof(char)*20);
            sprintf(error, "INV_GEDCOM\nLine: %d\n", err.line);
            break;
        case INV_HEADER:
        error = malloc(sizeof(char)*20);
            sprintf(error, "INV_HEADER\nLine: %d\n", err.line);
            break;
        case INV_RECORD:
        error = malloc(sizeof(char)*20);
            sprintf(error, "INV_RECORD\nLine: %d\n", err.line);
            break;
        case OTHER_ERROR:
        error = malloc(sizeof(char)*20);
            sprintf(error, "OTHER_ERROR\nLine: %d\n", err.line);
            break;
        case WRITE_ERROR:
        error = malloc(sizeof(char)*20);
            sprintf(error, "WRITE_ERROR\n");
            break;
    }
    return error;
    */
}

Individual* findPerson(const GEDCOMobject* familyRecord, bool (*compare)(const void* first, const void* second), const void* person) {

    if(familyRecord == NULL) {
        return NULL;
    }
    Individual* searchIndi = (Individual*)person;

    if(searchIndi == NULL) {
        return NULL;
    }

    Node* recNode = familyRecord->individuals.head;
    while(recNode != NULL) {
        Individual* compareIndi = (Individual*)recNode->data;

        if(compare(compareIndi, searchIndi)) {
            return compareIndi;
        }

        recNode = recNode->next;
    }
    return NULL;
}
List getDescendants(const GEDCOMobject* familyRecord, const Individual* person) {


    List descList = initializeList(&printIndividual, &deleteIndividual, &compareIndividuals);

    if(person == NULL) {
        return descList;
    }
    else {
        populateDescendants(&descList, person);
    }

    return descList;

}

GEDCOMerror writeGEDCOM(char* fileName, const GEDCOMobject* obj) {
    printf("WRITING GEDCOM FOR '%s'\n", fileName);
    GEDCOMerror err;
    err.type = OK;
    err.line = -1;
    FILE* fptr = NULL;

    int indivID = 1;
    int famID = 1;
    int numFams = 0;
    char stringID[8];
    char submTag[5] = "@U1@";

    if(fileName == NULL) {
        err.type = WRITE_ERROR;
        err.line = -1;
        return err;
    }
    if(obj == NULL) {
        err.type = WRITE_ERROR;
        err.line = -1;
        return err;
    }
    fptr = fopen(fileName, "w");
    if(fptr == NULL) {
        err.type = INV_FILE;
        return err;
    }

    Node* famIter = obj->families.head;
    while(famIter != NULL) {
        numFams++;
        famIter = famIter->next;
    }

    //*********************************************************************************
    //************** START ASSIGN INDIVIDUAL TAGS TO INDIVIDUALS
    //*********************************************************************************

    List indivTags = initializeList(&printWPerson, &deleteWPerson, &compareWPeople);
    Node* indivIter = obj->individuals.head;
    while(indivIter != NULL) {
        Individual* indiv = (Individual*)indivIter->data;
        sprintf(stringID, "@I%03d@", indivID);
        writePerson * storePerson = createWPerson(indiv, stringID);
        insertBack(&indivTags, storePerson);
        indivIter = indivIter->next;
        indivID++;
    }

    //*********************************************************************************
    //************** END ASSIGN INDIVIDUAL TAGS TO INDIVIDUALS
    //*********************************************************************************

    //*********************************************************************************
    //************** START ASSIGN FAMILY TAGS TO FAMILIES
    //*********************************************************************************

    famIter = obj->families.head;
    while(famIter != NULL) {
        sprintf(stringID, "@F%04d@", famID);
        Family* family = (Family*)famIter->data;
        //printFamily(family);
        Individual* husb = (Individual*)family->husband;
        Individual* wife = (Individual*)family->wife;
        Node* tagIndiv = indivTags.head;
        while(tagIndiv != NULL) {
            writePerson* taggedStruct = (writePerson*)tagIndiv->data;
            Individual* taggedIndiv = (Individual*)taggedStruct->person;
            if(taggedIndiv == husb) {
                addFamWPerson(taggedStruct, stringID, HUSB);
            }
            else if(taggedIndiv == wife) {
                addFamWPerson(taggedStruct, stringID, WIFE);
            }
            else {
                Node* children = family->children.head;
                while(children != NULL) {
                    Individual* child = (Individual*)children->data;
                    if(taggedIndiv == child) {
                        addFamWPerson(taggedStruct, stringID, CHIL);
                    }
                    children = children->next;
                }
            }
            tagIndiv = tagIndiv->next;
        }
        famID++;
        famIter = famIter->next;
    }

    //*********************************************************************************
    //************** END ASSIGN FAMILY TAGS TO FAMILIES
    //*********************************************************************************

    writeHeader(submTag, obj, fptr);
    writeIndividuals(indivTags, fptr);
    writeFamilies(indivTags,obj, numFams, fptr);
    writeSubmitter(obj, submTag, fptr);
    writeLine(0, "TRLR", "", fptr);
    fclose(fptr);
    printf("RETURNING SUCCESSFULLY FOR %s\n", fileName);
    return err;
}

ErrorCode validateGEDCOM(const GEDCOMobject* obj) {
    ErrorCode code = OK;
    int famLength = 0;
    int indivLength = 0;
    if(obj == NULL) {
        code = INV_GEDCOM;
        return code;
    }

    if(obj->header == NULL) {
        code = INV_GEDCOM;
        return code;
    }
    if(obj->header->submitter == NULL) {
        code = INV_HEADER;
        return code;
    }
    if(obj->submitter == NULL) {
        code = INV_GEDCOM;
        return code;
    }

    if(strcmp(obj->header->source, "") == 0) {
        code = INV_HEADER;
        return code;
    }

    if(obj->header->gedcVersion == 0) {
        code = INV_HEADER;
        return code;
    }

    if(strcmp(obj->submitter->submitterName, "") == 0) {
        code = INV_RECORD;
        return code;
    }

    Node* famIter = obj->families.head;
    while(famIter != NULL) {
        famIter = famIter->next;
        famLength++;
    }
    
    if(famLength != obj->families.length) {
        code = INV_RECORD;
        return code;
    }
    

    Node* indivIter = obj->individuals.head;
    while(indivIter != NULL) {
        Individual * checkIndiv = (Individual*)indivIter->data;
        if(checkIndiv->givenName == NULL || checkIndiv->surname == NULL) {
            code = INV_RECORD;
            return code;
        }
        else if(strlen(checkIndiv->givenName) > 200 || strlen(checkIndiv->surname) > 200) {
            code = INV_RECORD;
            return code;
        }
       
        Node* eventIter = checkIndiv->events.head;
        while(eventIter != NULL) {
             Event * checkEvents = (Event*)eventIter->data;
             if(checkEvents->date == NULL || checkEvents->place == NULL) {
                code = INV_RECORD;
                return code;
             }
             else if(strlen(checkEvents->date) > 200 || strlen(checkEvents->place) > 200) {
                code = INV_RECORD;
                return code;
             }
            eventIter = eventIter->next;
        }
        indivIter = indivIter->next;
        indivLength++;
    }
    
    if(indivLength != obj->individuals.length) {
        code = INV_RECORD;
        return code;
    }

    return code;
}

List getDescendantListN(const GEDCOMobject* familyRecord, const Individual* person, unsigned int maxGen) {
    List allDesc = initializeList(&printIndividual, &deleteIndividual, &compareIndividuals);
    List* desc = createList(&printIndividual, &deleteIndividual, &compareIndividuals);
    List* currList = createList(&printIndividual, &deleteIndividual, &compareIndividuals);
    int currGen = 0;
    if(familyRecord == NULL || person == NULL) {
        return allDesc;
    }
    Family* directDesc = NULL;
    Node* famIter = familyRecord->families.head;
    while(famIter != NULL) {
        Family * family = (Family*)famIter->data;
        
        Individual * husb = (Individual*)family->husband;
        Individual * wife = (Individual*)family->wife;
        if(compareIndividuals(person, husb) == 0 || (compareIndividuals(person, wife) == 0)) {
            directDesc = family;
            break;
        }
        famIter = famIter->next;
    }
    if(directDesc == NULL) {
        return allDesc;
    }
    Node* descIter = directDesc->children.head;
    while(descIter != NULL) {
        Individual* child = (Individual*)descIter->data;
        insertSorted(desc, child);
        descIter = descIter->next;
    }
        // NOW HAVE FIRST GEN OF FAMILY
    insertBack(&allDesc, desc);
    currGen++;
    if(currGen == maxGen) {
        return allDesc;
    }

    Node* directIter = desc->head;
    while(directIter != NULL) {
        Individual * directIndiv = (Individual*)directIter->data;
        //printIndividual(directIndiv);
        Family * directIndivFam = getFamily(directIndiv);
        if(directIndivFam != NULL) {
            Node* insertChildren = directIndivFam->children.head;
            while(insertChildren != NULL) {
                Individual* putChild = (Individual*)insertChildren->data;
                //printIndividual(putChild);
                insertSorted(currList, putChild);
                insertChildren = insertChildren->next;
            }
        }
    directIter = directIter->next;
    }
    if(currList->length == 0) {
        return allDesc;
    }
    insertBack(&allDesc, currList);
    currGen++;
    if(currGen == maxGen) {
        return allDesc;
    }
    while(currGen < maxGen) {
        List* addList = createList(&printIndividual, &deleteIndividual, &compareIndividuals);
        Node* genIter = currList->head;
        while(genIter != NULL) {
            Individual * addIndividual = (Individual*)genIter->data;
            Family * addFamily = getFamily(addIndividual);
            if(addFamily != NULL) {
            Node* insertChildren = addFamily->children.head;
                while(insertChildren != NULL) {
                    Individual* putChild = (Individual*)insertChildren->data;
                    //printIndividual(putChild);
                    insertSorted(currList, putChild);
                    insertChildren = insertChildren->next;
                }
            }
            
            genIter = genIter->next;
        }
        currList = addList;
        insertBack(&allDesc, currList);
        
        currGen++;
    }
    return allDesc;
}

List getAncestorListN(const GEDCOMobject* familyRecord, const Individual* person, int maxGen) {
    List allAns = initializeList(&printIndividual, &deleteIndividual, &compareIndividuals);
    List* parents = createList(&printIndividual, &deleteIndividual, &compareIndividuals);
    List* currList = createList(&printIndividual, &deleteIndividual, &compareIndividuals);
    int currGen = 0;
    if(familyRecord == NULL || person == NULL) {
        return allAns;
    }
    Family* getParents = NULL;
    Node* famIter = familyRecord->families.head;
    while(famIter != NULL) {
        Family * family = (Family*)famIter->data;
        Node* childrenIter = family->children.head;
        while(childrenIter != NULL) {
            Individual * checkChild = (Individual*)childrenIter->data;
            if(compareIndividuals(person, checkChild) == 0) {
                getParents = family;
                break;
            }
            childrenIter = childrenIter->next;
        }
        famIter = famIter->next;
    }
    if(getParents == NULL) {
        return allAns;
    }
    if(getParents->wife != NULL) {
        Individual* mom = (Individual*)getParents->wife;
        insertSorted(parents, mom);
    }
    if(getParents->husband != NULL) {
        Individual* dad = (Individual*)getParents->husband;
        insertSorted(parents, dad);
    }
    currList = parents;
    if(currList->length == 0) {
        return allAns;
    }
    insertBack(&allAns, currList);
    currGen++;
    if(currGen == maxGen) {
        return allAns;
    }
    while(currGen < maxGen) {
        int foundDad = 0;
        int foundMom = 0;
        List* addList = createList(&printIndividual, &deleteIndividual, &compareIndividuals);
        Node* parentIter = currList->head;
        while(parentIter != NULL) {
            Individual* findParent = (Individual*)parentIter->data;
            Node * parentFamilies = findParent->families.head;

            while(parentFamilies != NULL) {
                Family * checkFamily = (Family*)parentFamilies->data;
                Node* childrenIter = checkFamily->children.head;
                while(childrenIter != NULL) {
                    Individual* checkChild = (Individual*)childrenIter->data;
                    if(compareIndividuals(checkChild, findParent) == 0) {
                        if(checkFamily->husband != NULL) {
                            if(foundDad == 0) {
                                Individual* dad = (Individual*)checkFamily->husband;
                                insertSorted(addList, dad);
                                foundDad = 1;
                                break;
                            }
                            
                        }
                        if(checkFamily->wife != NULL) {
                            if(foundMom == 0) {
                                Individual* mom = (Individual*)checkFamily->wife;
                                insertSorted(addList, mom);
                                foundMom = 1;
                                break;
                            }
                        }
                    }
                    childrenIter = childrenIter->next;
                }
                parentFamilies = parentFamilies->next;
            }
            parentIter = parentIter->next;
        }
        
        currList = addList;
        if(currList->length == 0) {
            return allAns;
        }
        insertBack(&allAns, currList);
        currGen++;
    }


    return allAns;
}

char* indToJSON(const Individual* ind) {
    char* first = NULL;
    char* last = NULL;
    char* sex = NULL;
    int intFamSize = 0;
    char* famSize = NULL;
    Family* family = NULL;
    if(ind == NULL) {
        return "";
    }
    if(ind->givenName != NULL) {
        first = malloc(sizeof(char)*strlen(ind->givenName)+10);
        strcpy(first, ind->givenName);

    }
    else {
        first = malloc(sizeof(char)*3);
        strcpy(first, "");
    }

    if(ind->surname != NULL) {
        last = malloc(sizeof(char)*strlen(ind->surname)+10);
        strcpy(last, ind->surname);
    }
    else {
        last = malloc(sizeof(char)*3);
        strcpy(last, "");
    }

    Node* getSex = ind->otherFields.head;
    while(getSex != NULL) {
        Field* currField = (Field*)getSex->data;
        if(strcmp(currField->tag, "SEX") == 0) {
            sex = malloc(sizeof(char)*4);
            strcpy(sex, currField->value);
            break;
        }
        getSex = getSex->next;
    }
    if(sex == NULL) {
        sex = malloc(sizeof(char)*4);
        strcpy(sex, "");
    }

    int foundSpouse = 0;

    Node * famIter = ind->families.head;
    while(famIter != NULL) {
        Family* curr = (Family*)famIter->data;
        Individual* husb = (Individual*)curr->husband;
        Individual* wife = (Individual*)curr->wife;
        if(ind == husb || ind == wife) {
            foundSpouse = 1;
            break;
        }
        famIter = famIter->next;
    }

    if(foundSpouse == 1) {
        family = (Family*)famIter->data;
    }
    else {
        family = (Family*)ind->families.head->data;
    }
    famSize = malloc(sizeof(char)*4);

    if(family->husband != NULL) {
        intFamSize++;
    }
    if(family->wife != NULL) {
        intFamSize++;
    }
    Node * kids = family->children.head;
    while(kids != NULL) {
        intFamSize++;
        kids = kids->next;
    }
    if(intFamSize == 0) {
        intFamSize = 1;
    }
    sprintf(famSize, "%d", intFamSize);

    char* string = malloc(sizeof(char)*(strlen(first)+strlen(last)+200));

    //sprintf(string, "{\"givenName\":\"%s\",\"surname\":\"%s\"}", first, last);
    sprintf(string, "{\"givenName\":\"%s\",\"surname\":\"%s\",\"sex\":\"%s\",\"famSize\":\"%s\"}", first, last, sex, famSize);
    //printf("%s\n\n", string);
    /*
    if(first != NULL) {
        free(first);
    }
    if(last != NULL) {
        free(last);
    }
    if(sex != NULL) {
        free(sex);
    }
    if(famSize != NULL) {
        free(famSize);
    }
    */
    return string;
}

Individual* JSONtoInd(const char* str) {

    if(str == NULL) {
        return NULL;
    }
    char* delims = "\"{}:";
    char* copyString = malloc(sizeof(char)*strlen(str)+1);
    strcpy(copyString, str);
    char* substring = strtok(copyString, delims);
    if(substring == NULL) {
            return NULL;
    }
    char* givenName = NULL;
    char* surname = NULL;
    substring = strtok(NULL, delims);
    if(substring == NULL) {
            return NULL;
    }
    if(strcmp(substring, ",") == 0) {
        givenName = malloc(sizeof(char)*2);
        strcpy(givenName, "");
        substring = strtok(NULL, delims);
        substring = strtok(NULL, delims);
    }
    else {
        givenName = malloc(sizeof(char)*strlen(substring)+1);
        strcpy(givenName, substring);
        substring = strtok(NULL, delims);
        if(substring == NULL) {
            return NULL;
        }
        substring = strtok(NULL, delims);
        if(substring == NULL) {
            return NULL;
        }
        substring = strtok(NULL, delims);
    }

    
    if(substring == NULL) {
        surname = malloc(sizeof(char)*2);
        strcpy(surname, "");
    }
    else {
        surname = malloc(sizeof(char)*strlen(substring)+1);
        strcpy(surname, substring);
    }

    List events = initializeList(&printEvent, &deleteEvent, &compareEvents);
    List otherFields = initializeList(&printField, &deleteField, &compareFields);
    Individual* individual = createIndividual(givenName, surname, events, otherFields);
    return individual;
}

GEDCOMobject* JSONtoGEDCOM(const char* str) {
    if(str == NULL) {
        return NULL;
    }
    GEDCOMobject* obj = NULL;
    char* copyString = malloc(sizeof(char)*strlen(str)+1);
    char source[250];
    char submitterName[250];
    char address[250];
    float gedcVersion = 0;
    CharSet encoding;
    strcpy(copyString, str);
    char* substring = strtok(copyString, "\"");
    if(substring == NULL) {
            return NULL;
    }
    if(strcmp(substring, "{") == 0) {
        substring = strtok(NULL, "\"");
        if(substring == NULL) {
            return NULL;
        }
        if(strcmp(substring, "source") == 0) {
            substring = strtok(NULL, "\"");
            if(substring == NULL) {
            return NULL;
            }
            if(strcmp(substring, ":") == 0) {
                substring = strtok(NULL, "\"");
            }
            else {
                return NULL;
            }
            if(substring == NULL) {
                return NULL;
            }
            if(strcmp(substring, ",") == 0) {
                return NULL;
            }
            else {
                strcpy(source, substring);
            }
            substring = strtok(NULL, "\"");
            if(substring == NULL) {
                return NULL;
            }
            if(strcmp(substring, ",") == 0) {
                substring = strtok(NULL, "\"");
            }
            else {
                return NULL;
            }
            if(substring == NULL) {
                return NULL;
            }
            if(strcmp(substring, "gedcVersion") == 0) {
                substring = strtok(NULL, "\"");
            }
            else {
                return NULL;
            }
            if(strcmp(substring, ":") == 0) {
                substring = strtok(NULL, "\"");
            }
            else {
                return NULL;
            }
            if(substring == NULL) {
                return NULL;
            }
            if(strcmp(substring, ",") == 0) {
                return NULL;
            }
            else {
                gedcVersion = atof(substring);
            }
            substring = strtok(NULL, "\"");
            if(substring == NULL) {
                return NULL;
            }
            if(strcmp(substring, ",") == 0) {
                substring = strtok(NULL, "\"");
            }
            else {
                return NULL;
            }
            if(substring == NULL) {
                return NULL;
            }
            if(strcmp(substring, "encoding") == 0) {
                substring = strtok(NULL, "\"");
            }
            else {
                return NULL;
            }
            if(substring == NULL) {
                return NULL;
            }
            if(strcmp(substring, ":") == 0) {
                substring = strtok(NULL, "\"");
            }
            else {
                return NULL;
            }
            if(substring == NULL) {
                return NULL;
            }
            if(strcmp(substring, ",") == 0) {
                return NULL;
            }
            else {
                if(strcmp(substring, "ANSEL") == 0) {
                    encoding = ANSEL;
                }
                else if(strcmp(substring, "UTF-8") == 0) {
                    encoding = UTF8;
                }
                else if(strcmp(substring, "UNICODE") == 0) {
                    encoding = UNICODE;
                }
                else if(strcmp(substring, "ASCII") == 0) {
                    encoding = ASCII;
                }
                else {
                    return NULL;
                }
            }
            substring = strtok(NULL, "\"");
            if(substring == NULL) {
                return NULL;
            }
            if(strcmp(substring, ",") == 0) {
                substring = strtok(NULL, "\"");
            }
            else {
                return NULL;
            }
            if(substring == NULL) {
                return NULL;
            }
            if(strcmp(substring, "subName") == 0) {
                substring = strtok(NULL, "\"");
            }
            else {
                return NULL;
            }
            if(substring == NULL) {
                return NULL;
            }
            if(strcmp(substring, ":") == 0) {
                substring = strtok(NULL, "\"");
            }
            else {
                return NULL;
            }
            if(substring == NULL) {
                return NULL;
            }
            if(strcmp(substring, ",") == 0) {
                return NULL;
            }
            else {
                strcpy(submitterName, substring);
            }
            substring = strtok(NULL, "\"");
            if(substring == NULL) {
                return NULL;
            }
            if(strcmp(substring, ",") == 0) {
                substring = strtok(NULL, "\"");
            }
            else {
                return NULL;
            }
            if(substring == NULL) {
                return NULL;
            }
            if(strcmp(substring, "subAddress") == 0) {
                substring = strtok(NULL, "\"");
            }
            else {
                return NULL;
            }
            if(substring == NULL) {
                return NULL;
            }
            if(strcmp(substring, ":") == 0) {
                substring = strtok(NULL, "\"");
            }
            else {
                return NULL;
            }
            if(substring == NULL) {
                return NULL;
            }
            if(strcmp(substring, "}") == 0) {
                strcpy(address, "");
            }
            else {
                strcpy(address, substring);
            }
        }
        else {
            return NULL;
        }
    }
    else {
        return NULL;
    }
    obj = createObject();
    obj->submitter = malloc(sizeof(Submitter)+250);
    obj->submitter->otherFields = initializeList(&printField, &deleteField, &compareFields);
    strcpy(obj->submitter->submitterName, submitterName);
    strcpy(obj->submitter->address, address);
    obj->header = malloc(sizeof(Header));
    strcpy(obj->header->source, source);
    obj->header->gedcVersion = gedcVersion;
    obj->header->encoding = encoding;
    obj->header->submitter = obj->submitter;
    return obj;
}

void addIndividual(GEDCOMobject* obj, const Individual* toBeAdded) {
    if(obj != NULL && toBeAdded != NULL) {
        insertBack(&obj->individuals, (void*)toBeAdded);
    }
    return;
}
char* iListToJSON(List iList) {
    char* string = NULL;
    if(getLength(iList) < 1) {
        return NULL;
    }
    if(iList.head == NULL) {
        return NULL;
    }

    if(iList.length == 0) {
        return NULL;
    }

    int totalSize = 0;
    int count = 0;
    Node * temp = iList.head;
    while(temp != NULL) {
        Individual * indiv = (Individual*)temp->data;
        char* JSONString = indToJSON(indiv);
        totalSize+=strlen(JSONString);
        count++;
        temp = temp->next;
    }
    string = malloc(sizeof(char)*(totalSize+count*20));

        strcpy(string, "[");


    temp = iList.head;
    while(temp != NULL) {
        Individual * indiv = (Individual*)temp->data;
        char* JSONString = indToJSON(indiv);
        strcat(string, JSONString);
        if(temp->next != NULL) {
            strcat(string, ",");
        }
        //free(JSONString);
        temp = temp->next;
    }

    strcat(string, "]");

    return string;
}
char* gListToJSON(List gList) {

    if(getLength(gList) < 1) {
        return "[]";
    }
    int count = 0;
    int totalSize = 0;
    Node* gListIter = gList.head;
    while(gListIter != NULL) {
        List* currGen = (List*)gListIter->data;
        Node* currGenIter = currGen->head;
        while(currGenIter != NULL) {
            Individual * indiv = (Individual*)currGenIter->data;
            char* JSONString = indToJSON(indiv);
            printf("JSON IS %s\n", JSONString);
            totalSize+=strlen(JSONString);
            count++;
            currGenIter = currGenIter->next;
        }
        gListIter = gListIter->next;
    }
    char* string = malloc(sizeof(char)*totalSize+count+3);
    strcat(string, "[");
    gListIter = gList.head;
    int iter = 0;
    while(gListIter != NULL) {
        if(iter > 0) {
            strcat(string, ",");
        }
        strcat(string, "[");
        List* currGen = (List*)gListIter->data;
        Node* currGenIter = currGen->head;
        while(currGenIter != NULL) {
            Individual * indiv = (Individual*)currGenIter->data;
            char* JSONString = indToJSON(indiv);
            strcat(string, JSONString);
            if(currGenIter->next != NULL) {
                strcat(string, ",");
            }
            currGenIter = currGenIter->next;
        }
        strcat(string, "]");
        gListIter = gListIter->next;
        iter++;
    }
    strcat(string, "]");
    return string;
}

void deleteGeneration(void* toBeDeleted) {
    return;
}
int compareGenerations(const void* first,const void* second) {
    return 0;
}
char* printGeneration(void* toBePrinted) {
    return NULL;
}
//************************************************************************************************************

void deleteEvent(void* toBeDeleted) {
    Event* delEvent = (Event*)toBeDeleted;

    if(delEvent->place != NULL) {
        free(delEvent->place);
    }
    if(delEvent->date != NULL) {
        free(delEvent->date);
    }
    if(delEvent != NULL) {
        free(delEvent);
    }
}
int compareEvents(const void* first,const void* second) {
    if(first == NULL || second == NULL) {
        return -2;
    }
    Event* f = (Event*)first;
    Event* s = (Event*)second;
    char buf1[200];
    char buf2[200];
    sprintf(buf1, "%s %s %s", f->type, f->date, f->place);
    sprintf(buf2, "%s %s %s", s->type, s->date, s->place);

    if(strcmp(buf1, buf2) > 0) {
        return 1;
    }
    else if(strcmp(buf1, buf2) < 0) {
        return -1;
    }
    else {
        return 0;
    }
}
char* printEvent(void* toBePrinted) {
    Event* event = (Event*)toBePrinted;
    printf("Place: %s\n", event->place);
    return "\n";
}

void deleteIndividual(void* toBeDeleted) {
    Individual* delIndi = (Individual*)toBeDeleted;
    if(delIndi->givenName != NULL) {
        free(delIndi->givenName);
    }
    if(delIndi->surname != NULL) {
        free(delIndi->surname);
    }
    clearListNode(&delIndi->families);
    clearList(&delIndi->events);
    clearList(&delIndi->otherFields);
    if(delIndi != NULL) {
        free(delIndi);
    }
}
int compareIndividuals(const void* first,const void* second) {
    if(first == NULL || second == NULL) {
        return -2;
    }
    char buf1[200];
    char buf2[200];

    Individual* f = (Individual*)first;
    Individual* s = (Individual*)second;
    sprintf(buf1, "%s %s", f->givenName, f->surname);
    sprintf(buf2, "%s %s", s->givenName, s->surname);
    if(strcmp(buf1, buf2) > 0) {
        return 1;
    }
    else if(strcmp(buf1, buf2) < 0) {
        return -1;
    }
    else {
        Node* firstIter = f->events.head;
        Node* secondIter = s->events.head;
        while(firstIter != NULL || secondIter != NULL) {
            if(compareEvents(firstIter->data, secondIter->data) > 0) {
                return 1;
            }
            else if(compareEvents(firstIter->data, secondIter->data) < 0) {
                return -1;
            }
            firstIter = firstIter->next;
            secondIter = secondIter->next;
        }
    }
    return 0;
}
char* printIndividual(void* toBePrinted) {
    Individual* indiv = (Individual*)toBePrinted;
    printf("%s, %s\n", indiv->surname, indiv->givenName);
    /*********
    Node * findBirth = indiv->events.head;
    while(findBirth != NULL) {
        Event* birth = (Event*)findBirth->data;
        if(strcmp(birth->type, "BIRT") == 0) {
            printf("Born: %s\n", birth->date);
        }
        else {
            printf("EVENT: %s %s %s\n", birth->type, birth->date, birth->place);
        }
        findBirth = findBirth->next;
    }
    **********/
    /*********
    Node* other = indiv->otherFields.head;
    while(other != NULL) {
        printf("%s\n", indiv->otherFields.printData(other->data));
        other = other->next;
    }
**********/
    return "";
}

void deleteFamily(void* toBeDeleted) {
    Family * delFam = (Family*)toBeDeleted;

    if(delFam->wife != NULL) {
        free(delFam->wife);
    }
    if(delFam->husband != NULL) {
        free(delFam->husband);
    }
    clearListNode(&delFam->children);
    clearList(&delFam->events);
    clearList(&delFam->otherFields);
    if(delFam != NULL) {
        free(delFam);
    }
}
int compareFamilies(const void* first,const void* second) {
    if(first == NULL || second == NULL) {
        return -2;
    }
    Family* f = (Family*)first;
    Family* s = (Family*)second;
    int num1 = 0;
    int num2 = 0;
    if(f->wife != NULL) {
        num1++;
    }
    if(f->husband != NULL) {
        num1++;
    }
    Node * chIter = f->children.head;
    while(chIter != NULL) {
        num1++;
        chIter = chIter->next;
    }

    if(s->wife != NULL) {
        num2++;
    }
    if(s->husband != NULL) {
        num2++;
    }
    chIter = s->children.head;
    while(chIter != NULL) {
        num2++;
        chIter = chIter->next;
    }

    if(num1 > num2) {
        return 1;
    }
    else if(num1 < num2) {
        return -1;
    }
    else {
        return 0;
    }
}
char* printFamily(void* toBePrinted) {
    Family * fam = (Family*)toBePrinted;
    Individual * husband = (Individual*)fam->husband;
    Individual * wife = (Individual*)fam->wife;
    if(husband != NULL) {
        printf("HUSB: %s %s\n", husband->givenName, husband->surname);
    }
    else {
        printf("HUSB: <EMPTY>\n");
    }
    if(wife != NULL) {
        printf("WIFE: %s, %s\n", wife->givenName, wife->surname);
    }
    else {
        printf("WIFE: <EMPTY>\n");
    }
    return "";
}

void deleteField(void* toBeDeleted) {
    Field* delField = (Field*)toBeDeleted;
    if(delField->tag != NULL) {
        free(delField->tag);
    }
    if(delField->value != NULL) {
        free(delField->value);
    }
    if(delField != NULL) {
        free(delField);
    }
}
int compareFields(const void* first,const void* second) {
    Field * f = (Field*)first;
    Field * s = (Field*)second;

    if(strcmp(f->value, s->value) == 0) {
            return 0;
    }
    else {
        return 1;
    }
}
char* printField(void* toBePrinted) {
    Field* field = (Field*)toBePrinted;
    printf("TAG: %s\nVALUE: %s\n", field->tag, field->value);
    return "\n";
}

//************************************************************************************************************
char* fileToJSON(char* path, char* fileName) {

    char* convertFile = malloc(sizeof(char)*strlen(path)+strlen(fileName)+10);
    sprintf(convertFile, "%s/%s", path, fileName);

    GEDCOMobject* tempObj = NULL;
    GEDCOMerror err;

    err = createGEDCOM(convertFile, &tempObj);

    if(err.type == OK) {
        
        char* source = NULL;
        char* gedc = NULL;
        char* encoding = NULL;
        char* submitter = NULL;
        char* address = NULL;
        char* finalString = NULL;
        char* indivs = NULL;
        char* fams = NULL;
        source = stringToJSON("source", tempObj->header->source);
        char* gedcVersion = malloc(sizeof(char) * 5);
        sprintf(gedcVersion, "%.2f", tempObj->header->gedcVersion);
        gedc = stringToJSON("gedc", gedcVersion);

        char encodingVal[10];
        switch(tempObj->header->encoding) {
            case ANSEL:
                strcpy(encodingVal, "ANSEL");
                break;
            case UTF8:
                strcpy(encodingVal, "UTF-8");
                break;
            case UNICODE:
                strcpy(encodingVal, "UNICODE");
                break;
            case ASCII:
                strcpy(encodingVal, "ASCII");
                break;
        }
        encoding = stringToJSON("encoding", encodingVal);

        submitter = stringToJSON("submitter", tempObj->submitter->submitterName);
        if(strcmp(tempObj->submitter->address, "") == 0) {
            address = stringToJSON("address", tempObj->submitter->address);
        }
        else {
            address = stringToJSON("address", "");
        }


        char* numIndivs = malloc(sizeof(char)*10);
        sprintf(numIndivs,"%d", tempObj->individuals.length);

        indivs = stringToJSON("indivs", numIndivs);

        char* numFams = malloc(sizeof(char)*10);
        sprintf(numFams,"%d", tempObj->families.length);
        fams = stringToJSON("fams", numFams);
        int length = strlen(source)+strlen(gedc)+strlen(encoding)+strlen(submitter)+strlen(address) + strlen(indivs) + strlen(fams);

        finalString = malloc(sizeof(char) * length + 50);
        sprintf(finalString, "{%s,%s,%s,%s,%s,%s,%s}", source, gedc, encoding, submitter, address, indivs, fams);
/*
        if(source != NULL) {
            free(source);
        }
        if(gedc != NULL) {
            free(gedc);
        }
        if(encoding != NULL) {
            free(encoding);
        }
        if(submitter != NULL) {
            free(submitter);
        }
        if(address != NULL) {
            free(address);
        }
*/
        if(tempObj != NULL) {
            deleteGEDCOM(tempObj);
        }
        
        return finalString;
    }
    return "{}";
}

char* stringToJSON(char* type, char* string) {
    char* JSON = malloc(sizeof(char)*(strlen(string) + strlen(type) + 20));
    if(string == NULL) {
        sprintf(JSON, "\"%s\":\"\"", type);
    }
    else {
        sprintf(JSON, "\"%s\":\"%s\"", type, string);
    }
    return JSON;

}

char* JSONtoString(char* JSONstring) {
    char* delims = "\"{}:";
    char* substring = strtok(JSONstring, delims);
    char* type = NULL;
    char* string = NULL;
    type = malloc(sizeof(char)* strlen(substring)+1);
    strcpy(type, substring);
    substring = strtok(NULL, delims);

    string = malloc(sizeof(char)*strlen(substring)+1);
    strcpy(string, substring);
    substring = strtok(NULL, delims);
    return string;
}
char* formToGEDCOM(char* fileName, char* submitter, char* address) {

    char* delims = "\"";
    GEDCOMobject* obj = NULL;
    char* file = strtok(fileName, delims);
    char* subm = strtok(submitter, delims);
    char* addr = strtok(address, delims);

    obj = createObject();
    obj->submitter = malloc(sizeof(Submitter)+250);
    obj->submitter->otherFields = initializeList(&printField, &deleteField, &compareFields);
    obj->families = initializeList(&printFamily, &deleteFamily, &compareFamilies);
    obj->individuals = initializeList(&printIndividual, &deleteIndividual, &compareIndividuals);
    strcpy(obj->submitter->submitterName, subm);
    strcpy(obj->submitter->address, addr);
    obj->header = malloc(sizeof(Header));
    strcpy(obj->header->source, "PAF");
    obj->header->gedcVersion = 5.5;
    obj->header->encoding = UTF8;
    obj->header->submitter = obj->submitter;
    obj->header->otherFields = initializeList(&printField, &deleteField, &compareFields);

    char* fullFile = malloc(sizeof(char) * (strlen(file) + 100));
    sprintf(fullFile, "./uploads/%s", file);
    writeGEDCOM(fullFile, obj);

    return "";
}

char* indivsToJSON(char* fileName) {
    if(fileName == NULL) {
        return "[]";
    }
    char* delims = "\"";
    char* file = strtok(fileName, delims);
    if(file == NULL) {
        return "Invalid file";
    }
    char* fullFile = malloc(sizeof(char) * (strlen(file) + 100));
    sprintf(fullFile, "./uploads/%s", file);

    GEDCOMobject* obj = NULL;
    GEDCOMerror err; 
    err = createGEDCOM(fullFile, &obj);
    if(err.type == OK) {

        char* indivs = iListToJSON(obj->individuals);
        if(indivs == NULL) {
            return "[]";
        }
        return indivs;
    }

    return "[]";
}

char* addIndivToFile(char* JSONobject, char* fileName) {
    printf("%s", fileName);
    printf("%s", JSONobject);
    GEDCOMobject* obj = NULL;
    GEDCOMerror err;
    Individual* add = JSONtoInd(JSONobject);
    char* fullFile = malloc(sizeof(char) * (strlen(fileName) + 100));
    sprintf(fullFile, "./uploads/%s", fileName);
    err = createGEDCOM(fullFile, &obj);
    if(err.type == OK) {
        addIndividual(obj, add);
        writeGEDCOM(fullFile, obj);
        return "Successfully added individual";
    }
    else {
        return "Error adding individual";
    }
}
//************************************************************************************************************
