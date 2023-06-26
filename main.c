#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char* name;
    int duration;
} Task;

int main() {
    // timeUnite (semana)
    const char* timeUnit = "/s";

    FILE* file = fopen("tarefas.txt", "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    // Contar o número de tarefas no arquivo
    int numTasks = 0;
    char line[100];
    while (fgets(line, sizeof(line), file) != NULL) {
        numTasks++;
    }

    rewind(file);

    Task* tasks = (Task*)malloc(numTasks * sizeof(Task));
    if (tasks == NULL) {
        printf("Erro ao alocar memória.\n");
        return 1;
    }

    // Ler as tarefas do arquivo
    int i = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        char name[50];
        int duration;
        sscanf(line, "%s %d", name, &duration);
        tasks[i].name = strdup(name);
        tasks[i].duration = duration;
        i++;
    }

    fclose(file);

    int dependencies[][13] = {
        {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    // Cálculo (ES)
    int ES[13] = {0};
    for (int i = 1; i < numTasks; i++) {
        ES[i] = 0;
        for (int j = 0; j < numTasks; j++) {
            if (dependencies[j][i] && (ES[j] + tasks[j].duration > ES[i])) {
                ES[i] = ES[j] + tasks[j].duration;
            }
        }
    }

    // Cálculo (EF)
    int EF[13];
    for (int i = 0; i < numTasks; i++) {
        EF[i] = ES[i] + tasks[i].duration;
    }

    // Cálculo das datas de início mais tarde (LS)
    int LS[13];
    int LF[13];
    LF[numTasks - 1] = EF[numTasks - 1];
    for (int i = numTasks - 2; i >= 0; i--) {
        LF[i] = LF[i + 1];
        for (int j = numTasks - 1; j >= 0; j--) {
            if (dependencies[i][j] && (LF[j] - tasks[i].duration < LF[i])) {
                LF[i] = LF[j] - tasks[i].duration;
            }
        }
    }
    for (int i = 0; i < numTasks; i++) {
        LS[i] = LF[i] - tasks[i].duration;
    }

    // Encontrando o caminho crítico
    int criticalPath[13] = {0};
    for (int i = 0; i < numTasks; i++) {
        if (ES[i] == LS[i]) {
            criticalPath[i] = 1;
        }
    }

    // Imprimindo os resultados
    printf("Tarefa\t\tES\tEF\tLS\tLF\tFloat\n");
    for (int i = 0; i < numTasks; i++) {
        int floatTime = LF[i] - EF[i];
        printf("%s\t%d %s\t%d %s\t%d %s\t%d %s\t%d %s\n", tasks[i].name, ES[i], timeUnit, EF[i], timeUnit, LS[i], timeUnit, LF[i], timeUnit, floatTime, timeUnit);
    }

    printf("\nCaminho Crítico:\n");
    for (int i = 0; i < numTasks; i++) {
        if (criticalPath[i]) {
            printf("%s ", tasks[i].name);
        }
    }

    // Liberar a memória alocada
    for (int i = 0; i < numTasks; i++) {
        free(tasks[i].name);
    }
    free(tasks);

    return 0;
}
