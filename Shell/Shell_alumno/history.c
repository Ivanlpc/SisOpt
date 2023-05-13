#include "history.h"
#include "util.h"
#define CLEAN_CURRENT_LINE() printf("\r\033[K");
char getch();
void read_input(char inputBuffer[], int size, int * length, command* list) {
	int i = 0;
    int his_pos = 0;
	while(1) {
		
		int ch = getch();
		if(ch == 4){
			*length = 0;
			break;
		};
		if(ch == '\n') {
			inputBuffer[i] = ch;
            i++;
			*length = i;
			break;
		} else if (ch == 27) {
			ch = getch();
			if(ch == 91) {
				ch = getch();
				if(ch == 65) {
					command * last_command = get_command_bypos(list, his_pos + 1);
					if(last_command != NULL) {
						his_pos++;
						*length = last_command->length;
						i = *length;
						CLEAN_CURRENT_LINE()
						printf("COMMAND->%s", last_command->args);
						strcpy(inputBuffer, last_command -> args);
						
            			fflush(stdout);
					} 
				}else if(ch == 66) {
					if(his_pos == 0) continue;
					
					command * last_command = get_command_bypos(list, his_pos - 1);
					if(last_command != NULL) {
						his_pos--;
						*length = last_command->length;
						i = *length;
						CLEAN_CURRENT_LINE()
						printf("COMMAND->%s", last_command->args);
						strcpy(inputBuffer, last_command -> args);
						
            			fflush(stdout);

					}
					else {
						his_pos = 0;
						CLEAN_CURRENT_LINE()
						printf("COMMAND->");
						i = 0;
						*length = 0;
					}
				}
				
			}
			
		}else if(ch == 127 ) { // If the user presses backspace or delete
            if(i > 0) { // If there are characters in the string
                i--; // Move the index back by one
				*length = i - 1;

                inputBuffer[i] = '\0'; // Set the last character to null to remove it from the string
                CLEAN_CURRENT_LINE() // Clear the entire input line
                printf("COMMAND->%s", inputBuffer); // Print the updated string
				fflush(stdout);
            }
        }else { // If the user presses any other key
			if(i < size - 1){
				inputBuffer[i] = ch; // Add the character to the string
            i++; // Move the index forward by one
			
            printf("%c", ch); // Print the character to the screen
			fflush(stdout);
			}
        }
	}
}

command *get_command_bypos(command *list, int n)
{
	command *aux = list;
	if (n < 1)
		return NULL;
	n--;
	while (aux->next != NULL && n)
	{
		aux = aux->next;
		n--;
	}
	return aux->next;
};

command *new_command(char *args, int length) {
	command *aux;
	aux = (command *)malloc(sizeof(command));
	/* Si se ha indicado un comando en el trabajo, copiamos el array de argumentos al trabajo */
	if(args != NULL) {
		aux -> args = strndup(args, length);
		aux ->length = length;

	} else {
		aux ->args = NULL;
		aux -> length = 0;
	}
	aux->next = NULL;

	return aux;
}
void add_command(command *list, command *item)
{
	command *aux = list->next;
	list->next = item;
	item->next = aux;
}
void print_command_item(command *item, int n) {

	printf("%d %s\n", n, item->args);

}
void print_command_list(command *list) {
	int n = 1;
	command * command = get_command_bypos(list, n);
	while(command != NULL) {
		print_command_item(command, n);
		n++;
		command = get_command_bypos(list, n);
	}
}
char getch()
{
 int shell_terminal = STDIN_FILENO;
 struct termios conf;
 struct termios conf_new;
 char c;

 tcgetattr(shell_terminal,&conf); /* leemos la configuracion actual */
 conf_new = conf;

 conf_new.c_lflag &= (~(ICANON|ECHO)); /* configuramos sin buffer ni eco */
 conf_new.c_cc[VTIME] = 0;
 conf_new.c_cc[VMIN] = 1;

 tcsetattr(shell_terminal,TCSANOW,&conf_new); /* establecer configuracion */

 c = getc(stdin); /* leemos el caracter */
 
 tcsetattr(shell_terminal,TCSANOW,&conf); /* restauramos la configuracion */
 return c;
} 