/*
  The tic-tac-toe program of server.
*/

#include <stdio.h>
#include <ulfius.h>

#define PORT 8080
#define ZERO 0
#define ONE 1
#define TWO 2
#define NINE 9
#define BOARD "board"

/*
  Return the result of the game. 
  Return 1 if computer or client wins.
  Return 2 if draw.
  Return 0 otherwise.
  Given: the current cheerboard array.
  Returns: 1 if computer or client wins,
  2 if draw, 0 otherwise.
*/
  int win(int cur[]) {  
    if (cur[0] == cur[1] && cur[1] == cur[2] && cur[2] != ZERO) {
      return ONE;
    }
    if (cur[3] == cur[4] && cur[4] == cur[5] && cur[5] != ZERO) {
      return ONE;
    }
    if (cur[6] == cur[7] && cur[7] == cur[8] && cur[8] != ZERO) {
      return ONE;
    }
    if (cur[0] == cur[3] && cur[3] == cur[6] && cur[6] != ZERO) {
      return ONE;
    }
    if (cur[1] == cur[4] && cur[4] == cur[7] && cur[7] != ZERO) {
      return ONE;
    }
    if (cur[2] == cur[5] && cur[5] == cur[8] && cur[8] != ZERO) {
      return ONE;
    }
    if (cur[0] == cur[4] && cur[4] == cur[8] && cur[8] != ZERO) {
      return ONE;
    }
    if (cur[2] == cur[4] && cur[4] == cur[6] && cur[6] != ZERO) {
      return ONE;
    }
    if (cur[0] != ZERO && cur[1] != ZERO && cur[2] != ZERO 
      && cur[3] != ZERO && cur[4] != ZERO && cur[5] != ZERO 
      && cur[6] != ZERO && cur[7] != ZERO && cur[8] != ZERO) {
      return TWO;
    } 
    return ZERO;  
  } 

/*
  Do deep-first search to find maximal solution for computer.
  Given: compare: If computer equals to 1, it's computer's turn.
                  If computer equals to 0, it's player's turn.
         temp: a copy array to do dfs.
  Returns: 1 if finds maximal solution, 0 if not.
*/
  int dfs(int compare, int temp[]) {  
    int state = -ONE, n = ZERO;  
    for (int x = ZERO; x < NINE; x++) {
      if (temp[x] != ZERO) {
        n++;
      }
    }  
    if (n == NINE) {
      return ZERO;
    }  
    for (int x = ZERO; x < NINE; x++) {
      if (temp[x] == ZERO) {  
        if (compare == ONE) {
          temp[x] = TWO;
        } else {
          temp[x] = ONE;  
        }
        if (win(temp) == ONE) {  
          temp[x] = ZERO;  
          return ONE;  
        }  
        int boo = (compare == ZERO ? ONE : ZERO);
        int cur = dfs(boo, temp);  
  
        if (cur == -ONE) {  
          temp[x] = ZERO;  
          return ONE;  
        } else if (cur == ZERO) {
          state = ZERO; 
        } 
        temp[x] = ZERO;  
        }
      }  
    return state;  
  }

/*
  Get the index of the position the computer places cheer.
  Given the array received by the client and its copy array.
  Returns the index of the position.
*/
  int computer(int num[], int temp[]) {
    int ansx = -ONE;
    int state;  
    for (int x = ZERO; x < NINE; x++) {
      if (num[x] == ZERO) {  
        ansx = x;    
        num[x] = TWO;  
        if (win(num) == ONE) {  
          printf("1: what computer typed is %d\n", x + ONE);
          return x;  
        }  
        num[x] = ZERO;  
      }  
      temp[x] = num[x];  
    }  
  
    for (int x = ZERO; x < NINE; x++) {
      if (num[x] == ZERO) {  
        num[x] = ONE;  
        if (win(num) == ONE) {  
          printf("2: what computer typed is %d\n", x + ONE);  
          return x;  
        }  
        num[x] = ZERO;  
      }  
    }  
  
    for (int x = ZERO; x < NINE; x++) { 
      if (temp[x] == ZERO) {   
        temp[x] = TWO;    
        if (win(temp) == ONE) {  
          printf("3: what computer typed is %d\n", x + ONE);   
          return x;  
        }  
        state = dfs(ZERO, temp);  
        if (state == ZERO) {  
          ansx = x;    
        } else if (state == -ONE) {  
          printf("4: what computer typed is %d\n", x + ONE);    
          return x;  
        }  
        temp[x] = ZERO;  
      }
    }  
    printf("5: what computer typed is %d\n", ansx + ONE); 
    return ansx; 
  }

/*
  Receive chessboard from the client.
  Given: struct of request, struct of response, 
         and the pointer of user data.
  Returns: a default value
*/
  int callback_process_order(const struct _u_request * request, 
    struct _u_response * response, void * user_data) {
    json_auto_t * json_order = ulfius_get_json_body_request(request, NULL);

    if(!json_order) {
      printf("json_order is null, exiting\n");
    }

    json_auto_t * board;
    board = json_object_get(json_order, BOARD);
    if (json_is_array(board)) {
      printf("board value is an array\n");
    } else {
      printf("board value is NOT an array\n");
    }

    int board_size = json_array_size(board);
    int temp[NINE];

    for (int i = ZERO; i < board_size; i++) {
      printf("  [%d] %llu\n", i, json_integer_value(json_array_get(board, i)));
      temp[i] = json_integer_value(json_array_get(board, i));
    }
  
    int index = computer(temp, temp);
    json_array_set(board, index, json_integer(TWO));
    ulfius_set_json_body_response(response, 200, board);
  
    return U_CALLBACK_CONTINUE;
  }

/**
 * Main function to start the framework.
 */
  int main(void) {
    struct _u_instance instance;
    // Initialize instance with the port number
    if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
      fprintf(stderr, "Error ulfius_init_instance, abort\n");
      return(1);
    }
    ulfius_add_endpoint_by_val(&instance, "POST", 
    "/order", NULL, 0, &callback_process_order, NULL);

    // Start the framework
    if (ulfius_start_framework(&instance) == U_OK) {
      printf("Start framework on port %d\n", instance.port);
      getchar();
    } else {
      fprintf(stderr, "Error starting framework\n");
    }
    printf("End framework\n");

    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);

    return 0;
  }

