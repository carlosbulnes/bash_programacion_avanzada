// ls -l --color | cat > asd.txt

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
//   printf("cuenta_comandos: %s\n", ptr);
   cont++;

   while( (ptr = strtok( NULL, token )) != NULL ){
//      printf("cuenta_comandos: %s\n", ptr);
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
//         printf("cuenta_comandos: %s\n", ptr);
         cont++;
      }
   }

   return cont;
}


int main()
{
   while(1){
      int n_simbolos = 0, n_comandos = 0, i = 0, j = 0, pipe = 0;

      char s1[500], s3[500], *ptr;
      char token[4] = " \n\t", token2[10] = "|<>>>";

      printf("myBash$ ");
      fgets(s1, 100, stdin);

      memcpy(s3, s1, strlen(s1)+1);
//      printf("original string: %s, c: %s\n", s1, s3);
//      printf("n_simbolos: %d, n_comandos: %d\n", n_simbolos, n_comandos);

      //define_tam(s3);
      n_comandos = cuenta_comandos(s3);
      n_simbolos = cuenta_simbolos(s3);

//      printf("original string: %s, c: %s\n", s1, s3);
      printf("n_simbolos: %d, n_comandos: %d\n", n_simbolos, n_comandos);

//      char **array = malloc((n_simbolos+1)*sizeof(char*));
      char **simbolos = malloc((n_simbolos+1)*sizeof(char*));
      char **comandos = malloc((n_comandos+1)*sizeof(char*));


      // Busca por simbolos
      memcpy(s3, s1, strlen(s1)+1);
      ptr = strtok(s3, token);    // Primera llamada => Primer token

      while( (ptr = strtok( NULL, token ) ) != NULL ){
         if ((strcmp(ptr, "|") == 0) || (strcmp(ptr, "<") == 0) || (strcmp(ptr, ">") == 0) ||
            (strcmp(ptr, ">>") == 0)){
            simbolos[i] = malloc ((strlen(ptr)+1)*sizeof(char));
            strcpy(simbolos[i], ptr);
//            printf("simbolos[%d]; %s\n", i, simbolos[i]);
            i++;
         }
      }

      // Busca por comandos
      memcpy(s3, s1, strlen(s1)+1);
      ptr = strtok(s3, token2);    // Primera llamada => Primer token
      comandos[j] = malloc ((strlen(ptr)+1)*sizeof(char));
      strcpy(comandos[j], ptr);
//      printf("comandos[%d]; %s\n", j, comandos[j]);
      j++;

      while( (ptr = strtok(NULL, token2) ) != NULL ){
            comandos[j] = malloc ((strlen(ptr)+1)*sizeof(char));
            strcpy(comandos[j], ptr);
            comandos[j] = comandos[j] + 1;
            comandos[j][strlen(ptr)] = '*';
//            printf("comandos[%d]; %s\n", j, comandos[j]);
            j++;
      }

      // *************** Comienza parte de llamado a las funciones ********************

      if(n_simbolos == 0)
         stdin_stdout(comandos[0]);
      else{
         for(i = 0, j = 0; i < n_simbolos; i++, j++){
            if(i == 0){ // Para correr el primer par de instrucciones
               if(strcmp(simbolos[i], ">") == 0){
                  stdin_to_file(comandos[j+1], comandos[j], 1);
               }
               else if(strcmp(simbolos[i], ">>") == 0){
                  stdin_to_file(comandos[j+1], comandos[j], 2);
               }
               else if(strcmp(simbolos[i], "|") == 0){
                  pipe = stdin_to_pipe(comandos[j]);
                  if(n_simbolos == 1)
                     pipe_to_stdout(pipe, comandos[j+1]);
               }
            }
            else if(i != n_simbolos-1){ // Correr las instrucciones intermedias
               pipe = pipe_to_pipe(pipe, comandos[j]);
            }
            else{
               if(strcmp(simbolos[i], ">") == 0){
                  pipe_to_file(pipe, comandos[j+1], comandos[j], 1);
               }
               else if(strcmp(simbolos[i], ">>") == 0){
                  pipe_to_file(pipe,comandos[j+1], comandos[j], 2);
               }
               else if(strcmp(simbolos[i], "|") == 0){
                  pipe_to_stdout(pipe, comandos[j+1]);
               }
            }
         }
      }

   }

   return 0;
}
