#include <stdio.h>

//Memória Virtual = vai conter a lista todas as páginas ex: 1,2,3,4,5,6,1,2,3,4,5,6,1,2,3,4,5,6,1,2,3,4,5,6
//Memória Real    = vai conter o número de frames ex com 4 frames : | 1 | | 2 | | 3 | | 4 |

void iniciar_memoria_real(int mr_frames[], int numero_frames_mr)
{
    int i = 0;
    for (i; i < numero_frames_mr; ++i)
    {
        mr_frames[i] = -1;
    }
}

int exec_mru(int tempo[], int numero_frames)
{
    int i;
    int min = tempo[0];
    int posicao_vetor = 0;

    for (i = 1; i < numero_frames; ++i)
    {
        if (tempo[i] < min)
        {
            min = tempo[i];
            posicao_vetor = i;
        }
    }

    return posicao_vetor;
}

void exec_pagination(int numero_paginas_mv, int numero_frames_mr, int mr_frames[], int mv_paginas[])
{
    int i, j;
    int contador = 0, page_miss = 0;
    int frames_vazios, pagina_atual_alocada;
    int tempo_mr[numero_frames_mr];
    int posicao_atual = 0;

    for (i = 0; i < numero_paginas_mv; ++i)
    {
        frames_vazios = pagina_atual_alocada = 0;

        for (j = 0; j < numero_frames_mr; ++j)
        {
            if (mr_frames[j] == mv_paginas[i])
            {
                contador++;
                tempo_mr[j] = contador;
                frames_vazios = pagina_atual_alocada = 1;
                break;
            }
        }

        if (frames_vazios == 0)
        {
            for (j = 0; j < numero_frames_mr; ++j)
            {
                if (mr_frames[j] == -1)
                {
                    contador++;
                    page_miss++;
                    mr_frames[j] = mv_paginas[i];
                    tempo_mr[j] = contador;
                    pagina_atual_alocada = 1;
                    break;
                }
            }
        }

        if (pagina_atual_alocada == 0)
        {
            posicao_atual = exec_mru(tempo_mr, numero_frames_mr);
            contador++;
            page_miss++;
            mr_frames[posicao_atual] = mv_paginas[i];
            tempo_mr[posicao_atual] = contador;
        }
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
    
    int mr_frames[numero_frames_mr];
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
