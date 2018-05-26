#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "LinkedListAPI.h"
#include "GEDCOMparser.h"
#include "GEDCOMutilities.h"
#include "macros.h"

#define DEBUG1 true
#define DEBUG2 false
#define DEBUG3 false
#define DEBUG4 false
#define DEBUG5  false
#define DEBUG5a false
#define DEBUG5b false
#define DEBUG5c false
#define DEBUG5d false
#define DEBUG5e false
#define DEBUG6  false
#define DEBUG7  false
#define DEBUG8	false
#define DEBUG9  false
#define DEBUG11 false
#define DEBUG12 false
#define DEBUG13	false
#define DEBUG14 false
#define DEBUG15 false

		/**************SET OF TEST STRINGS**********/
char string1[50] = "Ming Shing";
char string2[50] = "I'm a tag";
char string3[50] = "    Fangs   ";
char string4[255] = "HUSB";
char string5[255] = "0 HEAD @pen@ candy";
char badLevelNumber[100] = "03 DICK @head@";
char badPointer[100] = "";


		/**********MAIN TEST PROGRAM***********/
int main(int argc, char* argv[])
{
   //case 1
   if(DEBUG1 == true){
     GEDCOMobject* obj = NULL;
     GEDCOMerror e = createGEDCOM(argv[1], &obj);
     const Individual* john = (const Individual*)getFromFront(obj->individuals);
     List l = getDescendants(obj, john);
     char* s = toString(l);
     printf("FINAL LIST %s", s);
   }//end case 1


    return 0;
}//endMain
