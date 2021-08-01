#include <stdio.h>

//Memória Virtual = vai conter a lista todas as páginas ex: 1,2,3,4,5,6,1,2,3,4,5,6,1,2,3,4,5,6,1,2,3,4,5,6
//Memória Real    = vai conter o número de frames ex com 4 frames : | 1 | | 2 | | 3 | | 4 |

struct tipo_pagina
{
    int frame_id;
    int pagina_id;
    int tempo;
};

typedef struct tipo_pagina pagina;

void iniciar_memoria_real(pagina mr_frames[], int numero_frames_mr)
{
    int i = 0;
    for (i; i < numero_frames_mr; ++i)
    {
        mr_frames[i].frame_id = i;
        mr_frames[i].pagina_id = -1;
        mr_frames[i].tempo = 0;
    }
}

int exec_mru(pagina mr_frames[], int numero_frames)
{
    int i;
    int min = mr_frames[0].tempo;
    int posicao_vetor = 0;

    for (i = 1; i < numero_frames; ++i)
    {
        if (mr_frames[i].tempo < min)
        {
            min = mr_frames[i].tempo;
            posicao_vetor = i;
        }
    }

    return posicao_vetor;
}

void print_tempos(pagina mr_frames[], int numero_frames_mr)
{
    int i;
    for (i = 0; i < numero_frames_mr; ++i)
    {
       printf("frame[%d]: %d\n", i, mr_frames[i].tempo);
    }
}

void print_config_pagina(pagina mr_frames[], int numero_frames_mr)
{
    int i;
    for (i = 0; i < numero_frames_mr; ++i)
    {
       printf("frame[%d]: %d\n", i, mr_frames[i].pagina_id);
    }
}

void exec_pagination(int numero_paginas_mv, int numero_frames_mr, pagina mr_frames[], int mv_paginas[])
{
    int i, j;
    int contador = 0, page_miss = 0;
    int frames_vazios, pagina_atual_alocada;
    int posicao_atual = 0;

    for (i = 0; i < numero_paginas_mv; ++i)
    {
        frames_vazios = pagina_atual_alocada = 1;

        for (j = 0; j < numero_frames_mr; ++j)
        {
            if (mr_frames[j].pagina_id == mv_paginas[i])
            {
                contador++;
                mr_frames[j].tempo = contador;
                frames_vazios = 0;
                pagina_atual_alocada = 0;
                break;
            }
        }

        if (frames_vazios == 1)
        {
            for (j = 0; j < numero_frames_mr; ++j)
            {
                if (mr_frames[j].pagina_id == -1)
                {
                    contador++;
                    page_miss++;
                    mr_frames[j].pagina_id = mv_paginas[i];
                    mr_frames[j].tempo = contador;
                    pagina_atual_alocada = 0;
                    break;
                }
            }
        }

        if (pagina_atual_alocada == 1)
        {
            posicao_atual = exec_mru(mr_frames, numero_frames_mr);
            contador++;
            page_miss++;
            mr_frames[posicao_atual].pagina_id = mv_paginas[i];
            mr_frames[posicao_atual].tempo = contador;
            printf("\nPage miss foi incrementado!\n");
        }

        printf("\nTempos rodada[%d]\n", i);
        print_tempos(mr_frames, numero_frames_mr);

        printf("\nConfiguracao das paginas na rodada[%d]\n", i);
        print_config_pagina(mr_frames, numero_frames_mr);
    }

    printf("\n\nPage miss = %d", page_miss);
}

int main(int argc, char const *argv[])
{
    int numero_frames_mr, numero_paginas_mv;
    int i, j;

    printf("Numero de frames da memoria real: ");
    scanf("%d", &numero_frames_mr);

    printf("Numero de paginas na memoria virtual: ");
    scanf("%d", &numero_paginas_mv);

    pagina mr_frames[numero_frames_mr];
    int mv_paginas[numero_paginas_mv];

    printf("Digite o identificador de cada pagina: \n");
    for (i = 0; i < numero_paginas_mv; ++i)
    {
        // printf("mv[%d]: ", i);
        scanf("%d", &mv_paginas[i]);
    }

    iniciar_memoria_real(mr_frames, numero_frames_mr);

    exec_pagination(numero_paginas_mv, numero_frames_mr, mr_frames, mv_paginas);

    return 0;
}
