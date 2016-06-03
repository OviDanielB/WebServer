#include <errno.h>
#include "wurfl.h"

void makeFifo(char *path)
{
    if (mkfifo(path, 0666) < 0) {
        perror("Error in creating fifo pipe");
        exit(EXIT_FAILURE);
    }
    	
}
/* opens the fifo for WRITING at path and returns the file descriptor on success */
/*int open_fifo_w(char *path){

	int fifo_fd;
	fifo_fd = open(path, O_WRONLY);
    if(fifo_fd < 0){
    	perror("Fifo open error");
    	exit(EXIT_FAILURE);
    }
    return fifo_fd;
}*/

/* opens the fifo for READING at path and returns the file descriptor on success */
/*int open_fifo_r(char *path){

	int fifo_fd;
	fifo_fd = open(path, O_RDONLY);
    if(fifo_fd < 0){
    	perror("Fifo open error");
    	exit(EXIT_FAILURE);
    }
    return fifo_fd;
}*/

/* opens and returns own child fifo
	NEVER CLOSE FIFO FILE DESCRIPTOR
	*/ 
int openChildOwnFifo_w()
{
	int fifo;
	char fifo_path[30];
	pid_t pid = getpid();
	
	sprintf(fifo_path, "/tmp/%ldw", (long) pid);
	//sprintf(fifo_path, "/tmp/%ld", (long) pid);

	printf("opening fifo %s for writing\n", fifo_path);

	fifo = open(fifo_path, O_WRONLY);
	if (fifo == -1) {
		perror("Child fifo open error\n");
		exit(EXIT_FAILURE);		
	}
	
	return fifo;
}

int openChildOwnFifo_r()
{
	int fifo;
	char fifo_path[30];
	pid_t pid = getpid();
	
	sprintf(fifo_path, "/tmp/%ldr", (long) pid);
	//sprintf(fifo_path, "/tmp/%ld", (long) pid);

	printf("opening fifo %s for reading\n", fifo_path);

    fifo = open(fifo_path, O_RDONLY);
	if (fifo == -1){
		perror("Child fifo open");
		exit(EXIT_FAILURE);		
	}
	
	return fifo;
}
    
void writeToFifo(int fifo, char *msg)
{
    size_t len = strlen(msg);
    
    if (write(fifo,msg,len) < 0){
    	perror("Fifo write error");
    	exit(EXIT_FAILURE);
    }
}

/* returns 1 if file exists, 0 if it doesn't */
int fileExist(char *path)
{
	/* check if file exists*/
	
	if( access(path, F_OK) != -1){
		/* file exists */
		return TRUE;
	}else{
		/* file doesn't exist */
		return FALSE;
	}
}

/* creates fifo for every child , called with child's pid number */
/*void createChildrenFifo(pid_t *pids)
{
	int i;
	char *file_name;

	file_name = malloc(30 * sizeof(char));
	for(i=0; i< CHILDREN_NUM; i++){
		sprintf(file_name, "/tmp/%ld", (long)pids[i]);

		if(!fileExist(file_name)){
			makeFifo(file_name);
			printf("created fifo %s\n", file_name);
		}
	}

}*/

/* creates fifo for every child , called with child's pid number */
void createChildrenFifo_w(pid_t *pids)
{
	int i;
	char *file_name;
	
	file_name = malloc(30 * sizeof(char));
	for(i=0; i< CHILDREN_NUM; i++){
		sprintf(file_name, "/tmp/%ldw", (long)pids[i]);

        if(!fileExist(file_name)){
			makeFifo(file_name);
            printf("created fifo %s\n", file_name);
		}
	}
	
}

void createChildrenFifo_r(pid_t *pids)
{
	int i;
	char *file_name;
	
	file_name = malloc(30 * sizeof(char));
	if (file_name == NULL) {
		perror("malloc error\n");
		exit(EXIT_FAILURE);
	}

	for(i = 0; i < CHILDREN_NUM; i++){
		sprintf(file_name, "/tmp/%dr", pids[i]);

		if (!fileExist(file_name)){
			makeFifo(file_name);
            printf("created fifo %s\n", file_name);
        }
	}
}

void removeChar(char *str, char garbage)
{
	char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}

void fillDeviceStruct(struct device *device, char *msg);
void fillDeviceStruct(struct device *device, char *msg)
{
	char id[100], width[10], height[10], colors[20];
	bool jpg=FALSE, png=FALSE, gif=FALSE;
	char *p;
	int i = 0;
	
	printf("%s\n",msg);
	
	removeChar(msg, '?');
	removeChar(msg,'!');
	printf("%s\n",msg);
	
	p = strtok(msg, "|");
	
	while( p != NULL){
		//printf(" p = %s len = %ld\n",p,strlen(p));
		
		switch(i){
			case 0:
				strcpy(id,p);
				i++;
				break;
			case 1:
				strcpy(width,p);
				i++;
				break;
			case 2:
				strcpy(height,p);
				i++;
				break;
			case 3:
				strcpy(colors,p);
				i++;
				break;
			case 4:
				if (strcmp(p,"true") == 0){
					jpg = TRUE;
				} else {
					jpg = FALSE;
				}
				i++;
				break;
			case 5:
				if (strcmp(p,"true") == 0){
					png = TRUE;
				} else {
					png = FALSE;
				}
				i++;
				break;
			case 6:
				if (strcmp(p,"true") == 0){
					gif = TRUE;
				} else{
					gif = FALSE;
				}
				i++;
				break;
			default:
				break;
			
		}
		p = strtok(NULL, "|");
	}
	
	strcpy(device->id, id);
	device->width = (size_t) strtol(width,NULL,10);
	device->height = (size_t) strtol(height,NULL,10);
	device->colors = strtol(colors,NULL,10);
	device->jpg = jpg;
	device->png = png;
	device->gif = gif;
}

/* returns a struct device * containing info of the characteristics supported by the client device
 * describes by the given user agent string
 *
 * @params userAgent : user agent string from client HTTP request
*/
void getDeviceByUserAgent(char *userAgent, struct device *device)
{
	sleep(5);
	int write_fd = openChildOwnFifo_w();

	int read_fd = openChildOwnFifo_r();
	printf("fifo r opened\n");
	printf("fifo w opened\n");

	/*struct device *device = (struct device *) malloc(sizeof(device));
	if (device == NULL){
		perror("malloc error");
		exit(EXIT_FAILURE);
	}*/
	
	char *msg = (char *) malloc(1024*sizeof(char));
	if(msg == NULL){
		perror("malloc error");
		exit(EXIT_FAILURE);
	}
	char ua[strlen(userAgent)+2];
	sprintf(ua, "%s\n",userAgent);
	/* write the ua string to the fifo to the php process */
	write(write_fd, ua, strlen(ua));
	
	/* read from the fifo an only string containing all the device's information;
	* the string starts and ends with '?!' an between every property there is '£'
	*/
	read(read_fd, msg, 1024);

	fillDeviceStruct(device, msg);

	printf("id = %s\n",device->id);
	printf("width = %ld\n",device->width);
	printf("height = %ld\n",device->height);
	printf("colors = %ld\n",device->colors);
	printf("jpg = %d\n",device->jpg);
	printf("png = %d\n",device->png);
	printf("gif = %d\n",device->gif);
	
	//printf("%ld",strlen(msg));
	//return device;
}

void initializeFifo(pid_t *pids)
{
    pid_t pid;

    if ((pid = fork()) == 0) {
        /*  beginnig php process to query WURFL file    */
        printf("beginning php process...\n");

        /*  composing argv as input for fifo.php process   */
        int i;
        /*char ps[CHILDREN_NUM*sizeof(int)];
        sprintf(ps, "%d", pids[0]);
        for (i = 1; i < CHILDREN_NUM; i++) {
            char p[sizeof(int)];
            sprintf(p, " %d", pids[i]);
            strcat(ps, p);
        }*/

		char **pds;
		pds = (char **) malloc(CHILDREN_NUM*sizeof(char*));
		//sprintf(ps[], "%d", pids[0]);
		for (i = 0; i < DEFAULT_CHILDREN; i++) {
			pds[i] = (char * ) malloc(10*sizeof(char));
			sprintf(pds[i], "%d\0", pids[i]);
            printf("pid%d :%s\n",i,pds[i]);
        }

        /*char b[50];
        sprintf(b, "php -f /home/laura_trive/ClionProjects/WebServer/php/fifo.php %s\n", ps);
        system(b);*/
        //execvp( "fifo.php", (void*) pids);
        printf("pid1 :%s\n",pds[0]);
        printf("pid2 :%s\n",pds[1]);
        execlp("/usr/bin/php", "/usr/bin/php", "-f", "/home/laura_trive/ClionProjects/WebServer/php/fifo.php",
			   pds[0], pds[1],  (char *) NULL);
    } else if (pid == -1) {
        perror("fork error\n");
    	exit(EXIT_FAILURE);
    }

    /*  created children fifo by father for writing and reading */
	createChildrenFifo_w(pids);
	createChildrenFifo_r(pids);
	//createChildrenFifo(pids);

    printf("created children fifo by father for writing and reading\n");
}

/*int main(int argc, char* argv[])
{
    int fifo_fd, fifo_r ; 
    char *fifo_name = "/tmp/1234w";
    char *fifo_name_r = "/tmp/1234r";
    char * msg = malloc(1024 * sizeof(char));
    device * device;
    pid_t pid;
    
    switch (pid = fork()){
    	case -1:
    		exit(EXIT_FAILURE);
    	case 0:
    		execlp("/usr/bin/php","/usr/bin/php" ,"-f fifo.php","1234");
    }

    // char user_agent[1024];
    //char *user_agent = "Mozilla/5.0 (iPhone; CPU iPhone OS 5_0 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Version/5.1 Mobile/9A334 Safari/7534.48.3\n";
    char *user_agent = "Mozilla/5.0 (Linux; Android 4.2.2; nl-nl; SAMSUNG GT-I9505 Build/JDQ39) AppleWebKit/535.19 (KHTML, like Gecko) Version/1.0 Chrome/18.0.1025.308 Mobile Safari/535.19";
    pid_t pids[NUM_CHILD] = {123, 1231, 12342, 1212};

    
    //createChildrenFifo_r(pids);
    
    //makeFifo(fifo_name_r);
    
    
    fifo_fd = open_fifo_w(fifo_name);
    fifo_r = open_fifo_r(fifo_name_r);
    
    device = getDeviceByUserAgent(user_agent,fifo_fd,fifo_r);
    
    printf("id = %s\n",device->id);
    printf("width = %d\n",device->width);
    printf("height = %d\n",device->height);
    printf("colors = %ld\n",device->colors);
    printf("jpg = %d\n",device->jpg);
    printf("png = %d\n",device->png);
    printf("gif = %d\n",device->gif);
    

        
    printf("server exit successfully\n");
    return EXIT_SUCCESS;
}*/

