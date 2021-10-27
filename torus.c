#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct _inf{
    int R;
    int S;
    int T;
    int flag_console;
    int flag_binary;
    char input_file[200];
    char output_file[200];
}Inf;

typedef struct _torus{
    int **torus_in;
    int **torus_out;
}Torus;

int print_console(int **torus, Inf *data)               // print torus on the console
{
    for (int i = 0; i< data->R; i++) {
        for (int j = 0; j < data->S; j++) {
            if (torus[i][j] == 1)
                printf("*");
            else
                printf(" ");
        }
        printf("\n");
    }
    return 0;
}

int CGol(Torus torus, int r, int s, Inf *data)      // calculate whether the cell will live or die
{
    int count = 0;
    int pr, ps;
    int flag = 0;

    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
                continue;
            pr = r + i;
            ps = s + j;
            if (pr == -1)
                pr = (data->R) - 1;
            else
                if (pr == (data->R))
                    pr = 0;
            if (ps == -1)
                ps = (data->S) - 1;
            else
                if (ps == (data->S))
                    ps = 0;
            if (torus.torus_in[pr][ps] == 1)
                count++;
        }
    if (count < 2)
        flag = 0;
    if (count > 3)
        flag = 0;
    if (count == 3 )
        flag = 1;
    if ((count == 2) && (torus.torus_in[r][s] == 1))
        flag = 1;

    return flag;
}

int next_state(Torus torus, Inf *data)                  // generates another state of Torus
{
    for (int r = 0; r < data->R; r++)
        for (int s = 0; s < data->S; s++)
        {
            torus.torus_out[r][s] = CGol(torus, r, s, data);
        }
    return 0;
}

int copy_torus(int **torus1, int **torus2, Inf *data)           // copy torus2 to torus1
{
    for (int r = 0; r < data->R; r++)
        for (int s = 0; s < data->S; s++)
        {
            torus1[r][s] = torus2[r][s];
        }
    return 0;
}

int evolve(Torus torus, Inf *data)                          // generate torus for n iterations
{
    print_console(torus.torus_in,data);
    for (int i = 0; i < data->T; i++ ) {
        next_state(torus, data);
        if (data->flag_console)
            print_console(torus.torus_out, data);
        copy_torus(torus.torus_in, torus.torus_out, data);
    }
    return 0;
}

int **build_array(Inf *data)                                    // generate 2D dynamical array
{

    int **array;

    array = (int **) malloc((data->R) * sizeof(int *));
    for (int i= 0; i< data->R; i++)
    {
        array[i] = (int *) malloc(data->S * sizeof(int));
    }
    return array;
}

int free_array(Torus torus, Inf *data)              // free toruses
{

    for(int r = 0; r < data->R; r++)
    {
        free(torus.torus_in[r]);
        free(torus.torus_out[r]);
    }
    free(torus.torus_in);
    free(torus.torus_out);
    return 0;
}

int read_text_file(Inf *data, Torus torus)
{
    int ch;
    FILE *fptr1;
    fptr1 = fopen(data->input_file, "r");

    if (fptr1 == NULL)
    {
        exit(-1);
    }

    for (int r = 0; r < data->R; r++) {
        for (int s = 0; s < data->S; s++) {
            ch = fgetc(fptr1);
            if (feof(fptr1))
                ch = '0';
            if (ch == '\n')
                ch = fgetc(fptr1);
            torus.torus_in[r][s] = ch - '0';
        }

    }
    fclose(fptr1);
    return 0;
}

int read_binary_file(Inf *data, Torus torus)
{
    char c;
    int bool;
    FILE *fptr2;
    fptr2 = fopen(data->input_file, "rb");

    if (fptr2 == NULL)
    {
        exit(-1);
    }

    for (int r = 0; r < data->R; r++) {
        fread(&c, sizeof(char), 1, fptr2);
        if (feof(fptr2))
            c = 0;
        for (int s = 0;  s < data->S; s++) {
            bool = c % 2;
            torus.torus_in[r][s] = bool;
            c /= 2;
        }

    }
    fclose(fptr2);
    return 0;

}

int write_binary_file(Inf *data, Torus torus)
{
    int res;
    int root;

    root = 1;
    res = 0;
    FILE *fptr3;
    fptr3 = fopen(data->output_file, "wb");

    if (fptr3 == NULL)
    {
        exit(-1);
    }

    for (int r=0; r< data->R; r++) {
        for (int s = 0; s < data->S; s++) {
            res += (torus.torus_out[r][s]) * root;
            root *= 2;
        }
        fwrite(&res, sizeof(char),1, fptr3);
        res = 0;
        root = 1;
    }
    fclose(fptr3);
    return 0;

}
int write_text_file(Inf *data, Torus torus)
{
    FILE *fptr4;
    fptr4 = fopen(data->output_file, "w");
    if (fptr4 == NULL)
        exit(-1);

    for (int r=0; r< data->R; r++) {
        for (int s = 0; s < data->S; s++) {
            fprintf(fptr4, "%d", torus.torus_out[r][s]);
        }
        fprintf(fptr4, "\n");
    }
    fclose(fptr4);
    return 0;
}

int is_positive_number(char str[]){
    if (*str == '\0')
        return 0;
    while(*str!='\0' ){
        if (!((*str >= 48 && *str <= 57)))          // from Ascii Table
            return 0;
        str++;
    }
    return 1;
}

int process_switches(int argc, char* argv[], Inf *data)
{
    int opt;

    while((opt = getopt(argc,argv,"vb"))!= -1)
    {
        switch(opt)
        {
            case 'v':
                data->flag_console = 1;
                break;
            case 'b':
                data->flag_binary = 1;
                break;
            case '?':
                exit(0);
            default:
                break;
        }
    }
    if ((argc - optind) < 5)
        exit(0);

    if (!is_positive_number(argv[optind]))
        exit(0);
    data->R = atoi(argv[optind]);
    optind++;
    if (!is_positive_number(argv[optind]))
        exit(0);
    data->S = atoi(argv[optind]);
    optind++;
    if (!is_positive_number(argv[optind]))
        exit(0);
    data->T = atoi(argv[optind]);
    optind++;

    if ((data->S) <= 0)
        exit(0);

    if ((data->R) <= 0)
        exit(0);

    if ((data->T) < 0)
        exit(0);

    if ((data->S) % 8 !=0)
        data->S = data->S + (8 - ((data->S) % 8));

    if ((fopen(argv[optind],"rb")==NULL) && (data->flag_binary == 1 ))
        exit(0);
    if ((fopen(argv[optind],"r")==NULL) && (data->flag_binary == 0))
        exit(0);

    strcpy(data->input_file, argv[optind]);
    optind++;

    if ((fopen(argv[optind],"wb")==NULL) && (data->flag_binary == 1))
        exit(0);
    if ((fopen(argv[optind],"w")==NULL) && (data->flag_binary == 0))
        exit(0);
    strcpy(data->output_file, argv[optind]);
    return 0;
}


int main(int argc, char *argv[]) {

    Inf Data;
    Inf *data;
    Torus torus;
    data = &Data;
    opterr = 0;
    data->flag_binary = 0;
    data->flag_console = 0;

    process_switches(argc,argv,data);
    torus.torus_in = build_array(data);
    torus.torus_out = build_array(data);

    if (data->flag_binary) {
        read_binary_file(data, torus);
        evolve(torus, data);
        write_binary_file(data, torus);
    }
    else {
        read_text_file(data, torus);
        evolve(torus, data);
        write_text_file(data, torus);
    }

    free_array(torus, data);
    return 0;
}
