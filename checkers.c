/* Program to print and play checker games.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  September 2021, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: [Liam Bong 1272915]
  Dated:     [14/10/2021]

*/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <string.h>

/* some #define's from my sample solution ------------------------------------*/
#define BOARD_SIZE          8       // board size
#define ROWS_WITH_PIECES    3       // number of initial rows with pieces
#define CELL_EMPTY          '.'     // empty cell character
#define CELL_BPIECE         'b'     // black piece character
#define CELL_WPIECE         'w'     // white piece character
#define CELL_BTOWER         'B'     // black tower character
#define CELL_WTOWER         'W'     // white tower character
#define COST_PIECE          1       // one piece cost
#define COST_TOWER          3       // one tower cost
#define TREE_DEPTH          3       // minimax tree depth
#define COMP_ACTIONS        10      // number of computed actions

#define LETTERS_ROW "     A   B   C   D   E   F   G   H" 
#define PLUS_MIN_ROW "   +---+---+---+---+---+---+---+---+"
#define EQUAL_SIGN_ROW "====================================="

#define ASCII_OF_A 65
#define ASCII_OF_a 97
#define MAKE_TOWER -(ASCII_OF_a - ASCII_OF_A)

#define MAX_POINTS COST_TOWER*12
#define MIN_POINTS -COST_TOWER*12

#define LEFT -1
#define RIGHT 1
#define UP -1
#define DOWN 1

#define WHITE_ENDLINE 7
#define BLACK_ENDLINE 0


#define NPLAYERS 2
#define LENGTH 6

#define TRUE 1
#define FALSE 0


/* one type definition from my sample solution -------------------------------*/
typedef unsigned char board_t[BOARD_SIZE][BOARD_SIZE];  // board type

typedef struct board_states board_states_t;

// a structure which can point to pointers to the original structure
struct board_states {
    board_t curr_state;
    board_states_t** more_states;
    int board_cost;
    int p_turn;
    int n_more_states;
    int tree_depth;
};

void print_board(board_t board);
int calc_board_cost(board_t board);
void assign_piece_status(unsigned char pt, int* is_tower, int* up, int* down);
int is_illegal_move(board_t board, unsigned char pt, 
                    int* sr, int* sc, int* tr, int* tc);
void check_move_violations(board_t board, unsigned char pt, 
                    int* sr, int* sc, int* tr, int* tc);
void make_move(board_t board, unsigned char pt, 
                    int* sr, int* sc, int* tr, int* tc);
void print_next_move(board_t board, unsigned char p_turn, int curr_action,
                    char s_letter, int s_row, char t_letter, int t_row);
unsigned char stage0(board_t board, int* do_stage_one, int* do_stage_two);


void add_extra_state(board_states_t* original, board_t curr_board, int* size, 
                    unsigned char pt, int* sr, int* sc, int* tr, int* tc);
void find_more_states(board_states_t* original, board_t curr_board, 
                    int row, int col, int p_turn, int* p_curr_size, int* iters);
void create_checkers_tree(board_states_t* node, int* iterations);
void search_deeper_level(board_states_t* current_tree, int depth);
board_states_t* 
find_min_max_move(board_states_t* original, board_states_t* tree, 
                    int* global_min, int* global_max, int pt);
void extremely_unelegant_free(board_states_t* original);
board_states_t* find_optimal_move(board_states_t* tree);
board_states_t* stage1(board_t board, int pt);

void stage2(board_t board, int pt);

/* NOTES: firstly, huge apolgies for whoever have to read this, secondly, 
   if at this point you're wondering where the issues was in my program, if it 
   helps, my bug testing has concluded there is an underlying error with the 
   creation of the checkers tree after multiple calls. Once reaching a depth of 
   2, things go south, and some more_states point to faulty board_states_t*s
   eg NULL or garbage values
*/

int 
main(int argc, char *argv[]) {
    // YOUR IMPLEMENTATION OF STAGES 0-2
    
    board_t my_board;
    unsigned char p_turn;
    int do_stage_one=FALSE, do_stage_two=FALSE;

    
    p_turn = stage0(my_board, &do_stage_one, &do_stage_two);
    // stage 1 and 2 are called based on its boolean pointers sent to stage 0
    
    /* ERROR: stage 1 and 2 interefere with the printing of stage 0 on dimefox,
       to see how it runs standalone, comment out the if and if else statement
    */
    if (do_stage_one) {
        stage1(my_board, p_turn);
    }
    else if (do_stage_two) {
        stage2(my_board, p_turn);
    }
    
    // algorithms are fun

    return EXIT_SUCCESS;            // exit program with the success code
}

/* helper function for stage 1 and 2 which
   calculates the board cost of the inputted board and returns accordingly
*/
int
calc_board_cost(board_t board) {
    int i, j, board_cost=0;

    for (i=0; i < BOARD_SIZE; i++) {
        for (j=0; j < BOARD_SIZE; j++) {
            board_cost += COST_PIECE*(board[i][j] == CELL_BPIECE);
            board_cost += COST_TOWER*(board[i][j] == CELL_BTOWER);
            board_cost -= COST_PIECE*(board[i][j] == CELL_WPIECE);
            board_cost -= COST_TOWER*(board[i][j] == CELL_WTOWER);
        }
    }

    return board_cost;
}

/* helper function for stage 1 and 2 which 
   takes a move and prints move if it gets the clear from check_move_violations
*/
void 
print_next_move(board_t board, unsigned char p_turn, int curr_action,
            char s_letter, int s_row, char t_letter, int t_row) {

    char colour[NPLAYERS][LENGTH] = {
        "WHITE", 
        "BLACK"
    };
 
    int s_col, t_col;

    s_row -= 1;
    t_row -= 1;
    // turns the coordinate letter into the index of its respective column     
    s_col = (int)(s_letter - ASCII_OF_A);
    t_col = (int)(t_letter - ASCII_OF_A);

    // checks if legal, otherwise it makes the move and prints accordingly
    check_move_violations(board, p_turn, &s_row, &s_col, &t_row, &t_col);   
    make_move(board, p_turn, &s_row, &s_col, &t_row, &t_col);

    // figures out the colour, based on the current action we are on
    printf("%s\n%s ", EQUAL_SIGN_ROW, colour[curr_action%2]);
    printf("ACTION #%d: ", curr_action);
    printf("%c%d-%c%d\n", s_letter, s_row + 1, t_letter, t_row + 1);
    printf("BOARD COST: %d\n", calc_board_cost(board));
    print_board(board);
}

/* helper function for stage 1 and 2 takes a move and adjusts the board
*/
void 
make_move(board_t board, unsigned char pt, int* sr, int* sc, int* tr, int* tc) {
    unsigned char temp;
    int rise, run;

    // at the end of the day first thing, source cell disappears
    temp = board[*sr][*sc];
    board[*sr][*sc] = CELL_EMPTY;

    rise = *tr - *sr;
    run = *tc - *sc;
    
    // for captures, checks if cell is empty to the immediate diagonal
    if (rise == 2*UP || rise == 2*DOWN) {
        board[*sr + rise/2][*sc + run/2] = CELL_EMPTY;
    }
    board[*tr][*tc] = temp;

    // for promotions, always need to check as you can capture then promote
    if (pt == CELL_BPIECE && *tr == BLACK_ENDLINE) {
        board[*tr][*tc] = CELL_BTOWER;
    }
    else if (pt == CELL_WPIECE && *tr == WHITE_ENDLINE) {
        board[*tr][*tc] = CELL_WTOWER;
    }

}

/* helper function for is_illegal_move, which assigns directions to up and down
   based on the colour of the piece and also checks whether it is a tower
*/
void 
assign_piece_status(unsigned char pt, int* is_tower, int* up, int* down) {
    if (pt == CELL_WPIECE || pt == CELL_WTOWER) {
        if (pt == CELL_WTOWER) {
            *is_tower = TRUE;
        }
        // flip vectors y vectors if white
        *up = -UP;
        *down = -DOWN;
    }
    else {
        if (pt == CELL_BTOWER) {
            *is_tower = TRUE;
        }
        *up = UP;
        *down = DOWN;
    }
}

/* helper function for stage 1 and 2 which takes a move and checks whether its
   legal based on a few assumptions such as the source cell is not empty etc..
*/
int 
is_illegal_move(board_t board, unsigned char pt, 
                int* sr, int* sc, int* tr, int* tc) {

    int is_tower=FALSE, empty_target=TRUE;
    int rise, run, up, down;  

    rise = *tr - *sr;
    run = *tc - *sc;

    // used for stage 2
    /* where 0 represents the left and top edge, and 7 represents the bottom 
       and right edge
    */
    empty_target = (board[*tr][*tc] == CELL_EMPTY);
    if (*tr < 0 || *tr > 7 || *tc < 0 ||  *tc > 7 || !empty_target) {
        return TRUE;
    }    

    // assign vectors and tower status
    assign_piece_status(pt, &is_tower, &up, &down);
    
    /* if its 2 right or left, then we have to check whether the immediate 
       diagonal in that direction, hold a piece of the opposite colour
    */
    if (run == 2*RIGHT || run == 2*LEFT) {
        if (rise == 2*up) {
            if (board[*sr + up][*sc + run/2] != pt
                && board[*sr + up][*sc + run/2] != pt + MAKE_TOWER
                && board[*sr + up][*sc + run/2] != CELL_EMPTY) {
                return FALSE;
            }
        }
        // only towers can move down, note the vectors were flipped accordingly
        else if (rise == 2*down && is_tower) {
            if (board[*sr + down][*sc + run/2] != pt
                && board[*sr + down][*sc + run/2] != pt + MAKE_TOWER
                && board[*sr + down][*sc + run/2] != CELL_EMPTY) {
                return FALSE;
            }
        }
    }

    // target cell has to be empty at this point
    else if (run == RIGHT || run == LEFT) {
        // simple check if up or down and if tower
        if (rise == up || (rise == down && is_tower)) {
            return FALSE;
        }
    }

    // if all fails, then its an illegal move
    return TRUE;
}

/* helper function for stage 1 which checks if a move has any violations, 
   prints the error message then exits
*/
void 
check_move_violations(board_t board, unsigned char pt, 
                int* sr, int* sc, int* tr, int* tc) {

    // does all checks but the last one
    /* where 0 represents the left and top edge, and 7 represents the bottom 
       and right edge
    */
    if (*sr < 0 || *sr > 7 || *sc < 0 ||  *sc > 7) {
        printf("ERROR: Source cell is outside of the board.\n");
    }
    else if (*tr < 0 || *tr > 7 || *tc < 0 ||  *tc > 7) {
        printf("ERROR: Target cell is outside of the board.\n");
    }
    else if (board[*sr][*sc] == CELL_EMPTY) {
        printf("ERROR: Source cell is empty.\n");
    }
    else if (board[*tr][*tc] != CELL_EMPTY) {
        printf("ERROR: Target cell is not empty.\n");
    }
    else if (board[*sr][*sc] != pt && board[*sr][*sc] != pt + MAKE_TOWER) {
        printf("ERROR: Source cell holds opponent's piece/tower.\n");
    }
    // checks if its an illegal move based on all the checks being passed
    else if (is_illegal_move(board, pt, sr, sc, tr, tc)){
        printf("ERROR: Illegal action.\n");
    }
    else {
        return;
    }
    exit(EXIT_FAILURE);
}

/* helper function for stage 1 and 2 which does as it sounds, prints the board
*/
void 
print_board(board_t board) {
    int i, j;

    printf("%s\n", LETTERS_ROW);
    for (i=0; i < 2*BOARD_SIZE + 1; i++) {
        // checks if row is even
        if (i % 2 == 0) {
            printf("%s\n", PLUS_MIN_ROW);
        }
        else {
            printf(" %d |", (int)(i/2.0 + 0.5));

            for (j=0; j < BOARD_SIZE; j++) {
                printf(" %c |", board[i/2][j]);
            }
            printf("\n");
        }
    }
}

/* stage zero, reading, analysing and printing the input data, and then returns 
   the colour of the player whose turn it is next
*/
unsigned char 
stage0(board_t board, int* do_stage_one, int* do_stage_two) {
    int i, j;
    unsigned char p_turn=CELL_BPIECE;
    char s_letter, t_letter;
    int s_row, t_row;

    //initalise the board with the starting values
    for (i=0; i < BOARD_SIZE; i++) {
        for (j=0; j < BOARD_SIZE; j++) {
            board[i][j] = CELL_EMPTY;

            if (i < ROWS_WITH_PIECES) {
                if (i % 2 != 0 && j % 2 == 0) {
                    board[i][j] = CELL_WPIECE;
                }
                else if (i % 2 == 0 && j % 2 != 0) {
                    board[i][j] = CELL_WPIECE;
                }
            }

            else if (i >= (BOARD_SIZE - ROWS_WITH_PIECES)) {
                if (i % 2 != 0 && j % 2 == 0) {
                    board[i][j] = CELL_BPIECE;
                }
                else if (i % 2 == 0 && j % 2 != 0) {
                    board[i][j] = CELL_BPIECE;
                }
            }
        }
    }

    // print the starting board
    printf("BOARD SIZE: 8x8\n#BLACK PIECES: 12\n#WHITE PIECES: 12\n");
    print_board(board);
    
    // read each action and send it to print next move
    i=0;
    while (scanf("%c%d-%c%d\n", &s_letter, &s_row, &t_letter, &t_row) == 4) {
        if (i % 2 == 0) {
            p_turn = CELL_BPIECE;
        }
        else {
            p_turn = CELL_WPIECE;
        }
        print_next_move(board, p_turn, i + 1, s_letter, s_row, t_letter, t_row);
        i++;
    }

    // if 'A' or 'P' is scanned, then we can set its pointer to call the stage
    if (s_letter == 'A') {
        *do_stage_one = TRUE;
    }
    else if (s_letter == 'P') {
        *do_stage_two = TRUE;
    }

    return p_turn;
}

/* helper function to find more states, which adds child boards to the branch
   of the original board
*/
void
add_extra_state(board_states_t* original, board_t curr_board, int* curr_size,
                unsigned char pt, int* sr, int* sc, int* tr, int* tc) {

    board_states_t *new_branch;
    board_t new_board;

    // sending the new branch to the heap
    new_branch = (board_states_t*)malloc(sizeof(*new_branch));
    assert(new_branch!=NULL);

    // make a copy of the original board and make the move
    memcpy(new_board,curr_board,sizeof(board_t));
    make_move(new_board, pt, sr, sc, tr, tc);
    
    // then assign the new adjusted board and all the other struct data
    memcpy(new_branch->curr_state,new_board,sizeof(board_t));
    new_branch->board_cost = calc_board_cost(new_branch->curr_state);
    new_branch->more_states = NULL;
    new_branch->n_more_states = 0;
    new_branch->tree_depth = original->tree_depth + 1;
    if (pt == 'b') {
        new_branch->p_turn = 'w';
    }
    else {
        new_branch->p_turn = 'b';
    }

    // multiplicatively realloc more memory to more_states if necessary
    if (original->n_more_states == *curr_size) {
        *curr_size *= 2;
        original->more_states = (board_states_t**)realloc(original->more_states, 
                    (*curr_size)*sizeof(*original->more_states));
        assert(original->more_states != NULL);
    }
    
    // assign the newest branch to the array of pointers to board_states_ts
    original->more_states[original->n_more_states++] = new_branch;

}

/* helper function for create checkers tree, which identifies the children
   boards of the original node and sends them to be added to the branch
*/
void 
find_more_states(board_states_t* original, board_t curr_board,
                int row, int col, int p_turn, int* p_curr_size, int* iters) {

    int i, j, sr, sc, tr, tc;
    int one_unit[] = {1, -1};
    int two_unit[] = {2, -2};
    board_states_t** board_children;

    /* initialies the pointer to the pointer of board_states_t and gives it 
       and gives it enough memory to point to one struct
    */
    if (original->more_states == NULL) {
        // sends to heap
        board_children = (board_states_t**)malloc(sizeof(*board_children));
        assert(board_children!=NULL);

        original->more_states = board_children;
    }
    
    
    
    // essentially checks all diagonals of both one and two units
    for (i=0; i < 2; i++) {
        for (j=0; j < 2; j++) {
            sr = row;
            sc = col;
            tr = row + one_unit[i];
            tc = col + one_unit[j];

            // if the move is legal, we add a child struct to the original node
            if (!is_illegal_move(curr_board, p_turn, &sr, &sc, &tr, &tc)) {
                add_extra_state(original, curr_board, p_curr_size, p_turn, &sr,
                                &sc, &tr, &tc);
                
            }
            else {
                tr = row + two_unit[i];
                tc = col + two_unit[j];
                if (!is_illegal_move(curr_board, p_turn, &sr, &sc, &tr, &tc)) {
                    add_extra_state(original, curr_board, p_curr_size, p_turn, 
                                &sr, &sc, &tr, &tc);
                }
                
            }
        }
    }
}

/* helper function for stage 1 which takes a pointer to a board_states_t and 
   creates a tree of depth TREE_DEPTH with the pointer as its foundation
*/
void 
create_checkers_tree(board_states_t* node, int* iterations) {
    int i, j, curr_cell, owns_cell, curr_size=1;
    unsigned char pt=node->p_turn;

    for (i=0; i < BOARD_SIZE; i++) {
        for (j=0; j < BOARD_SIZE; j++) {
            curr_cell = node->curr_state[i][j];
            owns_cell = (curr_cell == pt || curr_cell == pt + MAKE_TOWER);
            // no need to consider any of the other cells
            if (owns_cell) {
                find_more_states(node, node->curr_state, i, j, pt, 
                                &curr_size, iterations);
            }
        }
    }

    *iterations += 1;

    if (*iterations < TREE_DEPTH) {
        for (i=0; (i < node->n_more_states) ; i++) { 
            create_checkers_tree(node->more_states[i], iterations);
        }
    }
    
    
}

/* helper function for stage 1 which intends to place the more_states[0] of each
   node to board with the lowest or highest cost depending on what depth it is
   and whose turn
*/
void 
search_deeper_level(board_states_t* current_tree, int depth) {
    int i, j, min=MAX_POINTS, max=MIN_POINTS;

    // recursively calls until we hit a depth of 2
    if (current_tree->tree_depth < depth - 1) {
        for (i=0; i < current_tree->n_more_states; i++) {
            search_deeper_level(current_tree->more_states[i], depth);
        }
    }
    else if (current_tree != NULL) {
        if (current_tree->p_turn == CELL_WPIECE) {
            /* loops through the board costs of each child board, of the node 
               boards at depth 2
            */
            for (j=0; j < current_tree->n_more_states; j++) {
                if (current_tree->more_states[j]->board_cost < min) {
                    // assign the leftmost pointer to the new pointer
                    current_tree->more_states[0] = current_tree->more_states[j];

                    min = current_tree->more_states[j]->board_cost;
                }
            }
        }
        else {
            // same for if we need to find the maximum
            for (j=0; j < current_tree->n_more_states; j++) {
                if (current_tree->more_states[j]->board_cost > max) {
                    current_tree->more_states[0] = current_tree->more_states[j];

                    max = current_tree->more_states[j]->board_cost;
                }
            }
        }
    }
    
}

/* helper function for stage 1 which finds eventual the minimum or maxiumum 
   board cost, based on which players turn it is at the start of the stage 1 
   function call
*/
board_states_t*
find_min_max_move(board_states_t* original, board_states_t* tree, 
                int* global_min, int* global_max, int pt) {
    
    if (tree != NULL) {
        return NULL;
    }
    // recursively calls until we are at depth 3
    if (tree != NULL && tree->tree_depth != TREE_DEPTH) {
        if (tree->more_states[0] != NULL) {
            find_min_max_move(original, tree->more_states[0], global_min, 
                        global_max, pt);
        }
        
    }
    /* once we hit depth 3, we check if it is lower than or higher than the 
       the previous max or min (all based on the initial player turn)
    */
    else if (tree != NULL) {
        if (pt == CELL_WPIECE && tree->board_cost < *global_min) {
            *global_min = tree->board_cost;
            return original;

        }
        else if (pt == CELL_BPIECE && tree->board_cost > *global_max) {
            *global_max = tree->board_cost;
            return original;
        }
        
    }
    // returns NULL if it is not lower or higher than the min or max
    return NULL;
}

/* quite possiblly the most poorly written function of my extremely short 
   coding career, time is ticking and the pressure is on, efficient or not,
   the memory must still be freed, so here goes
*/
void
extremely_unelegant_free(board_states_t* original) {
    int i, j, k;
    board_states_t* level_two;

    // NOTE: doesnt work for TREE_DEPTHS other than 3, very versatile!

    // frees all the boards and pointers to pointers board_states_ts for depth 3
    for (i=0; i < original->n_more_states; i++) {
        for (j=0; j < original->more_states[i]->n_more_states; j++) {
            level_two = original->more_states[i]->more_states[j];
            for (k=0; k < level_two->n_more_states; k++) {
                free(level_two->more_states[k]->curr_state);
                free(level_two->more_states[k]);
                level_two->more_states[k] = NULL;

            }
        }
    }

    // now depth 2
    for (i=0; i < original->n_more_states; i++) {
        for (j=0; j < original->more_states[i]->n_more_states; j++) {
            free(original->more_states[i]->more_states[j]->curr_state);
            free(original->more_states[i]->more_states[j]);
            original->more_states[i]->more_states[j] = NULL;
        }
    }

    // now depth 1
    for (i=0; i < original->n_more_states; i++) {
        free(original->more_states[i]->curr_state);
        free(original->more_states[i]);
        original->more_states[i] = NULL;
    }

    // now the head
    free(original->curr_state);
    free(original);
    original = NULL;
}

/* helper function for stage 1, finds the optimal move and prints accordingly
   and then returns a pointer to a board_states_t or a NULL byte if no action 
   was found
*/
board_states_t* 
find_optimal_move(board_states_t* tree) {
    unsigned char pt=tree->p_turn;
    int global_min=MAX_POINTS, global_max=MIN_POINTS, i, depth=TREE_DEPTH;
    board_states_t* output=NULL;
    
    // modifies tree such that all min or max boards are on the left of its node
    while (depth > 2) {
        search_deeper_level(tree, depth);
        depth -= 1;
    }
    
    /* for each level 1 node, find the board cost if following its most 
       leftward path (the path with the min or max boards)
    */
    for (i=0; i < tree->n_more_states; i++) {
        if (find_min_max_move(tree, tree->more_states[i], &global_min,
                     &global_max, pt) != NULL) {
            output = find_min_max_move(tree, tree->more_states[i], &global_min,
                     &global_max, pt);
        }
    }
    // only print the board if an action can be computed
    if (output != NULL) {
        print_board(output->curr_state);
    }
    else {
        if (tree->p_turn == CELL_WPIECE) {
            printf("BLACK WIN!");
        }
        else {
            printf("WHITE WIN!");
        }
    }

    // almost done, freeing the memory from the whole tree
    extremely_unelegant_free(tree);
    return output;

}


/* stage one, computing and printing the best move to be played
*/
board_states_t* 
stage1(board_t board, int pt) {
    board_states_t *my_tree;
    int iterations=0;

    // assign my tree to the heap
    my_tree = (board_states_t*)malloc(sizeof(*my_tree));
    assert(my_tree!=NULL);

    // assign my tree the basic strcuture variable data
    memcpy(my_tree->curr_state,board,sizeof(board_t));
    my_tree->board_cost = calc_board_cost(board);
    my_tree->p_turn = pt;
    my_tree->n_more_states = 0;
    my_tree->tree_depth = 0;
    my_tree->more_states = NULL;

    
    create_checkers_tree(my_tree, &iterations);
    return find_optimal_move(my_tree);

}

/* stage two, computing the next ten actions and/or the winner
*/
void
stage2(board_t board, int pt) {
    int i;
    board_states_t* pointer;
    
    // we can use the fact that stage1 returns a pointer to a board_states_t
    pointer = stage1(board, pt);
    for (i=0; i < COMP_ACTIONS; i++) {
        // if it cannot compute the next action, we terminate
        if (pointer != NULL) {
            // otherwise, we access the returned board_t and call stage1 again
            pointer = stage1(pointer->curr_state, pt);
        }
        else {
            return;
        }
    }
    
}

/* THE END -------------------------------------------------------------------*/