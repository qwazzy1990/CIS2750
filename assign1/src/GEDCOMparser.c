#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GEDCOMparser.h"
#include <ctype.h>
#include "GEDCOMutilities.h"

#define DEBUG false
//checks if TRLR tag exists
//bool checkForTrailer(FILE* fp);
//check of file is empty
//bool isFileEmpty(FILE* fp);
//check if file contains a header?
//bool containsHeader(FILE* fp);

GEDCOMerror createGEDCOM(char* fileName, GEDCOMobject** obj)
{
    //if file name is the file he is checking then print based on what the file is checking for
    GEDCOMerror error;
    int lineNumber = 0;
    GEDCOMobject* temp = (GEDCOMobject*)malloc(sizeof(GEDCOMobject));
    char** theFile = NULL;
    FILE* FP = NULL;
    temp->individuals = createIndividualList(&printIndividual, &deleteIndividual, &compareIndividuals);
    temp->families = createFamilyList(&printFamily, &deleteFamily, &compareFamilies);
    temp->header = NULL;
    temp->submitter = NULL;

    if(doesStringExist(fileName)==false){
        error.type = INV_FILE;
        error.line = -1;
        return error;
    }

    if(validFileName(fileName)==false){
      error.type = INV_FILE;
      error.line = -1;
      return error;
    }
    FP = openFile(fileName);
    if(FP==NULL){
      error.type = INV_FILE;
      error.line = -1;
      return error;
    }




    theFile = returnFile(FP, &lineNumber);


    if(theFile == NULL){
        error.type = INV_GEDCOM;
        error.line = lineNumber;
        return error;
    }


    List gedLineList = createGEDLineList(theFile, lineNumber, &printGEDline, &destroyGEDline, &compareGEDline);

    HeaderCopy* head = createHeaderCopy();

    SubmitterCopy* subm = createSubmitterCopy();

    List recordList = createRecordList(&printGEDline, &destroyGEDline, &compareGEDline);

    List iCopyList = createIndividualCopyList(&printIndividualCopy, &deleteIndividualCopy, &compareIndividuals);

    List fCopyList = createFamilyCopyList(&printFamilyCopy, &deleteFamilyCopy, &compareFamilies);

    freeStringArray(theFile);

    /*******************PARSING FILE******************/
    lineNumber = 0;
    error.type = generalParser(gedLineList, recordList, &iCopyList, &fCopyList, head, subm, &lineNumber);
    if(error.type == INV_HEADER){
      error.line = lineNumber;
      return error;
    }
    if(error.type == INV_RECORD){
      error.line = lineNumber;
      return error;
    }//end Parse

    //Validate object
   generalValidator(&gedLineList, head, subm, &iCopyList, &fCopyList, &lineNumber, &error);
    //end validation
    if(error.type != OK){
         *obj = NULL;
         return error;
    }

    clearList(&recordList);

    linkTwoLists(&iCopyList, &fCopyList);
    populateIndividualList(&temp->individuals, &iCopyList);
    populateFamilyList(&temp->families, &fCopyList);
    //clearList(&iCopyList);
    //clearList(&fCopyList);
    (temp)->header = createHeader(head);
    (temp)->submitter = createSubmitter(subm);
    deleteSubmitterCopy(subm);

    //cloneFamilyList(&(*obj)->families, &fList);
    //cloneIndividualList(&(*obj)->individuals, &iList);
    *obj = temp;
    fileCloser(FP);
    return error;
}

/**
   *Function to print an event object
   *param = event object
*/



char* printGEDCOM(const GEDCOMobject* obj)
{
    char* returnString = (char*)calloc(8000000, sizeof(char));
    strcpy(returnString, toString((obj)->families));
    printf("%s\n", returnString);
    return returnString;

}




char* printError(GEDCOMerror err)
{
    char* errorMessage = NULL;
    stAllocater(&errorMessage);
    if(err.type == OK){
      strcpy(errorMessage, "----OK----");
    }else if(err.type == INV_HEADER){
        strcpy(errorMessage, "INVALID HEADER");
    }else if(err.type == INV_FILE){
        strcpy(errorMessage, "INVALID FILE");
    }else if(err.type == INV_GEDCOM){
        strcpy(errorMessage, "INVALID GEDCOM");
    }else{
        strcpy(errorMessage, "INVALID RECORD");
    }
    return errorMessage;
}


Individual* findPerson(const GEDCOMobject* familyRecord, bool (*compare)(const void* first, const void* second), const void* person)
{
     Individual* indi = (Individual*)findElement(familyRecord->individuals, compare, person);
     return indi;
}


List getDescendants(const GEDCOMobject* familyRecord, const Individual* person)
{
      Individual* clone = (Individual*)person;
      List descendants = initializeList(&printIndividual, &dummyDelete, &compareIndividuals);
      populateDescendantList(&descendants, clone);
      char* s = toString(descendants);
      printf("Leaving function %s", s);
      return descendants;
}



          /*****LIST POPULATERS************/

void deleteEvent(void* toBeDeleted)
{
      if(toBeDeleted == NULL){
	  return;
      }
      Event* del = (Event*)toBeDeleted;
      if(del->date != NULL){
	    free(del->date);
	}
      if(del->place != NULL){
	    free(del->place);
	}
      if(del->otherFields.length > 0){
	    ListIterator itr = createIterator(del->otherFields);
	     while(itr.current != NULL)
	      {
	         void* temp = itr.current;
           itr.current = itr.current->next;
           free(temp);
	      }
      }
}

/*In Order to do this you need to parse the string, then conatenat all
*/
int compareEvents(const void* first, const void* second)
{
    if(first == NULL || second == NULL){
	       return -2;
    }
    Event* e1 = (Event*)first;
    Event* e2 = (Event*)second;
    if((doesStringExist(e1->type))&&(doesStringExist(e2->type))){
        if(strcmp(e1->type, e2->type)<0){
          return -1;
        }else if(strcmp(e1->type, e2->type)>0){
            return 1;
        }else{
            return 0;
        }
    }else if(doesStringExist(e1->type)){
      return -1;
    }else if(doesStringExist(e2->type)){
      return 1;
    }else{
      return 0;
    }
}


char* printEvent(void* toBePrinted)
{
    if(toBePrinted == NULL){
	return NULL;
    }
    Event* printEvent = (Event*)toBePrinted;
    char* eventString = NULL;
    stringAllocater(eventString);
    if(doesStringExist(printEvent->type)){
       strcpy(eventString, printEvent->type);
       strcat(eventString, " ");
    }
    else if(doesStringExist(printEvent->date)){
      strcat(eventString, printEvent->date);
    }else{
      return NULL;
    }
    return eventString;
}






        /*********************FIELD ACCESOR, DESTROYER, COMPARE*******/
void deleteField(void* toBeDeleted)
{

}

int compareFields(const void* a, const void* b)
{
    if(a == NULL || b==NULL){
        return -2;
    }
    Field* first = (Field*)a;
    Field* second = (Field*)b;
    char* firstValue = NULL;
    stAllocater(&firstValue);
    char* secondValue = NULL;
    stAllocater(&secondValue);
    strcpy(firstValue, first->tag);
    strcat(firstValue, " ");
    strcat(firstValue, first->value);
    strcpy(secondValue, second->tag);
    strcat(secondValue, " ");
    strcat(secondValue, second->value);
    if(strcmp(firstValue, secondValue)<0){
      freeString(firstValue);
      freeString(secondValue);
      return -1;
    }
    else if(strcmp(firstValue, secondValue)>0){
      freeString(firstValue);
      freeString(secondValue);
      return 1;
    }
    else{
      freeString(firstValue);
      freeString(secondValue);
      return 0;
    }
}

char* printField(void* toBePrinted)
{
    if(toBePrinted == NULL){
      return NULL;
    }
    Field* theField = (Field*)toBePrinted;
    char* printString = stringCopy(theField->tag);
    strcat(printString, " ");
    strcat(printString, theField->value);
    return printString;
}






        /*********INDIVIDUAL TO-STRING COMPARERS AND DESTROYER*********/

int compareIndividuals(const void* a, const void* b)
{
    Individual* first = NULL;
    Individual* second = NULL;
    if(a == NULL || b == NULL){
        return -2;
    }
    first = (Individual*)a;
    second = (Individual*)b;
    if(strequal(first->givenName, second->givenName)&& strequal(first->surname, second->surname)){
         return 0;
    }
    else{
       return -1;
    }
}


        /*********DESTROYER FOR INDIVIDUAL*********/
void deleteIndividual(void* a)
{
    if(a == NULL)return;
    Individual* destroyer = (Individual*)a;
    if(doesStringExist(destroyer->givenName)){
      freeString(destroyer->givenName);
    }
    if(doesStringExist(destroyer->surname)){
        freeString(destroyer->surname);
    }
    clearList(&(destroyer->events));
    clearList(&(destroyer->families));
    clearList(&(destroyer->otherFields));
    free(destroyer);
}

char* printIndividual(void* toBePrinted)
{
    if(toBePrinted==NULL)return NULL;
    Individual* indi = (Individual*)toBePrinted;
    char* printer = (char*)calloc(800000, sizeof(char));
    if(doesStringExist(indi->givenName)&&(doesStringExist(indi->surname))){
        myStringCopy(&printer, &indi->givenName);
        strcat(printer, " ");
        strcat(printer, indi->surname);
        return printer;
    }
    else if(doesStringExist(indi->givenName)){
      myStringCopy(&printer, &indi->givenName);
      return printer;
    }
    else if(doesStringExist(indi->surname)){
      myStringCopy(&printer, &indi->surname);
      return printer;
    }
    else{
       char* noName = NULL;
       stAllocater(&noName);
       strncpy(noName, "No Name Available", strlen("No Name Available")+1);
       myStringCopy(&printer, &noName);
       freeString(noName);
       return printer;
    }
}



            /************COMPARER DESTROYER AND PRINT FOR FAMILIES***********/
int compareFamilies(const void* a, const void* b)
{
    if(a ==NULL || b == NULL)return -2;
    Family* f1 = (Family*)a;
    Family* f2 = (Family*)b;
    int num1 = 0;
    int num2 = 0;
    if(f1->wife != NULL){
      num1++;
    }
    if(f2->wife != NULL){
      num2++;
    }
    if(f1->husband != NULL){
      num1++;
    }
    if(f2->husband != NULL){
      num2++;
    }
    if(f1->children.length > 0){
        num1 = num1 + f1->children.length;
    }
    if(f2->children.length > 0){
      num2 = num2 + f2->children.length;
    }



    if(num1 < num2){
      return -1;
    }else if(num1 >num2){
      return 1;
    }else {
      return 0;
    }
}

void deleteFamily(void* a)
{
    if(a == NULL)return;
    Family* destroyer = (Family*)a;
    if(destroyer->wife != NULL){
        deleteIndividual(destroyer->wife);
    }
    if(destroyer->husband != NULL){
      deleteIndividual(destroyer->husband);
    }
    Node* itr = destroyer->children.head;
    Node* temp = NULL;
    for(int i = 0; i < destroyer->children.length; i++){
        if(itr == NULL)break;
        deleteIndividual(itr->data);
        temp = itr;
        itr = itr->next;
        free(temp);
    }
    itr = destroyer->events.head;
    temp = NULL;
    for(int i = 0; i < destroyer->events.length; i++){
        if(itr == NULL)break;
        deleteEvent(itr->data);
        temp = itr;
        itr = itr->next;
        free(temp);
    }
    itr = destroyer->otherFields.head;
    temp = NULL;
    for(int i = 0; i < destroyer->otherFields.length; i++){
        if(itr == NULL)break;
        deleteField(itr->data);
        temp = itr;
        itr = itr->next;
        free(temp);
    }
    free(destroyer);
}

char* printFamily(void* a)
{
    if(doesObjectExist(a) == false)
      return NULL;

    char* familyString = NULL;
    stAllocater(&familyString);
    Family* theFamily = (Family*)a;
    if(theFamily->husband != NULL && theFamily->wife != NULL && theFamily->children.length > 0){
         strcpy(familyString, printIndividual(theFamily->husband));
         strcat(familyString, " ");
         strcat(familyString, printIndividual(theFamily->wife));
         Node* itr = theFamily->children.head;
         while(itr != NULL){
             strcat(familyString, " ");
             Individual* temp = (Individual*)itr->data;
             strcat(familyString, printIndividual(temp));
             itr = itr->next;
         }
         return familyString;
    }
    else if(theFamily->husband != NULL){
          strcpy(familyString, printIndividual(theFamily->husband));
    }
    else if(theFamily->wife != NULL){
        strcpy(familyString, printIndividual(theFamily->wife));
    }
    else {
        strcpy(familyString, "No members in this family");
    }
    return familyString;
}



void deleteGEDCOM(GEDCOMobject* obj)
{
     ListIterator itr = createIterator(obj->individuals);
     ListIterator itr2 = createIterator(obj->families);
     while(itr.current != NULL){
         Individual* temp = (Individual*)nextElement(&itr);
         clearList(&(temp->events));
         clearList(&(temp->otherFields));
     }
     while(itr.current != NULL){
          Family* temp = (Family*)nextElement(&itr2);
          clearList(&(temp->otherFields));
          clearList(&(temp->events));
     }
     clearList(&(obj->header->otherFields));
     clearList(&(obj->submitter->otherFields));
     //clearList(&(obj->individuals));
     //clearList(&(obj->families));
     deleteHeader(obj->header);
     deleteSubmitter(obj->submitter);
     free(obj);
}
