/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
 * Stego.c: A program for hiding files in images                        *
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

void printStuff(unsigned char *p, int num);
void setlsb(unsigned char *p, unsigned char b0, int num);
struct Buffer scramble(struct Buffer data, int *numArray);
void randPerm(int *numArray, int n);
void InitializeArray(int *numArray, const int length);
void setSizelsb(unsigned char *p, int b0,int num);

int main(int argc, char* argv[])
{
	int i, j, k, cover_bits, bits;
  	struct Buffer b = {NULL, 0, 0}, sb = {NULL, 0, 0};
  	struct Image img = {0, NULL, NULL, NULL, NULL, 0, 0};
  	byte b0;

  	if (argc != 5)
  	{
      		printf("Error!!! Usage: \n%s <key> <cover_file> <stego_file> <file_to_hide> \n", argv[0]);
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
  	ReadBinaryFile(argv[4],&b);    // Read binary data
  	if (img.NofR == 0)
      		printf("failed to read image file\n");

  	// hidden information 
  	// first four bytes is the size of the hidden file
  	// next 4 bytes is the G number (4 bits per digit)
  	if (!GetColor)
    		cover_bits = img.NofC*img.NofR;
  	else
    		cover_bits = 3*img.NofC*img.NofR;
  	bits = (8 + b.size)*8;
  	if (bits > cover_bits)
   	{
      		printf("Cover file is not large enough %d (bits) > %d (cover_bits)\n",bits,cover_bits);
      		exit(1);
    	}

  	// embed four size bytes for the Buffer's size field
  	byte size = b.size;
  	byte *image = malloc(cover_bits/8);
  	for(int i = 0; i<cover_bits/8;i++)
  	{
        	*image = GetGray(i);
        	image++;
  	}
  	image -= cover_bits/8;
  	
	setSizelsb(image,b.size,32);
  	//setlsb(image, b.size, 32);
  	image = image+32;
  	//Embed G #
  	setlsb(image, 0, 4);
  	image += 4;
  	setlsb(image, 1, 4);
  	image += 4;
  	setlsb(image, 0, 4);
  	image += 4;
  	setlsb(image, 1, 4);
  	image += 4;
  	setlsb(image, 7, 4);
  	image += 4;
  	setlsb(image, 2, 4);
  	image += 4;
  	setlsb(image, 2, 4);
  	image += 4;
  	setlsb(image, 2, 4);
  	image += 4;
  	image -= 64;
  
  	printf("Embeding....... \n");
  	int perm[b.size];
  	InitializeArray(perm,b.size);
  
  	randPerm(perm, b.size);  
  	sb = scramble(b,perm);

  	for (i=0; i<b.size; i++)
  	{
      		// here you embed information into the image one byte at the time
      		// note that you should change only the least significant bits of the image
     		byte toEmbed = sb.data[i];
     		setlsb(&image[64+(8*i)],toEmbed,8);
  	}
  	int totalBytesChanged = 32 + 32 + b.size*8;
  	//printStuff(image+64,24);
  	for(int i = 0; i<totalBytesChanged; i++)
  		SetGray(i,image[i]);

  	WriteImage(argv[3],img);
  	free(image);
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
void setSizelsb(unsigned char *p, int b0,int num)
{
	printf("Size is %d Bytes\n",b0);
        for(int x = 0; x<num;x++)
        {
                unsigned char w = p[x];
                unsigned char t = w & ~(1);
                unsigned char z = t | ( (b0 & (1<<x))>>x);
                p[x] = z;
        }
}
void setlsb(unsigned char *p, unsigned char b0,int num)
{
        for(int x = 0; x<num;x++)
        {
                unsigned char w = p[x];
                unsigned char t = w & ~(1);
                unsigned char z = t | ( (b0 & (1<<x))>>x);
                p[x] = z;
        }
}
struct Buffer scramble(struct Buffer data, int *numArray)
{
	struct Buffer b = data;
	struct Buffer out;
	out.data = (unsigned char*)malloc(b.size);
	int size = b.size;
	int i = 0;
	for(i = 0; i<data.size; i++)
	{
		//printf("%c",GetByte(numArray[i]));
		out.data[i] = GetByte(numArray[i]);
	}
	//printf("%d\n",i);
	return out;
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
