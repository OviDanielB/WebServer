//
// Created by ovi on 4/7/16.
//

#include "db_cache.h"

/* returns a char** with the names of the files in the directory specified by path
 * the char ** in the call     char** files = files_in_dir(path) DOESN'T HAVE TO BE INITIALIZED
 * the names of the files are at maximum 256 as defined in dirent.h
*/
char **files_in_dir(char *path){

    DIR *dir;
    struct dirent *ent;

    int fileCount = 0;
    char **files;

    //
    files = malloc(20 * sizeof(char *));
    if ((dir = opendir (path)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {

            /* doesn't put the . and .. directory*/
            if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0){
                continue;
            }
            files[fileCount] = malloc(256*sizeof(char));
            strcpy(files[fileCount],ent->d_name);
            printf("%s\n",files[fileCount]);

            if(fileCount>10){
                files = realloc(files,(fileCount + 1) * sizeof(char));
                if(files == NULL){
                    perror("Calloc error.");
                    exit(EXIT_FAILURE);
                }
            }
            printf("%d\n",fileCount);
            fileCount++;

        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("Could not open directory for images search.");
        return NULL;
    }

    return files;

}