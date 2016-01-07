#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "funciones.h"

int cuenta_comandos(char *c){
   char *ptr;
   char token[10] = "|<>>>";
   char s[strlen(c) + 1];
   int cont = 0;

   memcpy(s, c, strlen(c) + 1);
   ptr = strtok(s, token );
   cont++;

   while( (ptr = strtok( NULL, token )) != NULL ){
      cont++;
   }

   return cont;
}

int cuenta_simbolos(char *c){
   char *ptr;
   char s[strlen(c) + 1];
   char token[4] = " \n\t";
   int cont = 0;

   memcpy(s, c, strlen(c) + 1);
   ptr = strtok(s, token );

   while( (ptr = strtok( NULL, token )) != NULL ){
      if ((strcmp(ptr, "|") == 0) || (strcmp(ptr, "<") == 0) || (strcmp(ptr, ">") == 0) ||
         (strcmp(ptr, ">>") == 0)){
         cont++;
      }
   }

   return cont;
}

void elimina_espacios ( char *str ) {
    char *p_a = str, *p_b = str;
    char space_cnt = 0;

    while (*p_b == ' ') {
        p_b++;
    }

    while (*p_b != '\0') {
        *p_a = *p_b;
        if ( *p_b == ' ') {
            space_cnt++;
        } else {
            space_cnt = 0;
        }
        p_a++;
        p_b++;
    }

    *(p_a - space_cnt) = '\0';
}

int main()
{
   while(1){
       int n_simbolos = 0, n_comandos = 0, i = 0, j = 0, pipe = 0, k=0, num_bloques=0;

       char s1[500], s3[500], *ptr, *bloques [100];
       char token[4] = " \n\t", token2[10] = "|<>>>\n";

       printf("myBash$ ");
       fgets(s1, 100, stdin);
       if (s1[0] == '\n') {
           continue;
       } else if (strcmp (s1, "exit\n") == 0) {
           break;
       }

       bloques[0] = strtok ( s1, "\n;");
       num_bloques++;
       while ( (bloques[num_bloques] = strtok (NULL, "\n;")) != NULL ) {
           num_bloques++;
       }

       for (k=0; k<num_bloques; k++) {
           j=0;
           i=0;

           memcpy(s3, bloques[k], strlen(bloques[k])+1);

           n_comandos = cuenta_comandos(s3);
           n_simbolos = cuenta_simbolos(s3);

           char **simbolos = malloc((n_simbolos+1)*sizeof(char*));
           char **comandos = malloc((n_comandos+1)*sizeof(char*));

           // Busca por simbolos
           memcpy(s3, bloques[k], strlen(bloques[k])+1);
           ptr = strtok(s3, token);    // Primera llamada => Primer token

           while( (ptr = strtok( NULL, token ) ) != NULL ){
               if ((strcmp(ptr, "|") == 0) || (strcmp(ptr, "<") == 0) || (strcmp(ptr, ">") == 0) ||
                       (strcmp(ptr, ">>") == 0)){
                   simbolos[i] = malloc ((strlen(ptr)+1)*sizeof(char));
                   strcpy(simbolos[i], ptr);
                   i++;
               }
           }

           // Busca por comandos
           memcpy(s3, bloques[k], strlen(bloques[k])+1);
           ptr = strtok(s3, token2);    // Primera llamada => Primer token
           comandos[j] = malloc ((strlen(ptr)+1)*sizeof(char));
           strcpy(comandos[j], ptr);
           j++;

           while( (ptr = strtok(NULL, token2) ) != NULL ){
               comandos[j] = malloc ((strlen(ptr)+1)*sizeof(char));
               strcpy(comandos[j], ptr);
               elimina_espacios (comandos[j]);
               j++;
           }

           // *************** Comienza parte de llamado a las funciones ********************

           if(n_simbolos == 0)
               stdin_stdout(comandos[0]);
           else{
               i = 0;
               j = 0;
               while ( i != n_simbolos ) {
                   if(i == 0){ // Para correr el primer par de instrucciones
                       if(strcmp(simbolos[i], ">") == 0){
                           if( n_simbolos == 2 ) { 
                               if ( strcmp(simbolos[i+1], "<") == 0){
                                   file_to_file (comandos[j+2],
                                           comandos[j+1],
                                           comandos[j],
                                           1);
                                   break;
                               } 
                           }
                           stdin_to_file(comandos[j+1], comandos[j], 1);
                           break;
                       }
                       else if(strcmp(simbolos[i], ">>") == 0){
                           if( n_simbolos == 2 ) { 
                               if ( strcmp(simbolos[i+1], "<") == 0){
                                   file_to_file (comandos[j+2],
                                           comandos[j+1],
                                           comandos[j],
                                           2);
                                   break;
                               } 
                           }
                           stdin_to_file(comandos[j+1], comandos[j], 2);
                           break;
                       }
                       else if(strcmp(simbolos[i], "<") == 0){
                           if( n_simbolos >= 2 ) { 
                               if ( strcmp(simbolos[i+1], ">") == 0){
                                   file_to_file (comandos[j+1],
                                           comandos[j+2],
                                           comandos[j],
                                           1);
                                   break;
                               } else if ( strcmp(simbolos[i+1], ">>") == 0){
                                   file_to_file (comandos[j+1],
                                           comandos[j+2],
                                           comandos[j],
                                           2);
                                   break;
                               } else if ( strcmp(simbolos[i+1], "|") == 0 ){
                                   pipe = file_to_pipe (comandos [j+1], comandos [j]);
                                   j+=2;
                                   i+=2;
                               }

                           } else {
                               file_to_stdout (comandos [j+1], comandos [j]);
                               break;
                           }

                       }
                       else if(strcmp(simbolos[i], "|") == 0){
                           pipe = stdin_to_pipe(comandos[j]);
                           if(n_simbolos == 1)
                               pipe_to_stdout(pipe, comandos[j+1]);
                           j+=1;
                           i+=1;
                       }
                   }
                   else if(i != n_simbolos-1){ // Correr las instrucciones intermedias
                       if ( strcmp(simbolos[i], "|") == 0 ){
                           pipe = pipe_to_pipe(pipe, comandos[j]);
                           j+=1;
                           i+=1;
                       } else {
                           printf ("simbolo inesperado %s\n", simbolos[i]);
                           break;
                       }
                   }
                   else{
                       if(strcmp(simbolos[i], ">") == 0){
                           pipe_to_file(pipe, comandos[j+1], comandos[j], 1);
                           break;
                       }
                       else if(strcmp(simbolos[i], ">>") == 0){
                           pipe_to_file(pipe,comandos[j+1], comandos[j], 2);
                           break;
                       }
                       else {
                           pipe_to_stdout(pipe, comandos[j]);
                           break;
                       }
                   }
               }
           }
       } 
   }

   return 0;
}
