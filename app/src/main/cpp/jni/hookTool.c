#include <hookTool.h>
#include <stdio.h>//fopen,fgets snprintf
#include <string.h>//strstr strtok
#include <stdlib.h>
#include "common.h"//��ӡ��־

//�޸��ڴ�����Ϊ��д��
//��ȡָ�������ڵ�so���ڵĻ���ַ
//�������staic�����˽�У����ļ�����ֻ���Լ��ļ��ɼ�������ȥ��
void *get_module_base(pid_t pid, const char *module_name) {
    FILE *fp;
    long addr = 0;
    char *pch;
    char filename[32];
    char line[1024];
    if (pid < 0) {
        /* self process */
        snprintf(filename, sizeof(filename), "/proc/self/maps");
    } else {
        snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
    }
    fp = fopen(filename, "r");
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, module_name)) {
                pch = strtok(line, "-");
                addr = strtoul(pch, NULL, 16);

                if (addr == 0x8000)
                    addr = 0;

                break;
            }
        }

        fclose(fp);
    }
    return (void *) addr;
}
