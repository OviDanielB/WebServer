//
// Created by laura_trive on 22/03/16.
//

/*  Implementation of functions to encode/decode with base64.    */

#include <stdio.h>
#include <string.h>

/*  base64 encoding/decoding table  */
char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*  decode 4 '6-bit' characters into 3 8-bit binary bytes
 *
 * @param in[]: array of char to decode
 * @param *clrStr: string decoded
 */
void decodeBlock(unsigned char in[], char *clrStr)
{
    unsigned char out[4];
    out[0] = in[0] << 2 | in[1] >> 4;
    out[1] = in[1] << 4 | in[2] >> 2;
    out[2] = in[2] << 6 | in[3] >> 0;
    out[3] = '\0';
    strncat(clrStr, out, sizeof(out));
}

/* decode a base 64 string to a characters string
 *
 * @param *b64src: string to decode
 * @param *clrDst: string decoded
 */
void b64Decode(char *b64src, char *clrDst)
{
    int c, phase, i;
    unsigned char in[4];
    char *p;

    clrDst[0] = '\0';
    phase = 0; i=0;
    while(b64src[i]) {
        c = (int) b64src[i];
        if(c == '=') {
            decodeBlock(in, clrDst);
            break;
        }
        p = strchr(b64, c);
        if(p) {
            in[phase] = p - b64;
            phase = (phase + 1) % 4;
            if(phase == 0) {
                decodeBlock(in, clrDst);
                in[0]=in[1]=in[2]=in[3]=0;
            }
        }
        i++;
    }
}

/*  encode 3 8-bit binary bytes as 4 '6-bit' characters
 *
 * @param in[]: array of char to encode
 * @param b64str[]: array of char encoded
 * @param len: length of array of encoded char
 */
void encodeBlock( unsigned char in[], char b64str[], int len )
{
    unsigned char out[5];
    out[0] = b64[ in[0] >> 2 ];
    out[1] = b64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? b64[ ((in[1] & 0x0f) << 2) |
                                             ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? b64[ in[2] & 0x3f ] : '=');
    out[4] = '\0';
    strncat(b64str, out, sizeof(out));
}

/*  base64 encode a stream, adding padding if needed
 *
 *  @param *clrStr: string to encode
 *  @param *b64dst: string encoded with base64
 */
void b64Encode(char *clrStr, char *b64dst)
{
    unsigned char in[3];
    int i, len = 0;
    int j = 0;

    b64dst[0] = '\0';
    while(clrStr[j]) {
        len = 0;
        for(i=0; i<3; i++) {
            in[i] = (unsigned char) clrStr[j];
            if (clrStr[j]) {
                len++; j++;
            }
            else in[i] = 0;
        }
        if (len) {
            encodeBlock(in,b64dst,len);
        }
    }
}