/**
 * Implementation of functions to manage FIFO communication between processes.
 */

#include "wurfl.h"

/*	 creating new FIFO as a file in file system	*/
void makeFifo(char *path)
{
    if (mkfifo(path, 0666) < 0) {
        perror("Error in creating fifo pipe");
        exit(EXIT_FAILURE);
    }
}

/* Opening caller child own writer FIFO	*/
int openChildOwnFifo_w()
{
	int fifo;
	char fifo_path[30];
	pid_t pid = getpid();

	/* to identify own process FIFO, it's saved with pid number as name
	 * adding "w" or "r" to indicate way of opening	*/
	sprintf(fifo_path, "/tmp/%ldw", (long) pid);

	/* opening FIFO in writing way	*/
	fifo = open(fifo_path, O_WRONLY);
	if (fifo == -1) {
		perror("Child fifo open error\n");
		exit(EXIT_FAILURE);		
	}
	return fifo;
}

/* Opening caller child own reader FIFO	*/
int openChildOwnFifo_r()
{
	int fifo;
	char fifo_path[30];
	pid_t pid = getpid();

	/* to identify own process FIFO, it's saved with pid number as name
	 * adding "w" or "r" to indicate way of opening	*/
	sprintf(fifo_path, "/tmp/%ldr", (long) pid);

	/*	opening fifo in reading way	*/
    fifo = open(fifo_path, O_RDONLY);
	if (fifo == -1){
		perror("Child fifo open");
		exit(EXIT_FAILURE);		
	}
	return fifo;
}

/** Check if file exists:
 * returns 1 if file exists, 0 if it doesn't.
 *
 * @param path : file path to check existence of
 */
int fileExist(char *path)
{
	if( access(path, F_OK) != -1){
		/* file exists */
		return TRUE;
	}else{
		/* file doesn't exist */
		return FALSE;
	}
}

/* Creates fifo for every child , called with child's pid number
 *
 * @param pids : array of children processes pids
 */
void createChildrenFifo_w(pid_t *pids)
{
	int i;
	/*	initialization of buffer for path of FIFO to create	*/
	char *file_name = malloc(30 * sizeof(char));
	if (file_name == NULL) {
		perror("malloc error");
		exit(EXIT_FAILURE);
	}

	/* create a writing FIFO for each child	*/
	for(i = 0; i< CHILDREN_NUM; i++){
		sprintf(file_name, "/tmp/%ldw", (long)pids[i]);

        if(!fileExist(file_name)){
			makeFifo(file_name);
            printf("created fifo %s\n", file_name);
		}
	}
}

/* Creates fifo for every child , called with child's pid number
 *
 * @param pids : array of children processes pids
 */
void createChildrenFifo_r(pid_t *pids)
{
	int i;
	/*	initialization of buffer for path of FIFO to create	*/
	char *file_name = malloc(30 * sizeof(char));
	if (file_name == NULL) {
		perror("malloc error\n");
		exit(EXIT_FAILURE);
	}

	/* create a reading FIFO for each child	*/
	for(i = 0; i < CHILDREN_NUM; i++){
		sprintf(file_name, "/tmp/%dr", pids[i]);

		if (!fileExist(file_name)){
			makeFifo(file_name);
            printf("created fifo %s\n", file_name);
        }
	}
}

/* helper function to read message useful to remove a char in a string
 *
 * @param str: string to remove char from
 * @param garbage : char to remove
 */
void removeChar(char *str, char garbage)
{
	char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}

/**	Fill struct device with information about client device received from PHP process
 * as successful result of query through user-agent
 *
 * @param device: struct device to fill
 * @param msg: received message to get information from
 */
void fillDeviceStruct(struct device *device, char *msg);
void fillDeviceStruct(struct device *device, char *msg)
{
	/*	initialized variables of elements to read from msg	*/
	char id[100], width[10], height[10], colors[20];
	bool jpg = FALSE, png = FALSE, gif = FALSE;

	char *p;
	int i = 0;

	removeChar(msg, '?');
	removeChar(msg,'!');
	p = strtok(msg, "|");

	/*	message parsing to collect information	*/
	while (p != NULL){
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

	/* filling struct device */
	strcpy(device->id, id);
	device->width = (size_t) strtol(width,NULL,10);
	device->height = (size_t) strtol(height,NULL,10);
	device->colors = strtol(colors,NULL,10);
	device->jpg = jpg;
	device->png = png;
	device->gif = gif;
}

/* Request of the characteristics supported by the client device
 * describes by the given user agent string
 *
 * @param userAgent : user agent string from client HTTP request
 * @param device : struct device to fill with the result of request
*/
void getDeviceByUserAgent(char *userAgent, struct device *device)
{
	/*	opening FIFO for writing	*/
	int write_fd = openChildOwnFifo_w();
	/* 	opening FIFO for reading	*/
	int read_fd = openChildOwnFifo_r();

	/* initialization of buffer of message to write/read on/from FIFO	*/
	char *msg = (char *) malloc(MAXLINE*sizeof(char));
	if(msg == NULL){
		perror("malloc error");
		exit(EXIT_FAILURE);
	}

	char ua[strlen(userAgent)+2];
	sprintf(ua, "%s\n",userAgent);
	/* write the ua string to the fifo to the php process */
	write(write_fd, ua, strlen(ua));
	/* read from the fifo an only string containing all the device's information;
	* the string starts and ends with '?!' an between every property there is '£' */
	read(read_fd, msg, MAXLINE);

	/* collect information by result of query into struct device	*/
	fillDeviceStruct(device, msg);
}

/**  Creation of a new PHP process and two FIFO file to communication
 * between the new process and the children ones
 *
 * @param pids : array of children pids
 */
void initializeFifo(pid_t *pids)
{
    pid_t pid;

    if ((pid = fork()) == 0) {
        /*  main process creates PHP process to query WURFL file    */
        printf("Launching PHP process...\n");

		/* composing array of children pid as strings as input for fifo.php process	*/
		char **pds;
		pds = (char **) malloc(CHILDREN_NUM*sizeof(char *));
		if (pds == NULL) {
			perror("malloc error");
			exit(EXIT_FAILURE);
		}
		int i;
		for (i = 0; i < CHILDREN_NUM; i++) {
			pds[i] = (char * ) malloc(10*sizeof(char));
			if (pds[i] == NULL) {
				perror("malloc error");
				exit(EXIT_FAILURE);
			}
			sprintf(pds[i], "%d", pids[i]);
        }

		/*	launching PHP process with pids as arguments	*/
		switch (CHILDREN_NUM) {
			case 1:
				execlp(PHP_PATH, PHP_PATH, "-f", FIFO_PATH,
					   pds[0], (char *) NULL);
				break;
			case 3:
				execlp(PHP_PATH, PHP_PATH, "-f", FIFO_PATH,
					   pds[0], pds[1], pds[2], (char *) NULL);
				break;
			case 4:
				execlp(PHP_PATH, PHP_PATH, "-f", FIFO_PATH,
					   pds[0], pds[1], pds[2], pds[3], (char *) NULL);
				break;
			case 5:
				execlp(PHP_PATH, PHP_PATH, "-f", FIFO_PATH,
					   pds[0], pds[1], pds[2], pds[3], pds[4], (char *) NULL);
				break;
			case 6:
				execlp(PHP_PATH, PHP_PATH, "-f", FIFO_PATH,
					   pds[0], pds[1], pds[2], pds[3], pds[4], pds[5], (char *) NULL);
				break;
			case 7:
				execlp(PHP_PATH, PHP_PATH, "-f", FIFO_PATH,
					   pds[0], pds[1], pds[2], pds[3], pds[4], pds[5], pds[6], (char *) NULL);
				break;
			case 8:
				execlp(PHP_PATH, PHP_PATH, "-f", FIFO_PATH,
					   pds[0], pds[1], pds[2], pds[3], pds[4], pds[5], pds[6], pds[7], (char *) NULL);
				break;
			default : // DEFAULT_CHILD = 2
				execlp(PHP_PATH, PHP_PATH, "-f", FIFO_PATH,
					   pds[0], pds[1],  (char *) NULL);
				break;
		}
    } else if (pid == -1) {
        perror("fork error\n");
    	exit(EXIT_FAILURE);
    }

    /*  created children fifo by main process for writing and reading */
	createChildrenFifo_w(pids);
	createChildrenFifo_r(pids);
}
