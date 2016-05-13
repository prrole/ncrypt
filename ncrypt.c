/* 
   Small utility to turn NAOMI binary files into cleartext 
   gcc ncrypt.c -lm /usr/lib64/libcrypto.a /usr/lib64/libz.so -o ncrypt
   Greetings from the Irken Empire 
*/

#include <stdio.h>
#include <stdlib.h>
#include <openssl/des.h>
#include <zlib.h>
#include <string.h>

int main(int argc, char** argv)

{
     FILE *infile, *outfile;
     int DESoperation;
     uLong  ecrc;
  
     DES_cblock key;
     DES_key_schedule schedule;

     unsigned char inbuf[8]={0};
     unsigned char outbuf[8]={0};

     //First some house-keeping stuff

     if(argc!=5)
	  errorandquit("missing parameters");

     if(strlen(argv[4])!=16)
	  errorandquit("key must be 16 characters hex (8 bytes)");

     if(!strcmp(argv[1],"-d"))
	  DESoperation=DES_DECRYPT;
     else
	  if(!strcmp(argv[1],"-e"))
	       DESoperation=DES_ENCRYPT;
	  else
	       errorandquit("operation not specified ");

     sscanf(argv[4],"%2x%2x%2x%2x%2x%2x%2x%2x",&key[0],&key[1],&key[2],&key[3],&key[4],&key[5],&key[6],&key[7]);

     if(!(infile=fopen(argv[2],"rb")))
     {
	  printf("%s not found\n",argv[1]);
	  exit(-1);
     }

     if(!(outfile=fopen(argv[3],"wb")))
     {
	  printf("%s could not be opened for writing\n",argv[2]);
	  exit(-1);
     }
     if(DESoperation==DES_DECRYPT)
	  printf("decrypting");
     else
	  printf("encrypting");
     printf(" %s to %s using DES ECB key 0x%s\n",argv[2],argv[3],argv[4]);

     // housekeeping done, start the real stuff

     ecrc = crc32(0L, Z_NULL, 0);  // initialize crc value to 0
     reverse(key); // we need to reverse the key
     DES_set_key_unchecked(&key, &schedule);  // initialize des key

     while (fread(inbuf,1,8,infile))   // read input, 8 bytes at a time
     {


	  if(DESoperation==DES_DECRYPT)
	       ecrc=crc32(ecrc,inbuf,8);      //crc32 of encrypted data

	  reverse(inbuf); //we need to reverse the input data
	  DES_ecb_encrypt(&inbuf, &outbuf,&schedule,DESoperation); // encrypt or decrypt data
	  reverse(outbuf); //we need to reverse the output data

	  if(DESoperation==DES_ENCRYPT)
	       ecrc=crc32(ecrc,outbuf,8);      //crc32 of encrypted data

	  fwrite(outbuf,1,8,outfile);
     }
     ecrc^=0xffffffff;
     printf("Encrypted image CRC32 Bxx.BIN format: [%x], TransferGame format: [%x]\n",endianswap(ecrc),ecrc);

     fclose(infile);
     fclose(outfile);

     return 0;
}

errorandquit(char *string)
{
     printf("usage: ncrypt -operation inputfile outputfile deskey\n");
     printf("example:  ncrypt -d encryptedfile.bin plaintext.bin 00AA00AA00AA00AA\n");
     printf("          ncrypt -e plaintext.bin encryptedfile.bin 00AA00AA00AA00AA\n");
     printf("error: %s\n",string);
     exit(-1);
}

reverse(unsigned char *inbuf)
{
     int i;
     unsigned char tmp[8];
     for(i=0;i<8;i++)
	  tmp[i]=*(inbuf+i);

     for(i=0;i<8;i++)
	  *(inbuf+i)=tmp[7-i];
}

int endianswap(int num)
{
  int byte0, byte1, byte2, byte3;

  byte0 = (num & 0x000000FF) >> 0 ;
  byte1 = (num & 0x0000FF00) >> 8 ;
  byte2 = (num & 0x00FF0000) >> 16 ;
  byte3 = (num & 0xFF000000) >> 24 ;

  return((byte0 << 24) | (byte1 << 16) | (byte2 << 8) | (byte3 << 0));
} 
