#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <ctype.h>

#include "LinkedListAPI.h"
#include "GEDCOMparser.h"
#include "GEDCOMutilities.h"
#include "macros.h"

#define d printf("CHECK\n");

#define DEBUG false
	/*******MACRO FRO HELPER FUNCTIONS PRINT STATEMENTS*********/
#define DEBUGH false

	/**********MACRO FOR DEBUGGING GEDLINE CONSTRUCTOR*********/
#define DEBUGC false
#define DEBUGF false
#define DEBUGA false
#define DEBUGG false
#define DEBUGGEDLINE false
#define DEBUGSTREQUAL false
#define DEBUGLINKER false
	/*   NUMBER OF LINES IN THE FILE*/

int NUM_LINES = 0;

	/** 	*********** SET OF TAGS AND SUBSETS OF TAGS***********/
char* ALL_TAGS[] = {
    "ABBR", "ADDR", "ADR1", "ADR2", "ADOP", "AFN", "AGE", "AGNC",
    "ALIA", "ANCE", "ANCI", "ANUL", "ASSO", "AUTH", "BAPL", "BAPM",
    "BARM", "BASM", "BIRT", "BLES", "BURI", "CALN", "CAST", "CAUS", "CENS",
    "CHAN", "CHAR", "CHIL", "CHR", "CHRA", "CITY",
    "CONC", "CONF", "CONL", "CONT", "COPR", "CREM", "CTRY",
    "DATA", "DATE", "DEAT", "DESC", "DESI", "DEST", "DIV", "DIVF",
    "DSCR", "EDUC", "EMAI", "EMIG", "ENDL", "ENGA", "EVEN", "FACT",
    "FAM", "FAMC", "FAMF", "FAMS", "FAX", "FCOM", "FILE", "FORM", "FONE",
    "GEDC", "GIVN", "GRAD", "HEAD", "HUSB", "IDNO", "IMMI", "INDI", "LANG",
    "LATI", "LONG", "MAP", "MARB", "MARC", "MARL", "MARR", "MARS", "MEDI",
    "NAME", "NATI", "NATU", "NCHI", "NICK", "NMR", "NOTE", "NPFX", "NSFX",
    "OBJE", "OCCU", "ORDI", "ORDN", "PAGE", "PEDI", "PHON", "PLAC", "POST",
    "PROB", "PROP", "PUBL", "QUAY", "REFN", "RELA", "RELI", "REPO", "RESI",
    "RESN", "RETI", "RFN", "RIN", "ROLE", "ROMN", "SEX", "SLGC", "SLGS",
    "SOUR", "SPFX", "SSN", "STAE", "STAT", "SUBM", "SUBN", "SURN", "TEMP",
    "TEXT", "TIME", "TITL", "TRLR", "TYPE", "VERS", "WIFE", "WILL", "WWW"};

		/************SUBSET OF INDIVIDUAL TAGS**********/
char* IND_FACTS[] = {"RESN", "SEX", "SUBM", "ALIA", "AFN", "FAMC", "FAMS", "HUSB", "WIFE"};


char* INDI_EVENTS[] = {"BIRT", "CHR", "DEAT", "BURI", "CREM", "ADOP", "BAPM", "BARM",
                      "BASM", "BLES", "CHRA", "CONF", "FCOM", "ORDN",
                      "NATU", "EMIG", "IMMI", "CENS", "PROB", "WILL", "GRAD",
                      "RETI", "EVEN"};


                        /***********FAMILY TAGS****************/
char* FAM_EVENTS[] = {"ANUL", "CENS", "DIV", "DIVF", "ENGA", "MARB", "MARC", "MARR",
                      "MARL", "MARS", "RESI"};

          /*********SET OF HEADER TAGS*************/
char* REQUIRED_HEADER_TAGS[] = {"0 HEAD", "1 SOUR", "1 SUBM", "1 GEDC", "2 VERS", "2 FORM", "1 CHAR"};
char* ALLOWED_HEADER_LINES[] = {"0 HEAD", "1 SOUR", "2 VERS", "2 NAME", "2 CORP", "2 DATA", "3 DATE", "3 CORP", "3 DATE", "3 CORP", "4 CONT", "4 CONC", "1 DEST", "1 DATE", "2 TIME", "1 SUBM", "1 SUBN", "1 FILE", "1 COPR", "1 GEDC", "2 VERS", "2 FORM", "1 CHAR", "2 VERS", "1 LANG", "1 PLAC", "2 FORM", "1 NOTE", "2 CONC", "2 CONT"};


	/***************TAGS FOR FAMILY EVENTS*******************/




/***************FILE OPENING FUNCTONS***************/

  FILE* openFile(char* fileName){
    if(fileName == NULL){
      return NULL;
    }
    if(strlen(fileName)==0){
      return NULL;
    }
    FILE* FP = (FILE*)calloc(100, sizeof(FILE));
    FP = fopen(fileName, "r");
    if(FP==NULL)return NULL;
    rewind(FP);
    return FP;
}




char** returnFile(FILE* FP, int* lineNumber)
{
    if(FP==NULL){
      return NULL;
    }
     char** fileLines = NULL;
     fileLines = stringarrayAllocater(fileLines);
    for(int j = 0; j < 1000; j++){
      if(doesStringExist(fileLines[j])){
        removeHardReturn(fileLines[j]);
        removeLeadingWhiteSpace(fileLines[j]);
        removeTrailingWhiteSpace(fileLines[j]);
      }
    }
    int i = 0;
    while(!feof(FP)){
        fgets(fileLines[i], 300, FP);
        removeLeadingWhiteSpace(fileLines[i]);
        removeHardReturn(fileLines[i]);
        i++;
       *lineNumber = i;
    }
    return fileLines;
}


      /************CLOSES FILE************/
void fileCloser(FILE* FP)
{
    if(FP == NULL)return;
      fclose(FP);
}


    /***********READ THE FILE*************/
char* readFile(char* buffer, int max, FILE* FP)
{

    int c;
    char* buff = buffer;
    for(buff = buffer, max--; max>0; max--){
        c = fgetc(FP);
        if(c == EOF)break;
        *buff++ = c;
        if(*buff == 10 || *buff == (13)){
           break;
        }
    }
    *buff = '\0';
    if(buff == buffer || c == EOF)return NULL;
    return buff;
}

/*******FUNCTIONS TO CHECK FOR VALIDITY********/



bool strequal(const void* first, const void* second){
    if(first == NULL || second == NULL){
	return false;
    }
    char* a = (char*)first;
    char* b = (char*)second;
    if(DEBUGSTREQUAL)printf("%s %s\n\n", a, b);
    if(strcmp(a, b)== 0){
       if(DEBUGSTREQUAL)printf("retruning true\n");
	     return true;
    }
    else{
      if(DEBUGSTREQUAL)printf("Returing false\n");
	     return false;
    }
}


/**
    CHECK IF TAG IS VALID
*/
bool validTag(char* s){
    if (s == NULL)return false;
    if(strlen(s)==0)return false;
    for(int i = 0; i < NUM_TAGS; i++){
	       if(strequal(s, ALL_TAGS[i])){
           if(DEBUGH)printf("It returns true\n");
	          return true;
	       }
    }
    return false;

}

      /*******CHECK IF IT IS A VALID INDIVIDUAL EVENT********/


bool validIndiEventTag(char* s)
{
    if(doesStringExist(s)==false)return false;
    for(int i = 0; i < NUM_INDI_EVENTS; i++){
      if(strequal(s, INDI_EVENTS[i])){
         return true;
      }
    }
  return false;
}

bool validFamEventTag(char* s)
{
  if(doesStringExist(s)==false)return false;
  for(int i = 0; i < NUM_FAM_EVENTS; i++){
    if(strequal(s, FAM_EVENTS[i])){
      return true;
    }
  }
  return false;
}

bool validNumber(char* s)
{
   if(strlen(s) > 2){
	return false;
   }
   if(strlen(s) == 2){
	if(!isdigit(s[0])){
	    return false;
	}
	if(s[0] == '0'){
	    return false;
	}
   }
   if(strlen(s)==1){
	if(!isdigit(s[0])){
	    return false;
	}
   }
   return true;
}
/**
    function to check if a string is an XREF_ID
***/
bool validXREFid(char* s)
{
   if(doesStringExist(s)==false)return false;

   if((s[0] == '@') && (s[strlen(s)-1]== '@')){
	    return true;
   }
   return false;
}


bool validFileName(char* s)
{
    if(s[strlen(s)-4]!= '.'){
      return false;
    }
    if(s[strlen(s)-3]!= 'g'){
      return false;
    }
    if(s[strlen(s)-2]!= 'e'){
      return false;
    }
    if(s[strlen(s)-1]!= 'd'){
      return false;
    }
    return true;
}

bool validPointer(char* s)
{
   if(s==NULL){
	return false;
   }
   if(strlen(s)<=0){
	return false;
   }
   int len = strlen(s);
   if(len <= 2){
	return false;
   }
   if((s[0] != '@') || (s[len-1]!= '@')){
	return false;
   }
   for(int i = 1; i < len-2; i++){
	    if(!(isdigit(s[i])) || !(isalpha(s[i]))){
	       return false;
	    }
   }
   return true;

}



            /***************VALIDATE THE GEDCOM*****************/


GEDCOMerror generalValidator(List* gedLineList, HeaderCopy* h, SubmitterCopy* s, List* iCopyList, List* fCopyList, int* n, GEDCOMerror* e)
{
    GEDCOMerror gedErr;
    if(validLevelNumbers(gedLineList, n)==false){
        e->type = INV_RECORD;
        e->line = *n;
        return gedErr;
    }
    if(validGEDCOM(gedLineList)==false){
        e->type = INV_GEDCOM;
        e->line = -1;
        return gedErr;
    }
    if(validHeader(h)==false){
        e->type = INV_HEADER;
        e->line = -1;
        return gedErr;
    }
    //need to fix this
    if(validSubmitter(s)==false){
        e->type = INV_RECORD;
        e->line = -1;
        return gedErr;
    }
    if(validTrailer(gedLineList)==false){
        e->type = INV_GEDCOM;
        e->line = -1;
        return gedErr;
    }
    e->type = OK;
    e->line = -1;
    return gedErr;
}





bool validLevelNumbers(List* l, int* lineNumber)
{
    int currVal = 0;
    int prevVal = 0;
    Node* itr = l->head;
    int i = 0;
    while(doesObjectExist(itr)){//start while
        GEDline* temp = (GEDline*)itr->data;

        if(i == 0){
            currVal = prevVal = atoi(temp->level);
            if(currVal != 0){//
               *lineNumber = -1;
                return false;
            }
        }//end if
        else{//else
            currVal = atoi(temp->level);
            if(currVal > (prevVal + 1)){
                 *lineNumber = temp->lineNumber;
                 return false;
            }
            if(currVal > 99 || currVal < 0){
                 *lineNumber = temp->lineNumber;
                 return false;
            }
        }//end else
        prevVal = currVal;
        i++;
        itr = itr->next;

    }//end while

    return true;
}




bool validHeader(HeaderCopy* h)
{
   if(doesObjectExist(h)==false)return false;
   if(doesStringExist(h->submitterTag)==false){
        return false;
   }
   if(doesStringExist(h->source)==false){
       return false;
   }
   if(h->gedcVersion == -1.0){
       return false;
   }
   return true;
}



bool validSubmitter(SubmitterCopy* s)
{
    if(doesObjectExist(s)==false)return false;
    if(doesStringExist(s->submitterName)==false)return false;
    if(doesStringExist(s->pointer)==false)return false;
    return true;

}



bool validTrailer(List* l)
{
    GEDline* trailer = getFromBack(*l);
    if(trailer == NULL)return false;
    if(doesStringExist(trailer->tag)==false)return false;
    if(strequal(trailer->tag, "TRLR")==false)return false;
    if(atoi(trailer->level) != 0)return false;
    return true;
}



bool validGEDCOM(List* l)
{
    if(l==NULL)return false;
    Node* itr = l->head;
    bool headX = false;
    bool trailerX = false;
    bool subX = false;

    while(doesObjectExist(itr)){//start while
      GEDline* temp = (GEDline*)itr->data;
      if(doesStringExist(temp->tag)){//start if
        if(strequal(temp->tag, "HEAD")){//start if
          headX = true;
        }
        if(strequal(temp->tag, "TRLR")){
            trailerX = true;
        }//end if
        if(strequal(temp->tag, "SUBM")){
          subX = true;
        }//edn if
      }//end of
      itr = itr->next;
    }//end while
    if(headX == true && trailerX == true && subX == true){
        return true;
    }
    return false;
}



      /***********END GEDCOM VALIDATORS*******************/
bool allCaps(char* s)
{
    for(int i = 0; i < strlen(s); i++){
	if(!(isupper(s[i]))){
	    return false;
	}
    }
   return true;
}



bool doesStringExist(char* s)
{
   if(s==NULL){
	    return false;
   }
   if(strcmp(s, "")==0){
	    return false;
   }
   return true;
}



bool doesObjectExist(void* a)
{
   if(a == NULL)return false;
   return true;
}





/**
	       STRING MODIFIERS
**/

void myStringCopy(char** dest, char** source)
{
     if(dest == NULL || source == NULL)return;
     if(*dest == NULL || *source == NULL)return;
     strncpy(*dest, *source, strlen(*source)+1);
}


void stringTok(char** array, char* s, char* delims)
{
    if(doesStringExist(s)==false || doesStringExist(delims)==false)return;
    if(doesObjectExist(array)==false)return;
    int numberOfDelims = strlen(delims);
    int counter = 0;
    int character = 0;
    for(int i = 0; i < strlen(s); i++){
      for(int j = 0; j < numberOfDelims; j++){
          if(s[i] != delims[j]){
              array[counter][character] = s[i];
              character++;
          }
          else{
              array[counter][character] = '\0';
              counter++;
              character = 0;
          }

      }
    }

}


char* appendSpace(char* s)
{
	s[strlen(s)] = ' ';
	if(DEBUGC)printf("%s\n", s);
	return s;
}


char* stringcatArray(char** source, int start, int numTimes, char* s)
{
    if(doesStringExist(source[start])==false)return NULL;
	   char* copy = NULL;
	  for(int i = 0; i < numTimes; i++){
		    if(i == 0){
			  copy = stringCopy(source[i]);
		    }else{
			       if(DEBUGC)printf("COPY IS %s\n", copy);
			       appendSpace(copy);
			       strncat(copy, source[i], strlen(source[i])+1);
		   }
	  }

    if(DEBUGC)printf("CONCATENATED STRING IS %s\n", copy);
    strcpy(s, copy);
    return copy;
}




char* emptyString()
{
    char* s;
    s = "";
    return s;
}



int stringToInt(void* data)
{
    if(data == NULL){
	return -1;
    }
    char* s = (char*)data;
    int lineNum = atoi(s);
    return lineNum;
}



char** tokenizeString(char* s, int arraySize)
{
    if(s == NULL){
		    return NULL;
    }
    if(arraySize == 0){
	     return NULL;
    }
     int i = 0;
     char** arrayOfStrings = NULL;
    arrayOfStrings = stringarrayAllocater(arrayOfStrings);
    char* tempString = NULL;
    stAllocater(&tempString);
    strncpy(tempString, s, strlen(s)+1);
    char* s2 = NULL;
    stAllocater(&s2);
    s2 = strtok(tempString, " ");
    myStringCopy(&arrayOfStrings[i], &s2);
    removeHardReturn(arrayOfStrings[i]);
    for(int i = 1; i < arraySize; i++){
	    s2 = strtok(NULL, " ");
      myStringCopy(&arrayOfStrings[i], &s2);
	    //arrayOfStrings[i] = stringCopy(s2);

      removeHardReturn(arrayOfStrings[i]);
    }
    //freeString(tempString);
    //freeString(s2);
    return arrayOfStrings;
}



int getNumTokens(char* s)
{
   if(s == NULL){
	return -1;
   }
   if(strlen(s) <= 0){
	return -1;
   }

   char* tempString = stringCopy(s);
   int numTokens = 0;
   int i = 0;
   char* s2 = stringCopy(tempString);
   s2 = strtok(tempString, " ");
   while(s2 != NULL){
	   if(DEBUGH)printf("Inside loop\n%s", s2);
	    s2 = strtok(NULL, " ");
	    i++;

   }
   //free(tempString);
   //free(s2);
   numTokens = i;
   if(DEBUGH)printf("NUMBER OF TOKENS IS %d\n", numTokens);
   return numTokens;
}


int getNumNames(char* s)
{
    if(s == NULL){
      return -1;
    }
    if(strlen(s)==0){
      return -1;
    }
    char* tempString = stringCopy(s);
    int numTokens = 0;
    int i = 0;
    char* s2 = stringCopy(tempString);
    s2 = strtok(tempString, "' '/");
    while(s2 != NULL){
 	    s2 = strtok(NULL, "' '/");
 	    i++;

    }
    numTokens = i;
    return numTokens;
}


int getNumChildren(char** s)
{
    if(doesObjectExist(s)==false)return -1;
    int numChildren = 0;
    int i = 0;
    if(!doesStringExist(s[0])){
      numChildren = 0;
      return numChildren;
    }
    while(doesStringExist(s[i])){
        if(s[i][0] == '@'){
          numChildren++;
          i++;
        }
    }
    return numChildren;
}
/**********FUNCTION TO COPY AN ARRAY OF STRINGS***********/
char** stringarrayCopy(char** array, int start, int end)
{
	if(DEBUGA)printf("INSIDE STRING ARRAYCOPY FUNCTION\n");
	if(DEBUGA)printf("start %d end %d\n",start, end);
    char** copy = NULL;
    copy = stringarrayAllocater(copy);
    int stop = end - start;
    int counter = start;
    for(int i = 0; i < stop; i++){
	    if(DEBUGG)printf("Origina l string is %d string is %s\n", i, array[counter]);
      //stAllocater(&copy[i]);
      myStringCopy(&copy[i], &array[counter]);
	    //copy[i] = stringCopy(array[counter]);
	    counter++;
	    if(DEBUGG)printf("copy of string array is string %s\n", copy[i]);
    }
    if(DEBUGC)printf("RETURNING FROM FUNC STRINGARRAYCOPY\n");
    return copy;
}






/*****************FUNCTION TO COPY A STRING***********/
char* stringCopy(char* original)
{
    char* clone = NULL;
    if(original == NULL){
		    clone = (char*)calloc(500, sizeof(char)+10);
        clone = "";
		return clone;
    }
    else{
         clone = (char*)calloc(500, sizeof(char)+10);
		       for(int i = 0; i < strlen(original); i++){
				         clone[i] = original[i];
		       }
    }
    clone[strlen(clone)]= '\0';
    if(DEBUGH)printf("clone address = %p\noriginal address = %p\n", clone, original);
    return clone;
}



        /*********BASIC ALLOCATERS AN FREE FUNCTIONS*************/
char* stringAllocater(char* s)
{
  s = (char*)calloc(500, sizeof(char));
  if(s == NULL){
    printf("Calloc failed\n");
    return NULL;
  }
  return s;
}



char** stringarrayAllocater(char** s)
{

    s = (char**)calloc(1000, sizeof(char*));
    if(s!=NULL){
      if(DEBUGC)printf("S is not NULL\n");
    }

    for(int i = 0; i < 1000; i++){
       s[i] = (char*)calloc(500, sizeof(char));
      if((s[i])==NULL){
           //printf("Calloc failed\n");
           return NULL;
      }
    }
    return s;
}



void stAllocater(char** s)
{
  *s = (char*)calloc(500, sizeof(char));
}

void strArrAllocater(char*** s)
{
    *s = (char**)calloc(500, sizeof(char*));
    for(int i = 0; i < 500; i++){
       s[i] = NULL;
       stAllocater(s[i]);
    }
}




      /***************BASIC DESTROYERS***********/
void freeString(char* s)
{
  free(s);
  s = NULL;
}

void freeStringArray(char** s)
{
   int i = 0;
   if(s==NULL){
     return;
   }
    while(doesStringExist(s[i])){
      freeString(s[i]);
      i++;
    }
    free(s);
    s = NULL;
}
/*********************FUNCTION TO REMOVE WHITE SPACE****************/

void removeHardReturn(char* s)
{
    for(int i = 0; i < strlen(s); i++){
        if(s[i] == 10 || s[i] == 13){
          s[i] = '\0';
        }
    }
}

void removeTrailingWhiteSpace(char* s)
{
      if(doesStringExist(s)==false)return;
      int j = strlen(s)-1;
      while(isspace(s[j])){
        s[j] = '\0';
        j--;
        if(j == 0){
          return;
        }
      }
}

char* removeLeadingWhiteSpace(char* s)
{
    if(s == NULL){
	return NULL;
    }
    if(strlen(s) <= 0){
	return NULL;
    }
    int i = 0;
    while(isspace(s[i])){
	     i++;
    }
    s = s+i;
   if(DEBUGH)printf("%s\n", s);
    return s;
}








/**
	************CONSTRUCTORS************
**/
/**
    Function to create a GEDCOM line
*/

TestGED* createTestGED()
{
      TestGED* newTest = (TestGED*)calloc(1, sizeof(TestGED));
      newTest->header = NULL;
      newTest->submitter = NULL;
      newTest->individuals = createIndividualList(&printIndividual, &deleteIndividual, &compareIndividuals);
      newTest->families = createFamilyList(&printFamily, &deleteFamily, &compareFamilies);
      return newTest;

}

GEDline* createGEDline(char* s, int i)
{
   int numTokens = 0;
   if(doesStringExist(s) == false){
	     return NULL;
   }
   //removeLeadingWhiteSpace(s);
   //removeHardReturn(s);
	 /**********MAKE SURE TO CHECK THIS********/
   GEDline* newGEDline = (GEDline*)calloc(50, sizeof(GEDline));
   newGEDline->level = NULL;
   stAllocater(&newGEDline->level);
   newGEDline->tag = NULL;
   stAllocater(&newGEDline->tag);
   newGEDline->pointer = NULL;
   stAllocater(&newGEDline->pointer);
   newGEDline->ID = NULL;
   stAllocater(&newGEDline->ID);
   newGEDline->value = NULL;
   stAllocater(&newGEDline->value);
   newGEDline->lineNumber = i;
   char* gedLine = NULL;
   stAllocater(&gedLine);
   myStringCopy(&gedLine, &s);

   if(DEBUGC)printf("TESTING MAKE GEDLINE\n");
   numTokens = getNumTokens(s);
   char** theTokens = tokenizeString(gedLine, numTokens);
    if(DEBUGC)printf("TESTING BEGINNING TO MAKED GEDLINE\n");
    if(DEBUGC)printf("%s\n", gedLine);
   for(int i = 0; i < numTokens; i++){
		if(i == 0){
      myStringCopy(&newGEDline->level, &theTokens[i]);
      //strncpy(newGEDline->level, theTokens[i], strlen(theTokens[i]));
      if(DEBUGC)printf("level number is %s\n", newGEDline->level);
		}
		if(i == 1){

			if(validTag(theTokens[i])){
        myStringCopy(&newGEDline->tag, &theTokens[i]);
        //strncpy(newGEDline->tag, theTokens[i], strlen(theTokens[i])+1);
        if(DEBUGC)printf("SUCEEDED\n");
			}else{
        myStringCopy(&newGEDline->pointer, &theTokens[i]);
        //strncpy(newGEDline->pointer, theTokens[i], strlen(theTokens[i]));
        if(DEBUGC)printf("WHY THE FUCK IS HEAD GOING TO POINTER %s\n", newGEDline->pointer);
			}
		}
		if(i == 2){
      if(validTag(theTokens[i])){
        myStringCopy(&newGEDline->tag, &theTokens[i]);
        //strncpy(newGEDline->tag, theTokens[i], strlen(theTokens[i])+1);
      }
      else if(validXREFid(theTokens[i])){
        myStringCopy(&newGEDline->ID, &theTokens[i]);
        //strncpy(newGEDline->ID, theTokens[i], strlen(theTokens[i])+1);
      }else{
			   char** s = stringarrayCopy(theTokens, i, numTokens);
        stringcatArray(s, 0, (numTokens-i), newGEDline->value);
     }
    }
  }
    //freeStringArray(theTokens);
    //freeString(gedLine);
    //freeString(s);
    return newGEDline;
}




      /********************FUNCTION TO CREATE A RECORD************/
Record* createRecord(GEDline* gedLine)
{
  Record* newRecord = (Record*)calloc(6, sizeof(Record));
  newRecord->info = (GEDline**)calloc(50, sizeof(GEDline*));
  newRecord->info[0] = gedLine;
  newRecord->size = 1;
  return newRecord;
}

        /******************FUCNTION TO CREATE AN INDEDIVIUDAL COPY*************/
IndividualCopy* createIndividualCopy(void* gedLine){
    GEDline* gedline = (GEDline*)gedLine;
    IndividualCopy* individualCopy = (IndividualCopy*)calloc(3, sizeof(IndividualCopy));
    individualCopy->pointer = NULL;
    stAllocater(&individualCopy->pointer);
    strncpy(individualCopy->pointer, gedline->pointer, strlen(gedline->pointer)+1);
    individualCopy->type = NULL;
    stAllocater(&individualCopy->type);
    strncpy(individualCopy->type, gedline->tag, strlen(gedline->tag)+1);
    individualCopy->individual = NULL;

    individualCopy->spouseOf = stringarrayAllocater(individualCopy->spouseOf);
    individualCopy->childOf = stringarrayAllocater(individualCopy->childOf);
    individualCopy->givenName = NULL;
    stAllocater(&individualCopy->givenName);
    individualCopy->surname = NULL;
    stAllocater(&individualCopy->surname);

    individualCopy->events = createEventList(&printEvent, &dummyDelete, &compareEvents);
    individualCopy->families = createFamilyList(&printFamily, &dummyDelete, &compareFamilies);
    individualCopy->otherFields = createOtherFieldsList(&printField, &dummyDelete, &compareFields);

    individualCopy->individual = (Individual*)calloc(1, sizeof(Individual));
    individualCopy->individual->givenName = NULL;
    stAllocater(&(individualCopy->individual->givenName));
    individualCopy->individual->surname = NULL;
    stAllocater(&(individualCopy->individual->surname));
    individualCopy->individual->families = createFamilyList(&printFamily, &dummyDelete, &compareFamilies);
    individualCopy->individual->events = createEventList(&printEvent, &dummyDelete, &compareEvents);
    individualCopy->individual->otherFields = createOtherFieldsList(&printField, &dummyDelete, &compareFields);
    return individualCopy;
}

        /**************CREATES AN INDIVIDUAL OBJECT**********/
Individual* createIndividual(void* individualCopy){

    IndividualCopy* temp = (IndividualCopy*)individualCopy;
    Individual* newIndividual = (Individual*)calloc(3, sizeof(Individual));


    newIndividual->givenName = stringAllocater(newIndividual->givenName);
    newIndividual->surname = stringAllocater(newIndividual->surname);
    newIndividual->events = createEventList(&printEvent, &deleteEvent, &compareEvents);
    newIndividual->families = createFamilyList(&printFamily, &dummyDelete, &compareFamilies);
    newIndividual->otherFields = createOtherFieldsList(&printField, &dummyDelete, &compareFamilies);


    if(doesStringExist(temp->givenName)){
        myStringCopy(&newIndividual->givenName, &temp->givenName);

    }
    if(doesStringExist(temp->surname)){
      myStringCopy(&newIndividual->surname, &temp->surname);
    }
    if(temp->events.length > 0){
       Node* itr = temp->events.head;
       while(itr != NULL){
         insertBack(&(newIndividual->events), cloneEvent(itr->data));
         itr = itr->next;
       }
    }
    if(temp->otherFields.length > 0){
       Node* itr = temp->otherFields.head;
       while(itr != NULL){
         insertBack(&(newIndividual->otherFields), cloneField(itr->data));
         itr = itr->next;
       }
    }

    if(temp->families.length>0){
      Node* itr = temp->families.head;
      while(itr != NULL){
        insertBack(&(newIndividual->families), createFamily(itr->data));
        itr = itr->next;
      }
    }
    if(DEBUGF)printf("givenName is %s %s\n", newIndividual->givenName, newIndividual->surname);
    return newIndividual;
}



          /****************CREATE A FAMILY***************/




FamilyCopy* createFamilyCopy(void* gedLine){

      if(gedLine == NULL)return NULL;
      GEDline* g = (GEDline*)gedLine;
      FamilyCopy* newFamily = (FamilyCopy*)calloc(1, sizeof(FamilyCopy));
      newFamily->type = stringCopy(g->tag);
      newFamily->pointer = stringCopy(g->pointer);
      newFamily->husbandLink = NULL;
      stAllocater(&newFamily->husbandLink);
      newFamily->wifeLink = NULL;
      stAllocater(&newFamily->wifeLink);
      newFamily->childrenLinks = NULL;
      newFamily->childrenLinks = stringarrayAllocater(newFamily->childrenLinks);
      newFamily->husband = NULL;
      newFamily->wife = NULL;


      newFamily->family = NULL;
      newFamily->family = (Family*)calloc(1, sizeof(Family));
      newFamily->wife = NULL;
      newFamily->husband = NULL;
      newFamily->family->children = createIndividualList(&printIndividual, &dummyDelete, &compareIndividuals);
      newFamily->family->otherFields = createOtherFieldsList(&printField, &dummyDelete, &compareIndividuals);
      newFamily->events = createEventList(&printEvent, &dummyDelete, &compareEvents);

      newFamily->children = createIndividualList(&printIndividual, &dummyDelete, &compareIndividuals);
      newFamily->otherFields = createOtherFieldsList(&printField, &dummyDelete, &compareFields);
      newFamily->events = createEventList(&printEvent, &dummyDelete, &compareEvents);
      return newFamily;
}

//******FIX THIS SHIT***********/
Family* createFamily(void* familyCopy)
{
    if(familyCopy == NULL){
      return NULL;
    }
    FamilyCopy* temp = (FamilyCopy*)familyCopy;
    Family* newFamily = (Family*)calloc(3, sizeof(Family));

    newFamily->children = createIndividualList(&printIndividual, &dummyDelete, &compareIndividuals);
    newFamily->otherFields = createOtherFieldsList(&printField, &dummyDelete, &compareFields);
    newFamily->events = createEventList(&printEvent, &dummyDelete, &compareEvents);

    if(temp->wife != NULL)
      newFamily->wife = createIndividual(temp->wife);
    if(temp->husband != NULL)
      newFamily->husband = createIndividual(temp->husband);
    if(temp->children.length > 0){
       Node* itr = temp->children.head;
       while(itr != NULL){
         insertBack(&(newFamily->children), createIndividual(itr->data));
         itr = itr->next;
       }
    }
    if(temp->events.length > 0){
       Node* itr = temp->events.head;
       while(itr != NULL){
         insertBack(&(newFamily->events), cloneEvent(itr->data));
         itr = itr->next;
       }
    }
    if(temp->otherFields.length > 0){
       Node* itr = temp->otherFields.head;
       while(itr != NULL){
         insertBack(&(newFamily->otherFields), cloneField(itr->data));
         itr = itr->next;
       }
    }
    return newFamily;
}


          /********CONSTRUCTOR FOR A FIELD*********/
Field* createField(void* tag, void* value)
{
    if(tag == NULL || value == NULL)return NULL;
    char* tg = (char*)tag;
    char* vl = (char*)value;
    Field* newField = (Field*)calloc(3, sizeof(Field));
    newField->tag = NULL;
    stAllocater(&newField->tag);
    newField->value = NULL;
    stAllocater(&newField->value);
    myStringCopy(&newField->tag, &tg);
    myStringCopy(&newField->value, &vl);
    return newField;
}




        /********CONSTRUCTOR FOR AN EVENT*********/
Event* createEvent(void* type, void* date, void* place)
{
    if(type == NULL || date == NULL || place == NULL)return NULL;
    Event* newEvent = (Event*)calloc(5, sizeof(Event));
    GEDline* typeLine = (GEDline*)type;
    GEDline* dateLine = (GEDline*)date;
    GEDline* placeLine = (GEDline*)place;

    newEvent->otherFields = createOtherFieldsList(&printField, &dummyDelete, &compareFields);

    stAllocater(&newEvent->date);
    stAllocater(&newEvent->place);

    strcpy(newEvent->type, typeLine->tag);
    strncpy(newEvent->date, dateLine->value, strlen(dateLine->value)+1);
    strncpy(newEvent->place, placeLine->value, strlen(placeLine->value)+1);
    return newEvent;
}




Header* createHeader(void* h)
{
    HeaderCopy* temp = (HeaderCopy*)h;
    if(temp == NULL)return NULL;

    Header* newHeader = (Header*)calloc(3, sizeof(Header));
    newHeader->otherFields = createOtherFieldsList(&printField, &deleteField, &compareFields);

    if(doesStringExist(temp->source)){
      strncpy(newHeader->source, temp->source, strlen(temp->source)+1);
    }
    newHeader->gedcVersion = temp->gedcVersion;
    newHeader->encoding  = temp->encoding;
    if(doesObjectExist(temp->submitter)){
      newHeader->submitter = temp->submitter;
    }


    if(temp->otherFields.length>0){
      Node* itr = temp->otherFields.head;
      while(itr != NULL){
          insertBack(&(newHeader->otherFields), itr->data);
          itr = itr->next;
      }
    }
    return newHeader;
}


HeaderCopy* createHeaderCopy(){

  HeaderCopy* newHeaderCopy = (HeaderCopy*)calloc(2, sizeof(HeaderCopy));
  newHeaderCopy->otherFields = createOtherFieldsList(&printField, &dummyDelete, &compareFields);
  return newHeaderCopy;

}


Submitter* createSubmitter(void* submitterCopy)
{
  if(submitterCopy == NULL)return NULL;

  SubmitterCopy* temp = (SubmitterCopy*)submitterCopy;
  Submitter* newSubmitter = (Submitter*)calloc(1, sizeof(Submitter)*sizeof(char)+256);
  newSubmitter->otherFields = initializeList(printField, dummyDelete, compareFields);
  if(doesStringExist(temp->submitterName)){
    strncpy(newSubmitter->submitterName, temp->submitterName, strlen(temp->submitterName)+1);
  }
  if(temp->otherFields.length > 0){
      Node* itr = temp->otherFields.head;
      while(itr != NULL){
        if(itr == NULL){
          break;
        }
        insertBack(&(newSubmitter->otherFields), itr->data);
        itr = itr->next;
      }
  }
  if(doesStringExist(temp->address)){
    strncpy(newSubmitter->address, temp->address, strlen(temp->address)+1);

  }
  return newSubmitter;
}

SubmitterCopy* createSubmitterCopy()
{

    SubmitterCopy* submitterCopy = (SubmitterCopy*)calloc(1, sizeof(SubmitterCopy)+(sizeof(char)*256));
    submitterCopy->otherFields = createOtherFieldsList(&printField, &dummyDelete, &compareFields);
    return submitterCopy;
}







              /****************MUTATORS*****************/
void appendValue(List gedList)
{
    Node* itr = gedList.head;
    GEDline* current = cloneGEDline((GEDline*)itr->data);
    for(int i = 0; i < gedList.length; i++){
        if(strequal(current->tag, "CONT")==true){
            if(DEBUGH)printf("Appending with CONT tag\n");
            GEDline* prev = (GEDline*)itr->previous->data;
            strcat(prev->value, "\n");
            strcat(prev->value, " ");
            strncat(prev->value, current->value, (strlen(current->value)+1));
        }
        if(strequal(current->tag, "CONC")==true){
           if(DEBUGH)printf("Appending with CONC tag\n");
            GEDline* prev = (GEDline*)itr->previous->data;
            strcat(prev->value, " ");
            strncat(prev->value, current->value, (strlen(current->value)+1));
            if(DEBUGH)printf("NEW VALUE is %s\n", prev->value);

        }
        itr = itr->next;
        if(itr == NULL){
          return;
        }
        free(current);
        current = cloneGEDline((GEDline*)itr->data);
    }
}






void individualCopySetName(void* i, char* name)
{
    IndividualCopy* indi = (IndividualCopy*)i;
    if(doesStringExist(name)==false){
      return;
    }
    if(indi == NULL)return;
    char* temp = NULL;
    stAllocater(&temp);
    strcpy(temp, name);
    int firstSpace = 0;
    for(int i = 0; i < strlen(temp); i++){
        if(temp[i]== 47){
          firstSpace = i;
          break;
        }
    }
    for(int j = 0;j < firstSpace; j++){
        indi->givenName[j] = temp[j];
    }
    removeTrailingWhiteSpace(indi->givenName);
    strcpy(indi->individual->givenName, indi->givenName);
    int k = 0;
    for(int i = firstSpace+1; i < strlen(temp); i++){
        if(temp[i]==47){
          i++;
          continue;
        }
        indi->surname[k] = temp[i];
        k++;
    }
    removeTrailingWhiteSpace(indi->surname);
    strcpy(indi->individual->surname, indi->surname);
}


char** tokenizeName(char* name)
{
     if(name == NULL)return NULL;
    char** theName = NULL;
    int counter = 0;
    int j = 0;
  theName = stringarrayAllocater(theName);

    for(int i = 0; i < strlen(name); i++){
        if(name[i] == 47){
          theName[counter][j+1] = '\0';
          counter++;
          j = 0;
        }
        else{
          theName[counter][j] = name[i];
          j++;
        }
    }
    return theName;
}



            /*************FUNCTIONS TO SET THE INDIVIDUALS FAMILY STATUS********/
void individualCopySetSpouseStatus(void* i, char* spouseOf, int familyNumber)
{
  IndividualCopy* indi = (IndividualCopy*)i;
  char* h = NULL;
  if(doesStringExist(spouseOf)==false){
    indi->spouseOf[familyNumber] = "";
    return;
  }
    stAllocater(&indi->spouseOf[familyNumber]);
    h = stringCopy(spouseOf);
    strncpy(indi->spouseOf[familyNumber], h, strlen(h)+1);
    free(h);
}


void individualCopySetChildStatus(void* i, char* childOf, int familyNumber)
{
    IndividualCopy* indi = (IndividualCopy*)i;
    if(doesStringExist(childOf)==false){
      indi->childOf[familyNumber] = "";
      return;
    }
    indi->childOf[familyNumber] = stringCopy(childOf);
}




      /******MAY NEED TO CHANGE PARAMETER TO EVENT COPY**********/
void individualCopySetEvent(void* i, Event* e)
{
    IndividualCopy* indi = (IndividualCopy*)i;
    insertBack(&(indi->events), e);
}



            /***********FAMILY MUTATORS********************/

void familyCopySetHusband(void* f, char* husband)
{
    FamilyCopy* fam = (FamilyCopy*)f;
    if(fam == NULL)return;
    if(doesStringExist(husband)== false)return;
    strncpy(fam->husbandLink, husband, strlen(husband)+1);
}

void familyCopySetWife(void* f, char* wife)
{
    FamilyCopy* fam = (FamilyCopy*)f;
    if(fam == NULL)return;
    if(doesStringExist(wife)==false)return;
    strncpy(fam->wifeLink, wife, strlen(wife)+1);
}


void familyCopySetChild(void* f, char* child, int numChild)
{
    if(f == NULL)return;
    FamilyCopy* fam = (FamilyCopy*)f;
    if(fam == NULL)return;
    if(doesStringExist(child)==false)return;
    stAllocater(&fam->childrenLinks[numChild]);
    strncpy(fam->childrenLinks[numChild], child, strlen(child)+1);
}



          /**************RECORD MUTATOR*************/
void setRecord(Record* record, GEDline* gedLine, int count)
{
    record->info[count] = gedLine;
    record->size = record->size+1;
}



          /**************STRING COPIERS**************/


void printArrayOfStrings(char** s)
{

    for(int k = 0; k < 6; k++)
	  if(DEBUG)printf("%s\n", s[k]);
}

int getLineSize(char* s)
{

    //int totalLength = strlen(s);
    int i = 0;
    while(isspace(s[i])){
	     i++;
    }
    int finalLength = strlen(s)-i;
    if(DEBUG)printf("final length is %d\n", finalLength);
    //int difference = totalLength - i;
    if(finalLength>(MAX_LEN)){

	return -1;
    }
    else {
	return finalLength;
    }
}
	/****************************CLONERS*******************/






Field* cloneField(void* original)
{
      if(original == NULL)return NULL;
      Field* clone = (Field*)calloc(9, sizeof(*clone));
      Field* temp = (Field*)original;
      if(doesStringExist(temp->tag)){
        clone->tag = NULL;
        stAllocater(&clone->tag);
        myStringCopy(&clone->tag, &temp->tag);
      }
      if(doesStringExist(temp->value)){
        clone->value = NULL;
        stAllocater(&clone->value);
        myStringCopy(&clone->value, &temp->value);
      }
      return clone;
}







Event* cloneEvent(void* original)
{
   if(original == NULL){
	    return NULL;
   }
   Event* eventClone = (Event*)calloc(10, sizeof(Event));
   Event* temp = (Event*)original;
   eventClone->otherFields = createOtherFieldsList(&printField, &dummyDelete, &compareFields);

   if(doesStringExist(temp->type)){
      strcpy(eventClone->type, temp->type);
   }
   if(doesStringExist(temp->date)){
       eventClone->date = NULL;
       stAllocater(&eventClone->date);
       myStringCopy(&eventClone->date, &temp->date);
   }
   if(doesStringExist(temp->place)){
      eventClone->place = NULL;
      stAllocater(&eventClone->place);
      myStringCopy(&eventClone->place, &temp->place);
   }
   if(temp->otherFields.length > 0){
       Node* itr = temp->otherFields.head;
       void* data = NULL;
       while(itr != NULL){
            data = itr->data;
            insertBack(&(eventClone->otherFields), cloneField(data));
            itr = itr->next;
       }
   }
   return eventClone;
}






void cloneFamilyList(List* dest, List* source)
{
    Node* itr = source->head;
    void* a = itr;
    printf("SOURCE HEAD IS %p\n", a);
    Node* start = source->head;
    while(itr != NULL){
        insertBack(dest, cloneFamily(itr->data));
        printf("CLONING LIST\n");
        itr = itr->next;
    }
    source->head = start;
}


void cloneIndividualList(List* dest, List* source)
{
    Node* itr = source->head;
    Node* start = source->head;
    while(itr!=NULL){
        insertBack(dest, cloneIndividual(itr->data));
        itr = itr->next;
    }
    source->head = start;
}


        /*********CLONE AN INDIVIDUAL********/
Individual* cloneIndividual(void* original)
{
      Individual* temp = (Individual*)original;
      if(temp == NULL)return NULL;
      Individual* individualClone = (Individual*)calloc(3, sizeof(*individualClone));
      individualClone->events = createEventList(&printEvent, &dummyDelete, &compareEvents);
      individualClone->otherFields = createOtherFieldsList(&printField, &dummyDelete, &compareFields);
      individualClone->families = createFamilyList(&printFamily, &dummyDelete, &compareFamilies);


      if(doesStringExist(temp->givenName)){
        individualClone->givenName = NULL;
        stAllocater(&individualClone->givenName);
        myStringCopy(&individualClone->givenName, &temp->givenName);
      }
      if(doesStringExist(temp->surname)){
        individualClone->surname = NULL;
        stAllocater(&individualClone->surname);
        myStringCopy(&individualClone->surname, &temp->surname);
      }
      if(temp->otherFields.length > 0){
          Node* itr = temp->otherFields.head;
          void* data = NULL;
          while(itr != NULL){
            if(itr == NULL)break;
            data = itr->data;
            insertBack(&(individualClone->otherFields), cloneField(data));
            itr = itr->next;
          }
      }
      if(temp->events.length > 0){
          Node* itr = temp->events.head;
          void* data = NULL;
          while(itr != NULL){
            if(itr == NULL)break;
            data = itr->data;
            insertBack(&(individualClone->events), cloneEvent(data));
            itr = itr->next;
          }
      }
      if(temp->families.length>0){
        Node* itr = temp->families.head;
        void* data = NULL;
        while(itr != NULL){
          if(itr == NULL)break;
          data = itr->data;
          insertBack(&(individualClone->events), cloneFamily(data));
          itr = itr->next;
        }
      }



  return individualClone;
}



          /************FUNCTION TO CLONE AN INDIVIDUAL COPY**********/
IndividualCopy* cloneIndividualCopy(void* original)
{
       IndividualCopy* temp = (IndividualCopy*)original;
       if(doesObjectExist(temp))return NULL;

       IndividualCopy* iClone = (IndividualCopy*)calloc(1, sizeof(IndividualCopy));


       iClone->events = createEventList(&printEvent, &dummyDelete, &compareEvents);
       iClone->families = createFamilyList(&printFamily, &dummyDelete, &compareFamilies);
       iClone->otherFields = createOtherFieldsList(&printField, &dummyDelete, &compareFields);

       if(doesStringExist(temp->type)){
         stAllocater(&iClone->type);
         myStringCopy(&iClone->type, &temp->type);
       }
       if(doesStringExist(temp->pointer)){
         stAllocater(&iClone->pointer);
         myStringCopy(&iClone->pointer, &temp->pointer);
       }

       if(doesStringExist(temp->givenName)){
         stAllocater(&iClone->givenName);
         myStringCopy(&iClone->givenName, &temp->givenName);
       }
       if(doesStringExist(temp->surname)){
         stAllocater(&iClone->surname);
         myStringCopy(&iClone->surname, &temp->surname);
       }

       if(doesObjectExist(temp->spouseOf)){
         int i = 0;
         iClone->spouseOf = stringarrayAllocater(iClone->spouseOf);
         while(temp->spouseOf[i][0] == '@'){
             stAllocater(&iClone->spouseOf[i]);
             myStringCopy(&iClone->spouseOf[i], &temp->spouseOf[i]);
             i++;
         }
       }

       if(doesObjectExist(temp->childOf)){
         int i = 0;
         iClone->childOf = stringarrayAllocater(iClone->childOf);
         while(temp->childOf[i][0] == '@'){
             stAllocater(&iClone->childOf[i]);
             myStringCopy(&iClone->childOf[i], &temp->childOf[i]);
             i++;
         }
       }

       if(temp->families.length>0){
         Node* itr = temp->families.head;
         void* data = NULL;
         while(itr != NULL){
           if(itr == NULL)break;
           data = itr->data;
           insertBack(&(iClone->events), cloneFamily(data));
           itr = itr->next;
         }
       }

       if(temp->otherFields.length > 0){
           Node* itr = temp->otherFields.head;
           void* data = NULL;
           while(itr != NULL){
             if(itr == NULL)break;
             data = itr->data;
             insertBack(&(iClone->otherFields), cloneField(data));
             itr = itr->next;
           }
       }
       if(temp->events.length > 0){
           Node* itr = temp->events.head;
           void* data = NULL;
           while(itr != NULL){
             if(itr == NULL)break;
             data = itr->data;
             insertBack(&(iClone->events), cloneEvent(data));
             itr = itr->next;
           }
       }

       return iClone;
}









Family* cloneFamily(void* original)
{
      if(doesObjectExist(original)==false)return NULL;

      Family* temp = (Family*)original;
      Family* familyClone = (Family*)calloc(1, sizeof(*familyClone));

      familyClone->children = createIndividualList(&printIndividual, &dummyDelete, &compareIndividuals);
      familyClone->otherFields = createOtherFieldsList(&printField, &dummyDelete, &compareFields);
      familyClone->events = createEventList(&printEvent, &dummyDelete, &compareEvents);

      if(doesObjectExist(temp->wife)){
        familyClone->wife = cloneIndividual(temp->wife);
      }
      if(doesObjectExist(temp->husband)){
        familyClone->husband = cloneIndividual(temp->husband);
      }
      if(temp->children.length>0){
        Node* itr = temp->children.head;
        while(itr != NULL){
          if(itr == NULL)break;
          insertBack(&(familyClone->children), cloneIndividual(itr->data));
          itr = itr->next;
        }
      }
      if(temp->otherFields.length > 0){
          Node* itr = temp->otherFields.head;
          void* data = NULL;
          while(itr != NULL){
            if(itr == NULL)break;
            data = itr->data;
            insertBack(&(familyClone->otherFields), cloneField(data));
            itr = itr->next;
          }
      }
      if(temp->events.length > 0){
          Node* itr = temp->events.head;
          void* data = NULL;
          while(itr != NULL){
            if(itr == NULL)break;
            data = itr->data;
            insertBack(&(familyClone->events), cloneEvent(data));
            itr = itr->next;
          }
      }

  return familyClone;

}




                          /**********COME BACK TO THIS************/
FamilyCopy* cloneFamilyCopy(void* original)
{
    FamilyCopy* temp = (FamilyCopy*)original;
    if(doesObjectExist(temp)==false)return NULL;
    FamilyCopy* familyCopyClone = (FamilyCopy*)calloc(1, sizeof(*familyCopyClone));


    familyCopyClone->children = createIndividualList(&printIndividual, &dummyDelete, &compareIndividuals);
    familyCopyClone->otherFields = createOtherFieldsList(&printField, &dummyDelete, &compareFields);
    familyCopyClone->events = createEventList(&printEvent, &dummyDelete, &compareEvents);

    if(doesStringExist(temp->pointer)){
      stAllocater(&familyCopyClone->pointer);
      myStringCopy(&familyCopyClone->pointer, &temp->pointer);
    }
    if(doesStringExist(temp->type)){
      stAllocater(&familyCopyClone->type);
      myStringCopy(&familyCopyClone->type, &temp->type);
    }

    if(doesStringExist(temp->husbandLink)){
      stAllocater(&familyCopyClone->husbandLink);
      myStringCopy(&familyCopyClone->husbandLink, &temp->husbandLink);
    }
    if(doesStringExist(temp->wifeLink)){
      stAllocater(&familyCopyClone->wifeLink);
      myStringCopy(&familyCopyClone->wifeLink, &temp->wifeLink);
    }

    if(doesObjectExist(temp->childrenLinks)){
      int i = 0;
      familyCopyClone->childrenLinks = stringarrayAllocater(familyCopyClone->childrenLinks);
      while(temp->childrenLinks[i][0] == '@'){
          //stAllocater(&familyCopyClone->childrenLinks[i]);
          myStringCopy(&familyCopyClone->childrenLinks[i], &temp->childrenLinks[i]);
          i++;
      }
    }

    if(doesObjectExist(temp->husband)){
        familyCopyClone->husband = cloneIndividual(temp->husband);
    }

    if(doesObjectExist(temp->wife)){
        familyCopyClone->wife = cloneIndividual(temp->wife);
    }

    if(temp->children.length>0){
      Node* itr = temp->children.head;
      while(itr != NULL){
        if(itr == NULL)break;
        insertBack(&(familyCopyClone->children), cloneIndividual(itr->data));
        itr = itr->next;
      }
    }
    if(temp->otherFields.length > 0){
        Node* itr = temp->otherFields.head;
        void* data = NULL;
        while(itr != NULL){
          if(itr == NULL)break;
          data = itr->data;
          insertBack(&(familyCopyClone->otherFields), cloneField(data));
          itr = itr->next;
        }
    }
    if(temp->events.length > 0){
        Node* itr = temp->events.head;
        void* data = NULL;
        while(itr != NULL){
          if(itr == NULL)break;
          data = itr->data;
          insertBack(&(familyCopyClone->events), cloneEvent(data));
          itr = itr->next;
        }
    }

    return familyCopyClone;


}







Submitter* cloneSubmitter(void* original)
{
    Submitter* temp = (Submitter*)original;
    if(temp == NULL)return NULL;
    Submitter* submitterClone = (Submitter*)calloc(1, sizeof(*submitterClone));
    submitterClone->otherFields = createOtherFieldsList(&printField, &dummyDelete, &compareFields);
    char* submitterName = stringCopy(temp->submitterName);
    char* submitterAddress = stringCopy(temp->address);
    if(doesStringExist(submitterName)){
      strcpy(submitterClone->submitterName, submitterName);
    }
    if(temp->otherFields.length > 0){
        Node* itr = temp->otherFields.head;
        void* data = NULL;
        while(itr != NULL)
          if(itr == NULL)break;
          data = itr->data;
          insertBack(&(submitterClone->otherFields), cloneField(data));
          itr = itr->next;
    }
    if(doesStringExist(submitterAddress)){
      strncpy(submitterClone->address, submitterAddress, strlen(submitterAddress)+1);
    }
    return submitterClone;

}






          /*****************LINKERS and Populaters*****************/


void populateIndividualList(List* gedcomIndividuals, List* iCopyList)
{
    Node* itr = (iCopyList)->head;
    //Node* start = iCopyList->head;
    while(itr != NULL){
      IndividualCopy* currentIndividual = (IndividualCopy*)itr->data;
      insertBack(gedcomIndividuals, currentIndividual->individual);
      itr = itr->next;

    }
}



void populateFamilyList(List* gedcomFamilies, List* fCopyList)
{
    Node* itr = (fCopyList)->head;
    //Node* start = fCopyList->head;
    while(itr != NULL){
      FamilyCopy* currentFamily = (FamilyCopy*)itr->data;
      insertBack(gedcomFamilies, currentFamily->family);
      itr = itr->next;
    }
}



                  /**Function to populate a list with the descendants of
                  an individual.
                      Input: List pointer and individual pointer
                      Output: A list populated with the descendants of
                              an individual
                      Pre-conditions: List is empty
                      Post-coniditions: List is empty or contains the descednats
                      of person. Person remains intact
                  **/

void populateDescendantList(List* theDescendants, Individual* person)
{
   //if
   if(doesObjectExist(theDescendants)==false || doesObjectExist(person)==false){
       return;
   }//endif
   Node* familyItr = person->families.head;
   if(doesObjectExist(familyItr)==false){
     return;
   }
   while(doesObjectExist(familyItr)){//start while
      Family* family = (Family*)familyItr->data;
      if(person == family->husband || person == family->wife){//start if
        Node* individualItr = family->children.head;
        while(doesObjectExist(individualItr)){//start while
            Individual* child = (Individual*)individualItr->data;
            populateDescendantList(theDescendants, child);
            individualItr = individualItr->next;
        }//end while
      }//end if
      else{//start else
          Individual* child = person;
          insertBack(theDescendants, child);
          return;
       }//end else
       familyItr = familyItr->next;
   }//end while

}//end function




void linkTwoLists(List* iListCopy, List* fListCopy)
{
    Node* itr = iListCopy->head;
    Node* itr2 = fListCopy->head;
    Node* startIndiList = iListCopy->head;
    Node* startFamList = fListCopy->head;
    IndividualCopy* i1 = NULL;
    FamilyCopy* f1 = NULL;
    int numChildren = 0;
    while(itr != NULL){
        int spouseOfCounter = 0;
        int childOfCounter = 0;
        i1 = (IndividualCopy*)itr->data;
        while(itr2 != NULL){
          f1 = (FamilyCopy*)itr2->data;
          if(doesObjectExist(i1->spouseOf))
          while(strequal(i1->spouseOf[spouseOfCounter], f1->pointer)){
            if(DEBUGLINKER)printf("Adding a family to an individual s\n");
            insertBack(&(i1->individual->families), f1->family);
            if(DEBUGLINKER)printf("%d\n", i1->individual->families.length);
            spouseOfCounter++;
          }
          else{
              if(DEBUGLINKER)printf("XREF and pointer not equal spouse\n\n%s %s", i1->spouseOf[0], f1->pointer);
          }
          if(doesObjectExist(i1->childOf))
          while(strequal(i1->childOf[childOfCounter], f1->pointer)){
            if(DEBUGLINKER)printf("Adding a family to an individual c\n");
            insertBack(&(i1->individual->families), f1->family);
            if(DEBUGLINKER)printf("%d\n", i1->individual->families.length);
            childOfCounter++;
          }
          else{
            if(DEBUGLINKER)printf("XREF and pointer not equal child\n\n%s %s", i1->childOf[0], f1->pointer);
          }
          itr2 = itr2->next;
        }
        itr2 = startFamList;
        itr = itr->next;
    }

    iListCopy->head = startIndiList;
    //populateIndividualList(iList, iListCopy);

    iListCopy->head = startIndiList;
    fListCopy->head = startFamList;
    itr = iListCopy->head;
    itr2 = fListCopy->head;
    while(itr2 != NULL){
        f1 = (FamilyCopy*)itr2->data;
        numChildren = getNumChildren(f1->childrenLinks);
        while(itr != NULL){
              i1 = (IndividualCopy*)itr->data;
              if(strequal(f1->husbandLink, i1->pointer)){
                  f1->family->husband = i1->individual;
              }
              if(strequal(f1->wifeLink, i1->pointer)){
                  f1->family->wife = i1->individual;              }
              if(numChildren > 0){
                    for(int i = 0; i < numChildren;i++){
                        if(strequal(f1->childrenLinks[i], i1->pointer)){
                            insertBack(&(f1->family->children), i1->individual);
                        }
                    }
              }
             itr = itr->next;
        }
        itr = startIndiList;
        itr2 = itr2->next;
    }
    //populateFamilyList(fList, fListCopy);
    iListCopy->head = startIndiList;
    fListCopy->head = startFamList;
}






	/*********PRINT FUNCTIONS************/
char* printString(void* print)
{
    if(print == NULL)return NULL;
    char* printer = NULL;
    printer = stringCopy(print);
    if(strlen(printer)==0){
	printer = "";
	return printer;
    }
    return printer;
}





char* printGEDline(void* print)
{
    if(print==NULL){
      printf("Is Null\n");
      return NULL;
    }

    /***MUTATE THE PRINT*********/
    GEDline* original = (GEDline*)print;
    GEDline* clone = cloneGEDline(original);
    if(clone != NULL){

    }
    char* gedInfo = NULL;
    gedInfo = stringAllocater(gedInfo);
    if(clone != NULL){
      if(doesStringExist(clone->level)){
        strncpy(gedInfo, clone->level, strlen(clone->level)+1);
        strcat(gedInfo, " ");
      }
      if(doesStringExist(clone->tag)){
        strncat(gedInfo, clone->tag, strlen(clone->tag)+1);
        strcat(gedInfo, " ");
      }
      if(doesStringExist(clone->pointer)){
        strncat(gedInfo, clone->pointer, strlen(clone->pointer)+1);
        strcat(gedInfo, " ");
      }
      if(doesStringExist(clone->ID)){
        strncat(gedInfo, clone->ID, strlen(clone->ID)+1);
        strcat(gedInfo, " ");
      }
      if(doesStringExist(clone->value)){
        strncat(gedInfo, clone->value, strlen(clone->value)+1);
        strcat(gedInfo, " ");
      }
      if(clone->lineNumber >= 0){
        char lineNumAsString [10];
        sprintf(lineNumAsString, "%d", original->lineNumber);
        strncat(gedInfo, lineNumAsString, strlen(lineNumAsString)+1);
      }
    }
    return gedInfo;
}

int compareGEDline(const void* a, const void* b)
{
    GEDline* x = (GEDline*)a;
    GEDline* y = (GEDline*)b;
    if(strequal(x->tag, y->tag)==true){
      return 0;
    }else{
      return -1;
    }
}


        /********************MUTATORS****************/
GEDline* cloneGEDline(GEDline* original)
{
    GEDline* clone = (GEDline*)calloc(100, sizeof(GEDline));
    if(original == NULL){
        return NULL;
    }
    if(original->level != NULL)
      clone->level = stringCopy(original->level);
    if(original->tag != NULL)
      clone->tag = stringCopy(original->tag);
    if(original->pointer != NULL)
      clone->pointer = stringCopy(original->pointer);
    if(original->ID != NULL)
      clone->ID = stringCopy(original->ID);
    if(original->value != NULL)
      clone->value = stringCopy(original->value);
    if(original->lineNumber > 0)
      clone->lineNumber = original->lineNumber;
    return clone;
}






            /**********************LIST CREATORS*******************/


              /********GEDLINE LIST INITIALIZER**********/

List createGEDLineList(char** fileLines, int numLinesInFile, char* (*print)(void* p), void (*del)(void* toBeDeleted), int(*comp)(const void* a, const void* b))
{
    List gedLineList = initializeList(print, del, comp);
    gedLineList.length = 0;
    for(int i = 0; i < numLinesInFile; i++){
        GEDline* original = createGEDline(fileLines[i], i);
        GEDline* clone = cloneGEDline(original);
          if(clone != NULL){
            insertBack(&gedLineList, clone);
          }

    }
    //freeStringArray(theFile);
    return gedLineList;
}




              /***********INDIVIDUAL LISTS*************/
List createIndividualCopyList(char* (*print)(void* toBePrinted), void (*del)(void* toBeDeleted), int (*comp)(const void* a, const void* b))
{
    List individualCopyList = initializeList(print, del, comp);
    return individualCopyList;
}



List createIndividualList(char* (*print)(void* toBePrinted), void (*del)(void* toBeDeleted), int (*comp)(const void* a, const void* b))
{
    List individualList = initializeList(print, del, comp);
    return individualList;
}


        /****************FAMILY LISTS**************/

        /********INITIALIZES A FAMILY COPY LIST*********/
List createFamilyCopyList(char* (*print)(void* toBePrinted), void(*del)(void* toBeDeleted), int (*comp)(const void* a, const void* b))
{
    List familyCopyList = initializeList(print, del, comp);
    return familyCopyList;
}
      /********INITIALIZES A FAMILY LIST********/
List createFamilyList(char* (*print)(void* toBePrinted), void(*del)(void* toBeDeleted), int (*comp)(const void* a, const void* b))
{
    List familyList = initializeList(print, del, comp);
    return familyList;
}




        /***********CREATES OTHER FIELDS LIST***********/
List createOtherFieldsList(char* (*print)(void* toBePrinted), void(*del)(void* toBeDeleted), int (*comp)(const void* a, const void* b))
{
    List otherFieldsList = initializeList(print, del, comp);
    return otherFieldsList;
}


    /**************CREATES EVENT LIST***********/

List createEventList(char* (*print)(void* toBePrinted), void(*del)(void* toBeDeleted), int (*comp)(const void* a, const void* b))
{
        List eventList = initializeList(print, del, comp);
        return eventList;
}


      /**********CREATES GEDRECORD LIST***********/


List createRecordList(char* (*print)(void* toBePrinted), void(*del)(void* toBeDeleted), int (*comp)(const void* a, const void* b))
{
    List recordList = initializeList(print, del, comp);
    return recordList;
}





          /***********************DESTROYERS************************/

        //Dummy Delete Function//

void dummyDelete(void* a){
    if(a == NULL)return;
}


      //Destroy GEDline//
void destroyGEDline(void* destroy)
{
    if(destroy == NULL)return;
    GEDline* gedLineDestroyer = (GEDline*)destroy;
    if(doesStringExist(gedLineDestroyer->level))
    freeString(gedLineDestroyer->level);
    if(doesStringExist(gedLineDestroyer->tag))
    freeString(gedLineDestroyer->tag);
    if(doesStringExist(gedLineDestroyer->pointer))
    freeString(gedLineDestroyer->pointer);
    if(doesStringExist(gedLineDestroyer->ID))
    freeString(gedLineDestroyer->ID);
    if(doesStringExist(gedLineDestroyer->value))
    freeString(gedLineDestroyer->value);
    free(gedLineDestroyer);
}


        /***********Destroy Record**********/

void deleteRecord(void* destroy)
{
  Record* r = (Record*)destroy;
  for(int i = 0; i < 50; i++)
  {
    if(r->info[i] == '\0'){
        free(r->info[i]);
    }
    else{
        destroyGEDline(r->info[i]);
    }
  }
  free(r->info);
  free(r);
}


          /**********DESTROYER FOR INDIVIDUALCOPY******************/

void deleteIndividualCopy(void* toBeDeleted)
{
      if(toBeDeleted == NULL)return;
      IndividualCopy* i = (IndividualCopy*)toBeDeleted;
      if(doesStringExist(i->type))
        freeString(i->type);
      if(doesStringExist(i->pointer))
          freeString(i->pointer);
      if(doesObjectExist(i->spouseOf))
            freeStringArray(i->spouseOf);
      if(doesObjectExist(i->childOf))
            freeStringArray(i->childOf);
      if(doesStringExist(i->givenName))
              freeString(i->givenName);
      if(doesStringExist(i->surname))
                freeString(i->surname);
      free(i);

}


        /*************DESTROY FAMILYCOPY***********************/

void deleteFamilyCopy(void* toBeDeleted)
{
    FamilyCopy* f = (FamilyCopy*)toBeDeleted;
    if(f == NULL)return;
    if(doesStringExist(f->type))
      freeString(f->type);
    if(doesStringExist(f->pointer))
        freeString(f->pointer);
    if(doesStringExist(f->husbandLink))
      freeString(f->husbandLink);
    if(doesStringExist(f->wifeLink))
      freeString(f->wifeLink);
    if(doesObjectExist(f->childrenLinks))
      freeStringArray(f->childrenLinks);
    free(f);
}


void deleteSubmitterCopy(void* toBeDeleted)
{
    SubmitterCopy* temp = (SubmitterCopy*)toBeDeleted;
    free(temp);
}

void deleteHeaderCopy(void* toBeDeleted)
{
  HeaderCopy* temp = (HeaderCopy*)toBeDeleted;
  free(temp);
}


void deleteHeader(void* toBeDeleted)
{
  if(toBeDeleted == NULL)
    return;
  Header* temp = (Header*)toBeDeleted;
  clearList(&temp->otherFields);
  free(temp);
}


void deleteSubmitter(void* toBeDeleted)
{
    if(toBeDeleted == NULL)return;
    Submitter* temp = (Submitter*)toBeDeleted;
    clearList(&temp->otherFields);
    free(temp);
}




                /**************PARSER****************/
ErrorCode generalParser(List gedLineList, List recordList, List* iList, List* fList, HeaderCopy* h, SubmitterCopy* s, int* lineNumber)
{
    Node* itr = NULL;
    itr = gedLineList.head;
    int  restart = 0;
    ErrorCode eCode;
    bool foundTrailer = false;
    for(int i = 0; i < gedLineList.length; i++){
          if(itr == NULL){
            break;
          }
          GEDline* newLine = (GEDline*)(itr->data);
          if(strequal(newLine->level, "0")){
               restart = 0;
               insertBack(&recordList, createRecord(newLine));
          }else{
            restart++;
            setRecord((Record*)getFromBack(recordList), newLine, restart);
          }
          itr = itr->next;
    }
    for(int i = 0; i < recordList.length; i++){
          if(i ==0){
            itr = recordList.head;
          }
          if(itr == NULL)break;

          Record* record = (Record*)itr->data;

          if(strequal(record->info[0]->tag, "HEAD")){
              eCode = headerCopyParser(h, record);
            if(eCode == INV_HEADER){
              return eCode;
            }
          }
          else if(strequal(record->info[0]->tag, "INDI")){
              eCode = individualCopyParser(iList, record);
              if(eCode == INV_RECORD){
                return eCode;
              }
          }
          else if(strequal(record->info[0]->tag, "FAM")){
               eCode = familyCopyParser(fList, record);
               if(eCode == INV_RECORD){
                 return eCode;
               }
          }
          else if(strequal(record->info[0]->tag, "SUBM")){
              eCode = submitterCopyParser(s, record);

              if(eCode == INV_RECORD){
                return eCode;
              }
              if(doesStringExist(s->pointer)){
                  if(strequal(s->pointer, h->submitterTag)){
                      h->submitter = createSubmitter(s);
                  }
              }
          }
          else if(strequal(record->info[0]->tag, "TRLR")){
            if(i == (recordList.length -1)){
              foundTrailer = true;
              eCode = OK;
              return eCode;
            }
          }
        itr = itr->next;
    }
    if(foundTrailer == false){
        eCode = INV_GEDCOM;
    }
    eCode = OK;
    return eCode;
}




ErrorCode headerCopyParser(HeaderCopy* head, Record* record)
{
    ErrorCode eCode;

    for(int i = 1; i < record->size; i++){
        if(strequal(record->info[1]->tag, "SOUR")==true){
            char* source = stringCopy(record->info[1]->value);
            strncpy(head->source, source, strlen(source)+1);
            free(source);
        }else{

            eCode = INV_HEADER;
        }


       if(strequal(record->info[i]->tag, "GEDC")){
          if(doesObjectExist(record->info[i+1])){
              if(strequal(record->info[i+1]->tag, "VERS")){
                  head->gedcVersion = atof(record->info[i+1]->value);
              }else{

                  eCode = INV_HEADER;
                  return eCode;
              }
          }
        }



       else if(strequal(record->info[i]->tag, "CHAR")){
          if(strequal(record->info[i]->value, "ASCII")){
             head->encoding = ASCII;
          }
          else if(strequal(record->info[i]->value, "ANSEL")){
            head->encoding = ANSEL;
          }
          else if(strequal(record->info[i]->value, "UNICODE")){
            head->encoding = UNICODE;
          }
          else if(strequal(record->info[i]->value, "UTF8")){
            head->encoding = UTF8;
          }
          else{
              //eCode = INV_HEADER;
              //return eCode;
          }
       }


        else if(strequal(record->info[i]->tag, "VERS")){
          if(doesObjectExist(record->info[i+1])){
            if(strequal(record->info[i+1]->tag, "FORM")){
                eCode = OK;
            }
          }
        }


        else if(strequal(record->info[i]->tag, "SUBM")){
            if(doesStringExist(record->info[i]->ID)){
              strcpy(head->submitterTag, record->info[i]->ID);
            }
            else{
              eCode = INV_HEADER;
              return eCode;
            }
        }


        else{
            if((doesStringExist(record->info[i]->tag)) && (doesStringExist(record->info[i]->value))){
                Field* newField = createField(record->info[i]->tag, record->info[i]->value);
                insertBack(&head->otherFields, newField);
            }else{
              eCode = INV_HEADER;
              return eCode;
            }
        }
    }
    eCode = OK;
    return eCode;
}





ErrorCode individualCopyParser(List* individualCopyList, Record* record)
{
    int numSpouce = 0;
    int numChild = 0;
    ErrorCode eCode;
    for(int i  = 0; i < record->size; i++){
        if(i == 0){
          insertBack(individualCopyList, createIndividualCopy(record->info[i]));
        }
        else if(strequal(record->info[i]->tag, "NAME")){
          individualCopySetName(getFromBack(*individualCopyList), record->info[i]->value);
        }
        else if(strequal(record->info[i]->tag, "FAMS")){
           individualCopySetSpouseStatus(getFromBack(*individualCopyList), record->info[i]->ID, numSpouce);
           numSpouce++;
        }
        else if(strequal(record->info[i]->tag, "FAMC")){
            individualCopySetChildStatus(getFromBack(*individualCopyList), record->info[i]->ID, numSpouce);
            numChild++;
        }
        else if(validIndiEventTag(record->info[i]->tag)){
            if((doesObjectExist(record->info[i+1]))&&(doesObjectExist(record->info[i+2]))){
              Event* newEvent = createEvent(record->info[i], record->info[i+1], record->info[i+2]);
              IndividualCopy* indi = (IndividualCopy*)getFromBack(*individualCopyList);
              insertBack(&(indi->events), newEvent);
              insertBack(&(indi->individual->events), newEvent);
              i = i+3;
          }
        }
        else{
          if((doesStringExist(record->info[i]->tag)) && (doesStringExist(record->info[i]->value))){
            Field* f = createField(record->info[i]->tag, record->info[i]->value);
            if(f == NULL)return eCode = INV_RECORD;
            IndividualCopy* indi = (IndividualCopy*)getFromBack(*individualCopyList);
            insertBack(&(indi->otherFields), f);
            insertBack(&(indi->individual->otherFields), f);
          }
        }
    }
    eCode = OK;
    return eCode;
}






ErrorCode submitterCopyParser(SubmitterCopy* submitter, Record* record)
{
    ErrorCode eCode;
    for(int i = 0; i < record->size; i++){
      if(i == 0){
        if(doesStringExist(record->info[i]->pointer)){
          strcpy(submitter->pointer, record->info[i]->pointer);
        }
        else{
          return eCode = INV_RECORD;
        }
      }
      if(strequal(record->info[i]->tag, "NAME")){
          if(doesStringExist(record->info[i]->value)){
            char* subName = stringCopy(record->info[i]->value);
            strncpy(submitter->submitterName, subName, strlen(subName));
          }
      }
      else if(strequal(record->info[i]->tag, "ADDR")){
         char* address = stringCopy(record->info[i]->value);
         strncpy(submitter->address, address, strlen(address)+1);
      }
      else if(strequal(record->info[i]->tag, "SUBM")){
          continue;
      }
       else{
          Field* f = createField(record->info[i]->tag, record->info[i]->value);
          insertBack(&(submitter->otherFields), f);
       }
    }
    eCode = OK;
    return eCode;
}



        //******WORK IN PROGRESS********/
ErrorCode familyCopyParser(List* familyCopyList, Record* record){
      ErrorCode eCode = OK;
      int numberOfChildren = 0;
      if(record == NULL){
        return OK;
      }
      for(int i = 0; i < record->size; i++){
          if(i == 0){
            insertBack(familyCopyList, createFamilyCopy(record->info[i]));
          }
          else if(strequal(record->info[i]->tag, "HUSB")){
              familyCopySetHusband(getFromBack(*familyCopyList), record->info[i]->ID);
          }
          else if(strequal(record->info[i]->tag, "WIFE")){
              familyCopySetWife(getFromBack(*familyCopyList), record->info[i]->ID);
          }
          else if(strequal(record->info[i]->tag,"CHIL")){
            familyCopySetChild(getFromBack(*familyCopyList), record->info[i]->ID, numberOfChildren);
            numberOfChildren++;
          }
              /******check for valid family tags*********/
          else if(validFamEventTag(record->info[i]->tag)){
              if((doesObjectExist(record->info[i+1]))&&(doesObjectExist(record->info[i+2]))){
                  Event* theEvent = createEvent(record->info[i], record->info[i+1], record->info[i+2]);
                  FamilyCopy* f = (FamilyCopy*)getFromBack(*familyCopyList);
                  insertBack(&(f->events), theEvent);
                  insertBack(&(f->family->events), theEvent);
              }
          }
          else{
              if((doesStringExist(record->info[i]->tag)) && (doesStringExist(record->info[i]->value))){
                Field* newField = createField(record->info[i]->tag, record->info[i]->value);
                FamilyCopy* f = (FamilyCopy*)getFromBack(*familyCopyList);
                insertBack(&(f->otherFields), newField);
                insertBack(&(f->family->otherFields), newField);
             }
          }
      }
      return eCode;
}


































                  /*********HELPER FUNCTIONS*************/
char* printIndividualCopy(void* toBePrinted)
{

      if(toBePrinted==NULL)return NULL;
      IndividualCopy* indi = (IndividualCopy*)toBePrinted;
      char* printer = NULL;
      stAllocater(&printer);
      if(doesStringExist(indi->givenName)&&(doesStringExist(indi->surname))){
          myStringCopy(&printer, &indi->givenName);
          strcat(printer, " ");
          strcat(printer, indi->surname);

      }
      else if(doesStringExist(indi->givenName)){
        printer = stringCopy(indi->givenName);
      }
      else if(doesStringExist(indi->surname)){
        printer = stringCopy(indi->surname);
      }
      if(doesStringExist(indi->pointer)){
           strcat(printer, " ");
           strcat(printer, indi->pointer);
      }
      if(doesObjectExist(indi->spouseOf)){
           int i = 0;
           while(doesStringExist(indi->spouseOf[i])){
               strcat(printer, " ");
               strcat(printer, "Spouse of");
               strcat(printer, " ");

               strcat(printer, indi->spouseOf[i]);
               i++;
           }

      }
      if(doesObjectExist(indi->childOf)){
          int i  = 0;
          while(doesStringExist(indi->childOf[i])){
              strcat(printer, " ");
              strcat(printer, "Spouse of");
              strcat(printer, " ");

              strcat(printer, indi->childOf[i]);
              i++;
          }
      }
      return printer;
}



char* printFamilyCopy(void* a)
{
    char* printer = NULL;
    stAllocater(&printer);
    if(a == NULL)return NULL;
    FamilyCopy* f = (FamilyCopy*)a;
    if(doesStringExist(f->pointer)){
      myStringCopy(&printer, &f->pointer);
    }
    if(doesStringExist(f->husbandLink)){
        strcat(printer, " ");
        strcat(printer, "Husband Link");
        strcat(printer, " ");

        strcat(printer, f->husbandLink);

    }
    if(doesStringExist(f->wifeLink)){
        strcat(printer, " ");
        strcat(printer, "wifeLink Link");
        strcat(printer, " ");

        strcat(printer, f->wifeLink);

    }
    if(doesObjectExist(f->childrenLinks)){
        int i = 0;
        while(doesStringExist(f->childrenLinks[i])){
          strcat(printer, " ");
          strcat(printer, "child Link");
          strcat(printer, " ");

          strcat(printer, f->childrenLinks[i]);
          i++;
        }
    }
    return printer;

}
