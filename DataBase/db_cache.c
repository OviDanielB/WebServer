//
// Created by ovi on 4/7/16.
//

#include "db_cache.h"

/* returns a char** with the names of the files in the directory specified by path
 * the char ** in the call     char** files = files_in_dir(path) DOESN'T HAVE TO BE INITIALIZED
 * the names of the files are at maximum 256 as defined in dirent.h
*/
void images_in_dir(char *path,struct img **images){

    DIR *dir;
    struct dirent *ent;

    int fileCount = 0;
    char complete_path[1024];

    MagickWand *m_wand = NULL;
    size_t width,height;

    m_wand = NewMagickWand();

    //
    images = malloc(30 * sizeof(struct img *));
    if ((dir = opendir (path)) != NULL) {
        /* print all the files and directories within directory */

        while ((ent = readdir (dir)) != NULL) {

            /* doesn't put the . and .. directory*/
            if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0){
                continue;
            }

            strcat(complete_path,path);
            strcat(complete_path,"/");
            strcat(complete_path,ent->d_name);
            printf("%s\n",complete_path);

            if(MagickReadImage(m_wand,strcat(path,ent->d_name)) == MagickFalse){
                perror("MagickReadImage error");
                exit(EXIT_FAILURE);
            }



            width = MagickGetImageWidth(m_wand);
            height =MagickGetImageHeight(m_wand);

            images[fileCount] = malloc(sizeof(struct img));
            if(images[fileCount] == NULL){
                perror("Malloc error.");
                exit(EXIT_FAILURE);
            }

            strcpy((images[fileCount])->name,ent->d_name);
            images[fileCount]->width =(size_t) 500;  // width;
            images[fileCount]->height =(size_t) 600 ; //height
            printf("name = %s, width = %ld, height = %ld\n",images[fileCount]->name,images[fileCount]->width,images[fileCount]->height);

            if(fileCount>10){
                images = realloc(images,(fileCount + 1) * sizeof(char));
                if(images == NULL){
                    perror("Calloc error.");
                    exit(EXIT_FAILURE);
                }
            }
            printf("%d\n",fileCount);
            fileCount++;

            complete_path[0]=0;



        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("Could not open directory for images search.");
        return;
    }

    return;

}