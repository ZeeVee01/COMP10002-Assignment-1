/* Program to do "calculations" on numeric CSV data files.

   Skeleton program written by Alistair Moffat, ammoffat@unimelb.edu.au,
   September 2020, with the intention that it be modified by students
   to add functionality, as required by the assignment specification.

   Student Authorship Declaration:

   (1) I certify that except for the code provided in the initial skeleton
   file, the  program contained in this submission is completely my own
   individual work, except where explicitly noted by further comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students, or by
   non-students as a result of request, solicitation, or payment, may not be
   submitted for assessment in this subject.  I understand that submitting for
   assessment work developed by or in collaboration with other students or
   non-students constitutes Academic Misconduct, and may be penalized by mark
   deductions, or by other penalties determined via the University of
   Melbourne Academic Honesty Policy, as described at
   https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will I
   do so until after the marks are released. I understand that providing my
   work to other students, regardless of my intention or any undertakings made
   to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring service,
   or drawing the attention of others to such services and code that may have
   been made available via such a service, may be regarded as Student General
   Misconduct (interfering with the teaching activities of the University
   and/or inciting others to commit Academic Misconduct).  I understand that
   an allegation of Student General Misconduct may arise regardless of whether
   or not I personally make use of such solutions or sought benefit from such
   actions.

   Signed by: Paul Hutchins - 1160468
   Dated:     16/09/2020

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>

/* these #defines provided as part of the initial skeleton */

#define MAXCOLS	20	/* maximum number of columns to be handled */
#define MAXROWS	999	/* maximum number of rows to be handled */
#define LABLEN  20	/* maximum length of each column header */
#define LINELEN 100	/* maximum length of command lines */

#define ERROR	(-1)	/* error return value from some functions */

#define O_NOC	'-'	/* the "do nothing" command */
#define O_IND	'i'	/* the "index" command */
#define O_ANA 	'a'	/* the "analyze" command */
#define O_DPY 	'd'	/* the "display" command */
#define O_PLT 	'p'	/* the "plot" command */
#define O_SRT 	's'	/* the "sort" command */

#define CH_COMMA ','	/* comma character */
#define CH_CR    '\r'	/* pesky CR character in DOS-format files */
#define CH_NL    '\n'	/* newline character */

/* if you wish to add further #defines, put them below this comment */

#define BARS   10 /* number of bars in the graph */
#define BARLENGTH 60 /* max bar length */
#define MINRANGE 1.0E-6 /* Minium dif for a data point to be equal */


/* and then, here are some types for you to work with */
typedef char head_t[LABLEN+1];

typedef double csv_t[MAXROWS][MAXCOLS];

/****************************************************************/

/* function prototypes */

void get_csv_data(csv_t D, head_t H[],  int *dr,  int *dc, int argc,
	char *argv[]);
void error_and_exit(char *msg);
void print_prompt(void);
int  get_command(int dc, int *command, int ccols[], int *nccols);
void handle_command(int command, int ccols[], int nccols,
			csv_t D, head_t H[], int dr, int dc);
void do_index(csv_t D, head_t H[], int dr, int dc, int ccols[], int nccols);

/* add further function prototypes below here */
void insertion_sort(csv_t D, int col, int dr, int dc);
void swap_lines(csv_t D, int i, int j, int dc);
void do_display(csv_t D, head_t H[], int dr, int dc, int ccols[], int nccols);
void do_analyze(csv_t D, head_t H[], int dr, int dc, int ccols[], int nccols);
void do_sorted(csv_t D, head_t H[], int dr, int dc, int ccols[], int nccols);
void do_plot(csv_t D, head_t H[], int dr, int dc, int ccols[], int nccols);


/****************************************************************/

/* main program controls all the action
*/
int
main(int argc, char *argv[]) {

	head_t H[MAXCOLS];	/* labels from the first row in csv file */
	csv_t D;		/* the csv data stored in a 2d matrix */
	int dr=0, dc=0;		/* number of rows and columns in csv file */
	int ccols[MAXCOLS];
	int nccols;
	int command;

	/* this next is a bit of magic code that you can ignore for
	   now, it reads csv data from a file named on the
	   commandline and saves it to D, H, dr, and dc
	   */
	get_csv_data(D, H, &dr, &dc, argc, argv);
	
	/* ok, all the input data has been read, safe now to start
	   processing commands against it */

	print_prompt();
	while (get_command(dc, &command, ccols, &nccols) != EOF) {
		handle_command(command, ccols, nccols,
			D, H, dr, dc);
		print_prompt();
	}

	/* all done, so pack up bat and ball and head home */
	printf("\nTa daa!!!\n");
	return 0;
}

/****************************************************************/

/* prints the prompt indicating ready for input
*/
void
print_prompt(void) {
	printf("> ");
}

/****************************************************************/

/* read a line of input into the array passed as argument
   returns false if there is no input available
   all whitespace characters are removed
   all arguments are checked for validity
   if no argumnets, the numbers 0..dc-1 are put into the array
*/
int
get_command(int dc, int *command, int columns[], int *nccols) {
	int i=0, c, col=0;
	char line[LINELEN];
	/* comand is in first character position */
	if ((*command=getchar()) == EOF) {
		return EOF;
	}
	/* and now collect the rest of the line, integer by integer,
	   sometimes in C you just have to do things the hard way */
	while (((c=getchar())!=EOF) && (c!='\n')) {
		if (isdigit(c)) {
			/* digit contributes to a number */
			line[i++] = c;
		} else if (i!=0)  {
			/* reached end of a number */
			line[i] = '\0';
			columns[col++] = atoi(line);
			/* reset, to collect next number */
			i = 0;
		} else {
			/* just discard it */
		}
	}
	if (i>0) {
		/* reached end of the final number in input line */
		line[i] = '\0';
		columns[col++] = atoi(line);
	}

	if (col==0) {
		/* no column numbers were provided, so generate them */
		for (i=0; i<dc; i++) {
			columns[i] = i;
		}
		*nccols = dc;
		return !EOF;
	}

	/* otherwise, check the one sthat were typed against dc,
	   the number of cols in the CSV data that was read */
	for (i=0; i<col; i++) {
		if (columns[i]<0 || columns[i]>=dc) {
			printf("%d is not between 0 and %d\n",
				columns[i], dc);
			/* and change to "do nothing" command */
			*command = O_NOC;
		}
	}
	/* all good */
	*nccols = col;
	return !EOF;
}

/****************************************************************/

/* this next is a bit of magic code that you can ignore for now
   and that will be covered later in the semester; it reads the
   input csv data from a file named on the commandline and saves
   it into an array of character strings (first line), and into a
   matrix of doubles (all other lines), using the types defined
   at the top of the program.  If you really do want to understand
   what is happening, you need to look at:
   -- The end of Chapter 7 for use of argc and argv
   -- Chapter 11 for file operations fopen(), and etc
*/
void
get_csv_data(csv_t D, head_t H[],  int *dr,  int *dc, int argc,
		char *argv[]) {
	FILE *fp;
	int rows=0, cols=0, c, len;
	double num;

	if (argc<2) {
		/* no filename specified */
		error_and_exit("no CSV file named on commandline");
	}
	if (argc>2) {
		/* confusion on command line */
		error_and_exit("too many arguments supplied");
	}
	if ((fp=fopen(argv[1], "r")) == NULL) {
		error_and_exit("cannot open CSV file");
	}

	/* ok, file exists and can be read, next up, first input
	   line will be all the headings, need to read them as
	   characters and build up the corresponding strings */
	len = 0;
	while ((c=fgetc(fp))!=EOF && (c!=CH_CR) && (c!=CH_NL)) {
		/* process one input character at a time */
		if (c==CH_COMMA) {
			/* previous heading is ended, close it off */
			H[cols][len] = '\0';
			/* and start a new heading */
			cols += 1;
			len = 0;
		} else {
			/* store the character */
			if (len==LABLEN) {
				error_and_exit("a csv heading is too long");
			}
			H[cols][len] = c;
			len++;
		}
	}
	/* and don't forget to close off the last string */
	H[cols][len] = '\0';
	*dc = cols+1;

	/* now to read all of the numbers in, assumption is that the input
	   data is properly formatted and error-free, and that every row
	   of data has a numeric value provided for every column */
	rows = cols = 0;
	while (fscanf(fp, "%lf", &num) == 1) {
		/* read a number, put it into the matrix */
		if (cols==*dc) {
			/* but first need to start a new row */
			cols = 0;
			rows += 1;
		}
		/* now ok to do the actual assignment... */
		D[rows][cols] = num;
		cols++;
		/* and consume the comma (or newline) that comes straight
		   after the number that was just read */
		fgetc(fp);
	}
	/* should be at last column of a row */
	if (cols != *dc) {
		error_and_exit("missing values in input");
	}
	/* and that's it, just a bit of tidying up required now  */
	*dr = rows+1;
	fclose(fp);
	printf("    csv data loaded from %s", argv[1]);
	printf(" (%d rows by %d cols)\n", *dr, *dc);
	return;
}
 
/****************************************************************/

void
error_and_exit(char *msg) {
	printf("Error: %s\n", msg);
	exit(EXIT_FAILURE);
}

/****************************************************************/

/* the 'i' index command */
void
do_index(csv_t D, head_t H[], int dr, int dc,
		int ccols[], int nccols) {
	int i, c;
	printf("\n");
	for (i=0; i<nccols; i++) {
		c = ccols[i];
		printf("    column %2d: %s\n", c, H[c]);
	}
}


/*****************************************************************
******************************************************************

Below here is where you do most of your work, and it shouldn't be
necessary for you to make any major changes above this point (except
for function prototypes, and perhaps some new #defines).

Below this point you need to write new functions that provide the
required functionality, and modify function handle_command() as you
write (and test!) each one.

Tackle the stages one by one and you'll get there.

Have Fun!!!

******************************************************************
*****************************************************************/

/* My helper functions */

/* insertion sort */

void
insertion_sort(csv_t D, int col, int dr, int dc){
	
	double cur_comp, swap_comp; /* containers for current comparison */
	int i, j; /* counter variables */
	int swap1, swap2; /* stores what lines need to be swapped */

	/* insertion swap code - provides lines to be swapped to swap_lines */
	for (i=1; i<dr; i++){
		cur_comp = D[i][col];

		for (j=i-1; j>= 0; j--){
			swap_comp = D[j][col];

			if (cur_comp < swap_comp){
				/* if the term prior to cur_comp swap term is lesser, swaps */
				swap1 = j + 1;
				swap2 = j;
				swap_lines(D, swap1, swap2, dc);
			}
			else{
				break;
			}
		}
	}
}

/* Swap lines for insertion sort */

void
swap_lines(csv_t D, int swap1, int swap2, int dc){
	double temp_line_data; /* temporary data container */
	int i;

    /* loops through values in swap rows and swaps them */
	for (i=0; i<dc; i++){
		temp_line_data = D[swap1][i];
		D[swap1][i] = D[swap2][i];
		D[swap2][i] = temp_line_data;
	}

}

/**************************************************************/

/* My Commands stored below this line */

/* The 'a' index command */
void
do_analyze(csv_t D, head_t H[], int dr, int dc,
		int ccols[], int nccols){

	printf("\n");

    int i, j; /* counter variables */
	int c; /* storage variables */

    /* Loops through given inputs after index command */
    for (i=0; i<nccols; i++) {

		c = ccols[i]; /* c = current column we are using in csv */
        int sorted = 1; /* initial allocation of sorted being true */

        /* initiates output variables to 1st value in column c*/
        double avg = D[0][c], max = D[0][c], min = D[0][c], cur;

        /* itterates through column and obtains values - max, min, avg,sorted */
        for (j=1; j<dr; j++){
            cur = D[j][c];
            if (cur < D[j-1][c]){
                sorted = 0;
            }
            if (cur > max){
                max = cur;
            }
            if (cur < min){
                min = cur;
            }
            avg = avg + cur;
        }
        avg = avg / dr;

        /* prints column name, and sorted if sorted */
        printf("%17s",H[c]);
        if (sorted == 1){
            printf(" (sorted)\n");
        }
        else{
            printf("\n");
        }

        /* prints output values */
        printf("    max = %7.1f\n    min = %7.1f\n    avg = %7.1f\n",
         max, min, avg);
        if (sorted == 1){
			/* if values are sorted, finds and prints median value */
            double med;
			med = D[dr/2][c];

			if ((dr % 2) == 0){
				/* if length of sorted values is even, averages middle values */
				med = (med + D[(dr/2) - 1][c]) / 2;
			}
            printf("    med = %7.1f\n", med);
        }
		if (i < nccols - 1){
			/* if not last input, print new line */
			printf("\n");
		}
	}
}

/****************************************************************/

/* the 'd' index command */
void
do_display(csv_t D, head_t H[], int dr, int dc, int ccols[], int nccols){
	
	printf("\n");
    
    int i, j, instances = 1, common_row = 0; /* initializes counters*/
	int c; /* intializes storage variables */

    /* prints data labels*/
    for (i=nccols - 1; i>=0; i--){
        c = ccols[i]; 
        for (j=i; j>0; j--){
            printf("        ");
        }
        printf("%8s\n", H[c]);
    }

	/* works through data set, prints values and observe for duplicates */
	for (i=0; i<dr; i++){
		common_row = 1;
		for (j=0; j<nccols; j++){
			c = ccols[j];
			
			/* if non duplicate consecutive row print*/
			if (instances == 1){
				printf(" %7.1f", D[i][c]);
			}

			/* if any entry in current row and next row != row is not common */
			if (D[i][c] != D[i+1][c]){
				common_row = 0;
			}
		}

		/* if row is common, instances + 1 */
		if (common_row == 1){
			instances += 1;
		}

		/* else row is not common, and print number of instances of row*/
		else{
			if (instances == 1){
				printf("    (%2d instance)\n", instances);
			}
			else{
				printf("    (%2d instances)\n", instances);
			}

			/* reset instances counter */
			instances = 1;
		}

	}
}

/****************************************************************/

/* the 's' index command */

void
do_sorted(csv_t D, head_t H[], int dr, int dc, int ccols[], int nccols){

	int i; /*counter variable */
	int col; /* storage variable, for current comparison col */

	printf("\n");

	/* loops through columns to sort backwards, inputs into insertion sort */
	for (i=nccols-1; i>=0; i--){
		col = ccols[i];
		insertion_sort(D, col, dr, dc);
	}

	/* prints sorted by statement */
	printf("    sorted by:");
	for (i=0; i<nccols; i++){
		printf(" %s", H[ccols[i]]);
		if ((nccols - 1) != i){
			printf(",");
		}
	}
	printf("\n");
}

/***************************************************************/

/* the 'p' index command */

void
do_plot(csv_t D, head_t H[], int dr, int dc, int ccols[], int nccols){

	int i, j, k; /* counter variables */
	double cur; /* temporary storage variables */
	double max, min, step, step_min, step_max, step_count; /* storage doubles */
	int col, scale = 1, max_data = 0, print_amount = 0; /*storage variables */

	printf("\n");

	/*intitalizes max and min value to first data point */
	max = min = D[0][ccols[0]];

	/* gets max and min values for graph */
	for (i=0; i<dr; i++){
		for (j=0; j<nccols; j++){
			col = ccols[j];
			cur = D[i][col];
			if (cur > max){
				max = cur;
			}
			if (cur < min){
				min = cur;
			}
		}
	}

	if ((max - min) <= MINRANGE){
		/* if all data values within min range, doesnt plot, and prints */
		printf("all selected elements are %.1f\n", min);
		return;
	}
	/* numerical width of bars on graph and adjusts for range error */
	min = min - MINRANGE;
	max = max + MINRANGE;
	step = (max - min) / BARS;

	/* iterates through data to find largest bar and stores in max_data */
	for (k=0; k<BARS; k++){
		/* iterates through each bar, and obtains min and max data values */
		step_min = min + (k * step);
		step_max = min + ((k+1) * step);

		for (j=0; j<nccols; j++){
			step_count = 0; /* stores how much data is in each bar */

			for (i=0; i<dr; i++){
				/* iterates through all data points */
				col = ccols[j];
				cur = D[i][col];

				if ((step_min < cur) && (step_max > cur)){
					/* if data point falls in current step, increments */
					step_count++;
				}
			}
			if (step_count > max_data){
				/* if step_count > max_data, step_count = max_data
				/* max_data stores max amount of data in a single bar */
				max_data = step_count;
			}
		}
	}
	

	/* Adjusts scale if max_data > bar length */
	if (max_data > BARLENGTH){
		scale = max_data / BARLENGTH + ((max_data % BARLENGTH) != 0);
	}

	/* Graphical creation of graph */
	for (k=0; k<BARS; k++){
		/* iterates through each bar and obtains max and min value for bar */
		step_min = min + (k * step);
		step_max = min + ((k+1) * step);
		printf("%11.1f +\n", step_min); /* prints data label */

		for (j=0; j<nccols; j++){
			/* iterates through cols in nccols */
			print_amount = 0;
			col = ccols[j];

			for (i=0; i<dr; i++){
				/* iterates through data in current col and increments print
				amount if within current step values */
				cur = D[i][col];

				if ((step_min < cur) && (step_max > cur)){
					/* if inside current step value, increment print_amount */
					print_amount++;
				}
				if (((k==0)&&step_min==cur) || ((k==(BARS-1))&&step_max==cur)){
					/* adjustment for upper and lower bounds of data */
					print_amount++;
				}
			}
			printf("%11d |", col); /* prints col values */
			print_amount = (print_amount / scale) + (print_amount % scale != 0);
			/* uses scale to calculate how many data points to print */

			for (i=0; i<print_amount; i++){
				/* loops through print amount and prints bars */
				printf("]");
			} 
			printf("\n");
		}
	}

	printf("%11.1f +\n", max); /* prints final bar value */
	printf("    scale = %d\n", scale); /* prints scale */
}

/****************************************************************/

/* this function examines each incoming command and decides what
   to do with it, kind of traffic control, deciding what gets
   called for each command, and which of the arguments it gets
*/
void
handle_command(int command, int ccols[], int nccols,
			csv_t D, head_t H[], int dr, int dc) { 
	if (command==O_NOC) {
		/* the null command, just do nothing */
	} 
    else if (command==O_IND){
		do_index(D, H, dr, dc, ccols, nccols);
		/* the index command */
    }
    else if (command==O_ANA){
        do_analyze(D, H, dr, dc, ccols, nccols);
		/* the analyze command */
    }
    else if (command==O_DPY){
        do_display(D, H, dr, dc, ccols, nccols);
		/* the display command */
    }
	else if (command==O_SRT){
		do_sorted(D, H, dr, dc, ccols, nccols);
		/* the sort command */
	}
	else if (command==O_PLT){
		do_plot(D, H, dr, dc, ccols, nccols);
		/* the plot command */
	}
	else {
		printf("command '%c' is not recognized"
			" or not implemented yet\n", command);
	}
	return;
}

/*****************************************************************************/
/**************************** ALGORITHMS ARE FUN! ****************************/
/*****************************************************************************/
