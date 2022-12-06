#include <stdio.h>
#include <stdlib.h>

const int token_size = 14;

int main()
{
    char *buffer = NULL;
    size_t characters;

    getline(&buffer,&characters,stdin);
    
    char *begin = buffer, *active = buffer + token_size-1, *end = buffer + characters;
    
    while(active < end) 
    {
        for(char* test = active;test>=active - (token_size-2);test--) 
          for(char* against = test-1;against>=active-(token_size-1);against--)
          {
//          printf("test = %zu , against = %zu, values = %d , %d \n",test,against,*test,*against);
            if(*test == *against)
            {
              active = against+token_size;
              goto cnt;
            }
          }
        
        active++;
        break;
      cnt:;
    }
    if(active > end)
        active = end;
    
    printf("%zu\n", active-begin);
    
    free(buffer);
    exit(0);
}
