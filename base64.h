//
// Created by laura_trive on 22/03/16.
//

/*  Functions prototypes to encode/decode into/from base64  */

#ifndef WEBSERVER_BASE64_H
#define WEBSERVER_BASE64_H


/* decode 4 '6-bit' characters into 3 8-bit binary bytes   */
void decodeBlock(unsigned char in[], char *clrStr);

/* decode a base 64 string to a characters string   */
void b64Decode(char *b64src, char *clrDst);

/* encode 3 8-bit binary bytes as 4 '6-bit' characters */
void encodeBlock( unsigned char in[], char b64str[], int len );

/*  base64 encode a stream, adding padding if needed    */
void b64Encode(char *clrStr, char *b64dst);


#endif //WEBSERVER_BASE64_H
