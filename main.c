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
      char token[4] = " \n\t", token2[10] = "|<>>>\n";

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

      //enum src_types {STDINPUT, IN_FILE, PIPE};
      //enum dst_types {STDOUTPUT, OUT_FILE, PIPE};

      //src_types from = STDINPUT;
      //dst_types from = STDOUTPUT;

      if(n_simbolos == 0)
         stdin_stdout(comandos[0]);
      else{
          i = 0;
          j = 0;
          while ( i != n_simbolos ) {
              //for(i = 0, j = 0; i < n_simbolos; i++, j++){
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
                      printf ("comando: %s, archivo: %s\n", comandos[j], comandos[j+1]);
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
                              file_to_file (comandos[j+2],
                                      comandos[j+1],
                                      comandos[j],
                                      1);
                              break;
                          } else if ( strcmp(simbolos[i+1], ">>") == 0){
                              file_to_file (comandos[j+2],
                                      comandos[j+1],
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

   return 0;
}
