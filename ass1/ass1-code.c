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

   Signed by: [Brian Soe Khant Aung 1166368]
   Dated:     [21/09/2020]

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
#define TRUE 1
#define FALSE 0
#define EMPTY -1.0
#define BANDS 10
#define MAX_ELEM 60

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

void do_analyze(csv_t D, head_t H[], int dr, int dc, int ccols[], int nccols);
void print_stats(csv_t D, int dr, int c);
int is_sorted(csv_t D, int dr, int c);

void do_display(csv_t D, head_t H[], int dr, int dc, int ccols[], int nccols);
void display_rows(csv_t D, int nccols, int ccols[], int dr);
int is_samearray(double array1[], double array2[], int n);
void print_values(double array[], int n);
void print_instances(int i);

void do_sort(csv_t D, head_t H[],int dr, int dc, int ccols[], int nccols);
void row_swap(csv_t D, int r1, int r2, int dc);
void sorted_by(head_t H[], int ccols[], int nccols);
void next_column(csv_t D, int ccols[], int *pc, int j, int nccols, int ccolsnum);

void do_plot(csv_t D, head_t H[], int dr, int dc, int ccols[], int nccols);
void find_minmax(csv_t D, int dr, int ccols[], int nccols, 
				double *pMin, double *pMax);
void print_histogram(csv_t D, int dr, int ccols[], int nccols, 
					double min, double max, double interval);
int count_elemnum(csv_t D, int dr, int ccols[], int nccols, int c, 
				double lower_lim, double upper_lim);
int find_scale(int nccols, int A[][nccols]);
void plot_graph(int nccols, int A[][nccols], int ccols[], 
				double interval, double min, double max, int scale);
void print_bars(int bars);
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
		// printf("yayy");
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

/* the 'i' index command
*/
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

/* this function examines each incoming command and decides what
   to do with it, kind of traffic control, deciding what gets
   called for each command, and which of the arguments it gets
*/
void
handle_command(int command, int ccols[], int nccols,
			csv_t D, head_t H[], int dr, int dc) { 
	if (command==O_NOC) {
		/* the null command, just do nothing */
	} else if (command==O_IND) {
		do_index(D, H, dr, dc, ccols, nccols);
	} else if (command==O_ANA) {
		do_analyze(D, H, dr, dc, ccols, nccols);
	} else if (command==O_DPY) {
		do_display(D, H, dr, dc, ccols, nccols);
	} else if (command==O_SRT) {
		do_sort(D, H, dr, dc, ccols, nccols);
	} else if (command==O_PLT) {
		do_plot(D, H, dr, dc, ccols, nccols);
	} else {
		printf("command '%c' is not recognized"
			" or not implemented yet\n", command);
	}
	return;
}

/****************************************************************/

/* Stage 1 - Averaging and Displaying */
/* the 'a' analyze command 
*/
void
do_analyze(csv_t D, head_t H[], int dr, int dc, 
        int ccols[], int nccols) {
    int i, c;

    for (i=0; i<nccols; i++) {
        c = ccols[i];
		printf("\n");
		/* print the header */
		printf("%17s", H[c]);
		/* check if the row is sorted */
		if (is_sorted(D, dr, c)) {
			printf(" (sorted)");
		}
		printf("\n");

		/* print out the max, min, avg values.
		   if the array is sorted, print out the med value too. */
		print_stats(D, dr, c);

    }
}

/* calculate and print the max, min, avg and med values. 
*/
void
print_stats(csv_t D, int dr, int c) {
    int i;
    double max, min, med, avg, sum=0;

    max = min = D[0][c];
    for (i=0; i<dr; i++) {
        /* find the largest value in the specified column */
        if (D[i][c] > max) {
            max = D[i][c];
        }
        /* find the smallest value in the specified column */
        if (D[i][c] < min) {
            min = D[i][c];
        }
        /* find the sum of the elements in array */
        sum += D[i][c];
    }

    /* find average */
    avg = sum/dr;

    /* print the values for max, min and avg */
	printf("    max = %7.1f\n    min = %7.1f\n    avg = %7.1f\n",
            max, min, avg);
    
    /* find medium if the array is sorted */
    if (is_sorted(D, dr, c)) {
        /* num of elements are even */
        if (dr%2 == 0) {
            med = (D[(dr-1)/2][c] + D[dr/2][c])/2.0;
        }
        /* num of elements are odd */
        else {
            med = D[dr/2][c];
        }
        printf("    med = %7.1f\n", med);
    }
}

/* check if the specified column is sorted
   return TRUE or FALSE 
*/
int
is_sorted(csv_t D, int dr, int c) {
    int i;

    for (i=0; i<dr-1; i++) {
        /* if the value in current row is larger than next value,
		   the column is not sorted. */
        if (D[i][c] > D[i+1][c]) {
            return FALSE;
        }
    }
    /* finished checking all the values in the column
	   the column is sorted */
    return TRUE;
}

/* the 'd' display command 
*/
void
do_display(csv_t D, head_t H[], int dr, int dc, 
        int ccols[], int nccols) {
    int i, c;
	printf("\n");

	/* print headers */
    for (i=nccols-1; i>=0; i--){
        c = ccols[i];
        printf("%*s", i*8, "");
		printf("%8s\n", H[c]);
    }
	/* print the rows and the no of times each row appears */
	display_rows(D, nccols, ccols, dr);
}

/* display rows and print out values from specific column
   indicate how many consecutive rows have those value */
void 
display_rows(csv_t D, int nccols, int ccols[], int dr) {
    int i, j, c, instance=0;
	double prev_array[nccols], curr_array[nccols];
    
	/* initialise the arrays */
	memset(prev_array, 0, sizeof prev_array);
	memset(curr_array, 0, sizeof curr_array);

    /* check each row from all the specified columns
	   and count how many consecutive rows have same value*/
    for (i=0; i<dr; i++) {
        for (j=0; j<nccols; j++) {
            c = ccols[j];
			curr_array[j] = D[i][c];
			if (i!=0) {
				prev_array[j] = D[i-1][c];
			}
		}        
        /* not consecutive row. 
		   print the previous row and its instances */
        if (i!=0 && !(is_samearray(curr_array, prev_array, nccols))) {
            print_values(prev_array, nccols);
            print_instances(instance);
            /* reset the counter. */
            instance = 0;
        }
        /* consecutive row. 
		   update the counter */
        instance += 1;
    }
	/* print out the values of last row and its instances */
	print_values(curr_array, nccols);
	print_instances(instance);
}

/* check if the 2 arrays have same items.
   return TRUE or FALSE 
*/
int
is_samearray(double array1[], double array2[], int n) {
    int i;
    for (i=0; i<n; i++) {
        if (array1[i]!=array2[i]) {
            return FALSE;
        }
    }
    return TRUE;
}

/* print the values inside the array 
*/
void
print_values(double array[], int n) {
    int i;
    for (i=0; i<n; i++) {
        printf(" %7.1f", array[i]);
    }
}

/* print the instances of the row 
*/
void
print_instances(int instance) {
    if (instance>1) {
        printf("    (%2d instances)\n", instance);
    } else {
        printf("    (%2d instance)\n", instance);
    }
}

/****************************************************************/

/* Stage 2 - Sorting */
/* the 's' sort command 
*/
void
do_sort(csv_t D, head_t H[], int dr, int dc,
        int ccols[], int nccols) {
    int i, j, c, ccolsnum;

	/* adapted from insertion sort program by Alistair Moffat */
    for (i=1; i< dr; i++) {
        for (j=i-1; j>=0; j--) {            
			ccolsnum = 0;
			c = ccols[ccolsnum];
			/* check value from next column 
			   if current row and next row values are same */
            next_column(D, ccols, &c, j, nccols, ccolsnum);	            
            /* current value is larger than next value. swap them */
			if(D[j][c]>D[j+1][c]) {
                row_swap(D, j, j+1, dc);
            } else {
				break;
			}
	    }
    }
    /* print out the headers of columns used to sort */
    sorted_by(H, ccols, nccols);    
}
    
/* swap the rows r1 and r2 from the array
   adapted from int_swap function by Alistair Moffat 
*/
void
row_swap(csv_t D, int r1, int r2, int dc) {
	double temp;
	int i;
	for (i=0; i<dc; i++) {
		temp = D[r2][i];
		D[r2][i] = D[r1][i];
		D[r1][i] = temp;
	}
}

/* print out the header of columns used to sort the rows. 
*/
void
sorted_by(head_t H[], int ccols[], int nccols) {
    int i, c;

    printf("\n");
    printf("    sorted by: ");
    for (i=0; i<nccols; i++) {
        c = ccols[i];
        printf("%s", H[c]);
        /* if it's not the last column, 
		   print a comma after the header. */
        if (!(i==(nccols-1))) {
            printf("%c ", CH_COMMA);
        }
    }
    printf("\n");
}

/* keep checking the next column indicated 
   by command if the row values are same 
*/
void
next_column(csv_t D, int ccols[], int *pc, 
			int j, int nccols, int ccolsnum) {    
	while (D[j][*pc]==D[j+1][*pc] && ccolsnum!=nccols) {
        ccolsnum += 1;
        *pc = ccols[ccolsnum];
    }
}

/****************************************************************/

/* Stage 3 - Plotting */
/* the 'p' plot command 
*/
void
do_plot(csv_t D, head_t H[], int dr, int dc, 
		int ccols[], int nccols) {    
    double min, max;
	double interval=0;

    printf("\n");
    min = max = D[0][ccols[0]];
    /* find the min and max value among the indicated columns. */
	find_minmax(D, dr, ccols, nccols, &min, &max);

    /* if range==0, all the values are the same.
       no need to plot the histogram */
    if (max-min==0) {
        printf("all selected elements are %.1f\n", max);
    } else {
		/* find the interval and print the graph */
		interval = ((max+1e-6)-(min-1e-6))/BANDS;
		print_histogram(D, dr, ccols, nccols, min, max, interval);
	}
}

/* find the largest and smallest double values 
   among the indicated columns. 
*/
void
find_minmax(csv_t D, int dr, int ccols[], int nccols, 
            double *pMin, double *pMax) {
    int i, j, c;
    
    for (i=0; i<dr; i++) {
        /* for each row, look through all indicated columns. */
        for (j=0; j<nccols; j++) {
            c = ccols[j];
            /* find the max value. */
            if (D[i][c]>*pMax) {
                *pMax = D[i][c];
            }
            /* find the min value. */
            if (D[i][c]<*pMin) {
                *pMin = D[i][c];
            }
        }
    }
}

/* print out the frequency histogram for indicated columns 
*/
void
print_histogram(csv_t D, int dr, int ccols[], int nccols, 
				double min, double max, double interval) {
    double lower_lim, upper_lim;
    int i=0, j, c, scale, elements;
    int elem_array[BANDS][nccols];

	/* initialise the elem_array */
	memset(elem_array, 0, sizeof elem_array);

	/* find the number of elements that are between upp and low limit,
	   in each indicated column. store them in elem_array */
    lower_lim = min-1e-6;
    while (i<BANDS) {
        upper_lim = lower_lim+interval;
        for (j=0; j<nccols; j++) {
            c = ccols[j];
            elements = count_elemnum(D, dr, ccols, nccols, 
							c, lower_lim, upper_lim);
            elem_array[i][j] = elements;
        }
		lower_lim = upper_lim;
		i++;
    }

	/* find the scale to use when plotting the graph */
	scale = find_scale(nccols, elem_array);

	/* plot the histogram */
	plot_graph(nccols, elem_array, ccols, interval, min, max, scale);
}

/* count all the elements that are between lower and upper limit
   in a given column 
*/
int
count_elemnum(csv_t D, int dr, int ccols[], int nccols, 
            int c, double lower_lim, double upper_lim) {
    int i, count=0;

    for (i=0; i<dr; i++) {
        if (D[i][c]>=lower_lim && D[i][c]<upper_lim) {
            count += 1;
        }
    }
    return count;
}

/* calculate the scale needed to use when plotting the histogram 
*/
int
find_scale(int nccols, int A[][nccols]) {
	int i, j, max = A[0][0], scale;

	/* find the largest num of elements */
	for (i=0; i<BANDS; i++) {
		for (j=0; j<nccols; j++) {
			if (A[i][j]>max) {
				max = A[i][j];
			}
		}
	}
	/* calculate scale */
	scale = (max/MAX_ELEM)+1;
	if (max%MAX_ELEM==0) {
		scale -= 1;
	}	
	return scale;
}

/* plot the histogram
*/
void
plot_graph(int nccols, int A[][nccols], int ccols[], 
		double interval, double min, double max, int scale) {
	int i=0, j, c, bars;
	double lower_lim, upper_lim;

	lower_lim = min-1e-6;
	while (i<BANDS) {
		upper_lim = lower_lim+interval;
		printf("%11.1f +\n", lower_lim);
		/* plot the num of elements that are between upp and low
		   limit for each indicated column */
		for (j=0; j<nccols; j++) {
			c = ccols[j];
			printf("%11d |", c);
			bars = A[i][j]/scale;
			print_bars(bars);
		}
		lower_lim = upper_lim;
		i++;
	}
	/* print the last upper limit (max) and the scale used */
	printf("%11.1f +\n", upper_lim);
	printf("    scale = %d\n", scale);
}

/* print out the bars 
*/
void
print_bars(int bars) {
	int i;
	for (i=0; i<bars; i++) {
		printf("]");
	}
	printf("\n");
}

/****************************************************************/

/* algorithms are fun. */