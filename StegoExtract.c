/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
 * StegoExtract.c: A program for manipulating images                    *
 * Author: Brett Henry			                                *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include <string.h>
#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0) 
#define PRINTBIN(x) printf(BYTETOBINARYPATTERN, BYTETOBINARY(x)); 
// randperm function

unsigned char getlsb(unsigned char *p, int size);
void printStuff(unsigned char *p, int num);
void randPerm(int *numArray, int n);
void InitializeArray(int *numArray, const int length);
void printArray(int arr[], int n);
int getSizelsb(unsigned char *p,int size);
// unscramble function
struct Buffer unscram(struct Buffer in);

int main(int argc, char *argv[])
{  
  int i, j, k, cover_bits, bits;
  struct Buffer b = {NULL, 0, 0}, ub = {NULL, 0, 0};   // need to buffers
  struct Image img = {0, NULL, NULL, NULL, NULL, 0, 0};
  byte b0;
   
  if (argc != 4) 
    {
      printf("Error!!! Usage:\n%s <key> <stego_file> <file_to_extract> \n", argv[0]);
      exit(1);
    }

  // convert key from string to integer
  const int KEY;
  sscanf(argv[1],"%d",&KEY);
  // use key to initialize random number generator
  srandom(KEY);
  ReadImage(argv[2],&img);       // read image file into the image buffer img
                                 // the image is an array of unsigned chars (bytes) of NofR rows
                                 // NofC columns, it should be accessed using provided macros

  // hidden information 
  // first four bytes is the size of the hidden file
  // next 4 bytes is the G number (4 bits per digit)
  if (!GetColor)
    cover_bits = img.NofC*img.NofR;
  else 
    cover_bits = 3*img.NofC*img.NofR;    
  
  unsigned char* image = (unsigned char*)malloc(sizeof(byte)*64);
  for(int i =0; i<64;i++)
  {
	*image = GetGray(i);
        image++;
  }
  
  image -= 64;
  byte *start = image;
  b.size = 0;
  b.size = getSizelsb(image,32);
  printf("Size is: %d Bytes\n",b.size);
  
  char Gnum[8];
  printf("\nGNumber is: \n");
  for(int x =0;x<8;x++)
  {
  	Gnum[x] = getlsb(image+32+(4*x),4);
  	printf("%d",Gnum[x]);
  }
  unsigned char * stuff = (unsigned char*)malloc(sizeof(byte)*(8*b.size));
  
  for(int i =0; i<b.size*8;i++)
  {
	*stuff = GetGray(64+i);
	//PRINTBIN(*stuff);
	//printf("\n");        
	stuff++;
  }
  stuff -= (8*b.size);
  //printStuff(stuff,24);
  b.data = (unsigned char*)malloc(sizeof(byte)*(8*b.size));
  byte curr;
  
  for (int t = 0; t<b.size; t++)
    {
	curr = getlsb(stuff,8);     
	SetByte(t,curr);
	//printf("%c\n",GetByte(t));
        stuff+=8;
    }
  stuff -= sizeof(byte)*(8*b.size);
  free(image);
  free(stuff);
  ub = unscram(b);

  WriteBinaryFile(argv[3],ub);  // output payload file
}
unsigned char getlsb(unsigned char *p,int size)
{
	unsigned char num = 0;
	unsigned char set;
	for(int x =0; x< size; x++)
	{
		set = p[x];
		unsigned char y = set & 1;
		y = y<<x;
		num = num | y;
	}
	return num;
}
int getSizelsb(unsigned char *p,int size)
{
	int num = 0;
	unsigned char set;
	for(int x =0; x< size; x++)
	{
		set = p[x];
		int y = set & 1;
		y = y<<x;
		num = num | y;
	}
	return num;
}
struct Buffer unscram(struct Buffer in)
{
	int size = in.size;
	//printf("%d\n%d\n",in.size,size);
	int perm[size];
	InitializeArray(perm,size);
	
	randPerm(perm,size);
	struct Buffer out;
	out.data = (unsigned char*)malloc(size);
	out.size = in.size;
	for(int x =0; x<size;x++)
	{
		out.data[perm[x]] = in.data[x];
	}
	return out;
}
void printStuff(unsigned char *p, int num)
{
	for(int x =0; x<num; x++)
	{
                if(x%8 == 0)
			printf("\n");    
		PRINTBIN(p[x]);
		printf("\n");
	}
}
void randPerm(int *numArray, int n)
{
	for(int i = n-1; i>0; i--)
	{
		int index = random() % (i+1);
		int temp = numArray[i];
		numArray[i] = numArray[index];
		numArray[index] = temp;
		//printArray(numArray,n); //Uncomment for debug
		//fgetc(stdin); // uncomment to pause after each swap in the permutation, to see the algorithm in action
	}
}
void InitializeArray(int *numArray, const int length)
{
	for(int i = 0; i< length; i++)
	{
		*numArray = i;
		//printf("%d",numArray[i]);
		numArray++;
	}
}
void printArray(int arr[], int n)
{
	printf("[");
	for(int i =0; i<n; i++)
	{
		if(i == 0)
			printf("%d",arr[i]);
		else if(i == n-1)
			printf(", %d", arr[i]);
		else
			printf(" , %d",arr[i]);
	}
	printf("]\n");
}
