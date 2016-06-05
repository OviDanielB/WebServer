/**
 *  Implementation of helper functions.
 */

#include "helper.h"

/* Hash function to calculate an (almost) unique identifier for every manipulated image
 * from the resource name, where are indicated the adapted values or the original ones.
 *
 * @param: name = string formatted as <originalname><width><height><qualityfactor><type><colorsnum>
 */
unsigned long getHashCode(unsigned char *name)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *name++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

/*  remove from file system file in server cache identified by name */
void removeFromDisk(char *name)
{
    char *filename = (char *) malloc(MAXLINE*sizeof(char));
    if (filename == NULL) {
        perror("malloc error");
        exit(EXIT_FAILURE);
    }

    sprintf(filename, "%s%s.*", CACHE_PATH, name);

    if (remove(filename) == -1) {
        perror("remove error");
        exit(EXIT_FAILURE);
    }
}

/*  read a line of data of length maxlen    */
int readline(int fd, void *buf, int maxlen)
{
    int n;
    ssize_t rc;
    char c, *ptr;

    ptr = buf;
    for (n = 1; n < maxlen; n++) {
        if ((rc = read(fd, &c, 1)) == 1) {
            *ptr++ = c;
            if (c == '\n') break;	/* letto newline */
        }
        else
        if (rc == 0) {		/* read ha letto l'EOF */
            if (n == 1) return(0);	/* esce senza aver letto nulla */
            else break;
        }
        else return(-1);		/* errore */
    }

    *ptr = 0;	/* per indicare la fine dell'input */
    return(n);	/* restituisce il numero di byte letti */
}

/*  delete all images in server cache directory in file system  */
void removeAllCacheFromDisk()
{
    char *filename = (char *) malloc(MAXLINE*sizeof(char));
    if (filename == NULL) {
        perror("malloc error");
        exit(EXIT_FAILURE);
    }

    sprintf(filename, "%s*", CACHE_PATH);

    if (remove(filename) == -1) {
        perror("remove error");
        exit(EXIT_FAILURE);
    }
}

size_t dim[2];
/*  Calculation of image's dimensions, maintaining the aspect ratio
 *
 * @param original_w: original image width
 * @param original_h: original image height
 * @param adapted_w: image width requested
 * @param adapted_h: image height requested
 */
size_t *proportionalSize(size_t original_w, size_t original_h, size_t adapted_w, size_t adapted_h)
{
    double orw = (double) original_w;
    double orh = (double) original_h;
    double adw = (double) adapted_w;
    double adh = (double) adapted_h;
    double r = orw/orh;

    if (adw/adh == r) {
        dim[0] = adapted_w , dim[1] = adapted_h;
        return dim;
    } else if (original_w>=original_h){
        dim[0] = adapted_w;
        dim[1] = (size_t) adw/r;
        return dim;
    }
    dim[0] = (size_t) adh*r;
    dim[1] = adapted_h;
    return dim;
}

/*  get IP address of server acquired in LAN */
char *getServerIp()
{
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;
    char addressBuffer[INET_ADDRSTRLEN];

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            /* considered only LAN  */
            if (strcmp(ifa->ifa_name, "wlan0") == 0) {
                return &addressBuffer[0];
            }
        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
}


/*  Read name and extension of a file   */
void readNameAndType(char *s, char *name, char *ext)
{
    if (memset(name,'\0',256)==NULL) {
        perror("memset error\n");
        exit(EXIT_FAILURE);
    }
    if (memset(ext,'\0',4)==NULL) {
        perror("memset error\n");
        exit(EXIT_FAILURE);
    }

    char *p = s;

    int i=0;
    while (*p!='.') {
        name[i] = *p;
        i++;
        p++;
    }
    p++;
    i=0;
    while (*p!='\0') {
        ext[i] = *p;
        i++;
        p++;
    }
}