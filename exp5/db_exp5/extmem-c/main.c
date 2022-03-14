# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdbool.h>
# include "extmem.h"


typedef struct tuple {
    int x;
    int y;
}tuple;

int LinearSelect();
int TwoPhaseMultiwayMergeSort(int start_blk_addr, int end_blk_addr, int write_base_addr);
int makeIndex(int start_addr, int end_addr, int index_start_addr);
int searchByIndex(int index_start_addr, int index_num, int pattern, int write_base_addr);
int join(int a_start_addr, int a_num, int b_start_addr, int b_num, int write_base_addr);
int intersect(int a_start_addr, int a_num, int b_start_addr, int b_num, int write_base_addr);

tuple readTuple(char* blks[], int index, int length);
bool checkIdx(int idx[], int num);
void getTuplesFromBlock(char* blk, int num, tuple* ts);
void task1(void);
void task2(void);
void task3(void);
void task4(void);
void task5(void);



int main(void)
{
    task1();
    task2();
    task3();
    task4();
    task5();
    getchar();
	return 0;
}

void task1(void)
{
    /*��ʽ�����*/
    printf("==================================\n");
    printf("��������������ѡ���㷨S_C=130\n");
    printf("==================================\n");
    LinearSelect();
    return;
}

void task2(void)
{
    printf("==================================\n");
    printf("����ɨ�������㷨\n");
    printf("==================================\n");
    int IOTimes = 0;
    int tmp = TwoPhaseMultiwayMergeSort(1, 16, 201);
    if (tmp == -1)
    {
        printf("Something Wrong!\n");
        return;
    }
    IOTimes += tmp;
    tmp = TwoPhaseMultiwayMergeSort(17, 48, 217);
    if (tmp == -1)
    {
        printf("Something Wrong!\n");
        return;
    }
    IOTimes += tmp;
    printf("IO����Ϊ%d\n", IOTimes);
    return;
}

void task3(void)
{
    printf("==================================\n");
    printf("���������������㷨\n");
    printf("==================================\n");
    //LinearSelect();
    printf("==================================\n");
    printf("��������\n");
    printf("==================================\n");
    int index_num = makeIndex(217, 248, 301);
    printf("==================================\n");
    printf("����������������\n");
    printf("==================================\n");

    int IOTimes = searchByIndex(301, index_num, 130, 350);
    printf("IOTimes: %d\n", IOTimes);

    return;
}

void task4(void)
{
    printf("==================================\n");
    printf("��������������㷨\n");
    printf("==================================\n");
    int joinTimes = join(201, 16, 217, 32, 401);
    printf("���Ӳ�������:%d\n", joinTimes);
}

void task5(void)
{
    printf("==================================\n");
    printf("��������Ĺ�ϵ���㷨\n");
    printf("==================================\n");
    int intersectTimes = intersect(201, 16, 217, 32, 601);
    printf("����Ԫ������:%d\n", intersectTimes);
}

/// <summary>
/// �������������Ĺ�ϵѡ���㷨��ѡ��S.C=130��Ԫ��
/// </summary>
int LinearSelect()
{
    Buffer buf; /* A buffer */
    unsigned char* read_blk; /* A pointer to a block */
    unsigned char* write_blk;


    /* Initialize the buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }

    read_blk = getNewBlockInBuffer(&buf);
    write_blk = getNewBlockInBuffer(&buf);
    int read_blk_addr = 17;     // S��ϵ��һ�����ַΪ17
    int write_blk_addr = 100;   // �����ŵĿ��ʼ��ַ
    int count = 0;              // ͳ���ҵ�������
    int write_blk_count = 0;    // ͳ����д��Ŀ���

    while(1)
    {
        // ��ȡS��ϵһ����
        if ((read_blk = readBlockFromDisk(read_blk_addr, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        // ���
        printf("��ȡ���̿�%d\n", read_blk_addr);

        // ���ζ�ȡ7��Ԫ��
        for (int i = 0; i < 7; i++)
        {
            char str[5];
            memcpy(str, read_blk + i * 8, 4);
            int S_C = atoi(str);
            memcpy(str, read_blk + i * 8 + 4, 4);
            int S_D = atoi(str);
            if (S_C == 130)
            {
                // ���
                printf("(%d, %d)\n", S_C, S_D);
                // д��д��飬���ж��Ƿ��������Ҫд�����
                memcpy(write_blk + count * 8, read_blk + i * 8, 8);
                count++;
                if (count==7)
                {
                    // �ڿ����д����һ��ĵ�ַ
                    itoa(write_blk_addr + write_blk_count+1, write_blk + 7 * 8, 10);
                    /* Write the block to the hard disk */
                    if (writeBlockToDisk(write_blk, write_blk_addr+write_blk_count, &buf) != 0)
                    {
                        perror("Writing Block Failed!\n");
                        return -1;
                    }
                    write_blk_count++;
                    count = 0;
                }
            }
        }
        // ��ȡ���һ��int��ȡ��һ�����ַ
        char str[5];
        memcpy(str, read_blk + 7 * 8, 4);
        int addr = atoi(str);
        if (addr < 17 || addr > 48)
            break;
        freeBlockInBuffer(read_blk, &buf);
        read_blk_addr = addr;
    }

    // ������д����ڻ������ݣ���Ҫ�����д�����
    if (count != 0)
    {
        if (writeBlockToDisk(write_blk, write_blk_addr + write_blk_count, &buf) != 0)
        {
            perror("Writing Block Failed!\n");
            return -1;
        }
        write_blk_count++;
    }
    // ������ݴ��λ��
    printf("���ݴ����:");
    for (int i = 0; i < write_blk_count; i++)
    {
        printf("%d", write_blk_addr + i);
    }
    printf("\n");
    // ���Ԫ�������IO����
    printf("�����õ���Ԫ�����:%d\n", (write_blk_count-1) * 7 + count);
    printf("IO����:%d\n", buf.numIO);
    freeBuffer(&buf);
    return 0;
}

/// <summary>
/// ���׶ζ�·�����������Ƚ���ϵ����ΪС��Ԫ���������Ȼ���ٶ�������Ӽ����й鲢����
/// </summary>
/// <param name="start_blk_addr">��ʼ���̵�ַ</param>
/// <param name="end_blk_addr">�������̵�ַ</param>
/// <param name="write_base_addr">д����̻���ַ</param>
/// <returns>IO��д����</returns>
int TwoPhaseMultiwayMergeSort(int start_blk_addr, int end_blk_addr, int write_base_addr)
{
    Buffer buf; /* A buffer */

    /* Initialize the buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    int write_addr = write_base_addr;

    // ÿ�ζ���6���������ֱ��7��ȫ��������
    int rd_blk_ptr = start_blk_addr;

    unsigned char* sort_blk[6]; // �洢���������ݿ�
    unsigned char* write_blk;   // �洢��д�����ݿ�
    for (int i = 0; i < 6; i++)
        sort_blk[i] = NULL;
    write_blk = NULL;
    int write_blk_idx = 0;      // ��¼��д����д���ַ�±꣬Ϊ7ʱ��Ҫд�����
    while (1)
    {
        // ����д���
        write_blk = getNewBlockInBuffer(&buf);
        // ���㱾������Ŀ�����num
        int num = (end_blk_addr - rd_blk_ptr + 1 < 7) ? end_blk_addr - rd_blk_ptr + 1: 6;
        // ����num����
        for (int i = 0; i < num; i++)
        {
            /* Read the block from the hard disk */
            if ((sort_blk[i] = readBlockFromDisk(rd_blk_ptr+i, &buf)) == NULL)
            {
                perror("Reading Block Failed!\n");
                return -1;
            }
        }
        // ������ÿ����������
        for (int i = 0; i < num; i++)
        {
            // ���������ݶ�ȡ��tuple���鵱��
            tuple ts[7];
            for (int j = 0; j < 7; j++)
            {
                char str[5];
                memcpy(str, sort_blk[i] + j*8, 4);
                ts[j].x = atoi(str);
                memcpy(str, sort_blk[i] + j * 8 + 4, 4);
                ts[j].y = atoi(str);
            }
            // ����ѡ��������п�������
            for (int j = 0; j < 6; j++)
            {
                int min = ts[j].x;
                int min_idx = j;
                for (int k = j+1; k < 7; k++)
                {
                    if (ts[k].x < min)
                    {
                        min = ts[k].x;
                        min_idx = k;
                    }
                }
                // ��idx��Ӧ��Ԫ�黻��λ��j��
                tuple tmp = ts[j];
                ts[j] = ts[min_idx];
                ts[min_idx] = tmp;
            }
            // ��������Ԫ��д�ؿ���
            for (int j = 0; j < 7; j++)
            {
                char str[5];
                itoa(ts[j].x, str, 10);
                memcpy(sort_blk[i] + j * 8, str, 4);
                itoa(ts[j].y, str, 10);
                memcpy(sort_blk[i] + j * 8 + 4, str, 4);
            }
        }
        // ���ù鲢��������õ�6����д��write_blk,��write_blk����д�����
        int idx[6];        // num���±�ָ��num���������
        for (int i = 0; i < num; i++)
            idx[i] = 0;
        while (!checkIdx(idx, num))
        {
            // ��ÿ�����ڵ�һ��Ԫ�ض�������tuple��
            tuple element[6];
            for (int i = 0; i < num; i++)
            {
                // ��δ�������������Ԫ��
                if (idx[i] < 7)
                {
                    char str[5];
                    memcpy(str, sort_blk[i] + idx[i] * 8, 4);
                    element[i].x = atoi(str);
                    memcpy(str, sort_blk[i] + idx[i] * 8 + 4, 4);
                    element[i].y = atoi(str);
                }
                else
                    element[i].x = 0x7fffffff;
            }
            // �ҳ�num��Ԫ������С��һ��д��write_blk
            int min = element[0].x;
            int min_idx = 0;
            for (int i = 1; i < num; i++)
            {
                if (element[i].x < min)
                {
                    min = element[i].x;
                    min_idx = i;
                }
            }
            // д��write_blk
            char str[5];
            itoa(element[min_idx].x, str, 10);
            memcpy(write_blk + write_blk_idx * 8, str, 4);
            itoa(element[min_idx].y, str, 10);
            memcpy(write_blk + write_blk_idx * 8 + 4, str, 4);
            write_blk_idx++;
            // write_blk����Ҫд�����
            if (write_blk_idx == 7)
            {
                itoa(write_addr + 1, write_blk + 7 * 8, 10);
                if (writeBlockToDisk(write_blk, write_addr, &buf) != 0)
                {
                    perror("Writing Block Failed!\n");
                    return -1;
                }
                // ����д���
                write_blk = getNewBlockInBuffer(&buf);
                write_addr++;
                write_blk_idx = 0;
            }
            idx[min_idx] = (idx[min_idx] == 7) ? 7 : idx[min_idx]+1;
        }
        rd_blk_ptr += num;
        // �ͷ�num������ڴ�
        for (int i = 0; i < num; i++)
        {
            freeBlockInBuffer(sort_blk[i], &buf);
        }
        freeBlockInBuffer(write_blk, &buf);
        write_blk = NULL;
        if (rd_blk_ptr == end_blk_addr + 1)
        {
            break;
        }
    }

    // ���Ѿ��ֺ���Ŀ��ٽ��й鲢����
    // ���ȼ�����˶����飬���һ����ٿ�
    int group_num = (end_blk_addr - start_blk_addr + 1) / 6;    // ������
    int last_group_num;                                         // ���һ���ڿ�����
    if ((end_blk_addr - start_blk_addr + 1) % 6 == 0)
    {
        last_group_num = 0;
    }
    else
    {
        last_group_num = (end_blk_addr - start_blk_addr + 1) % 6;
        group_num++;
    }

    // ����������>���������(7)�����
    int new_idx[7];    // ����ָ��,ָ�����Ԫ�飬��Χ��0<=new_idx<7
    int block_idx[6];  // ����ָ��,ָ�����ڿ飬��Χ��0<=block_idx<6 or last_group_num
    for (int i = 0; i < group_num; i++)
    {
        new_idx[i] = 0;
        block_idx[i] = 0;
    }
    // ����������ĵ�һ��
    for (int i = 0; i < group_num; i++)
    {
        if ((sort_blk[i] = readBlockFromDisk(write_base_addr+i*6, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
    }
    // ����д���
    write_blk = getNewBlockInBuffer(&buf);
    // ��ʼ�鲢����
    while (1)
    {
        tuple element[7];
        // ��ȡÿ��������Ӧ���±�Ԫ��
        for (int i = 0; i < group_num; i++)
        {
            if (new_idx[i] < 7)
            {
                char str[5];
                memcpy(str, sort_blk[i] + new_idx[i] * 8, 4);
                element[i].x = atoi(str);
                memcpy(str, sort_blk[i] + new_idx[i] * 8 + 4, 4);
                element[i].y = atoi(str);
            }
            else
                element[i].x = 0x7fffffff;

        }
        // �ҳ���СԪ��
        int min = element[0].x;
        int min_idx = 0;
        for (int i = 1; i < group_num; i++)
        {
            if (element[i].x < min)
            {
                min = element[i].x;
                min_idx = i;
            }
        }
        // ����СԪ��д��write_blk
        // д��write_blk
        char str[5];
        itoa(element[min_idx].x, str, 10);
        memcpy(write_blk + write_blk_idx * 8, str, 4);
        itoa(element[min_idx].y, str, 10);
        memcpy(write_blk + write_blk_idx * 8 + 4, str, 4);
        write_blk_idx++;
        // д�����ʱ��Ҫд�����
        if (write_blk_idx == 7)
        {
            itoa(write_addr + 1, write_blk + 7 * 8, 10);
            if (writeBlockToDisk(write_blk, write_addr, &buf) != 0)
            {
                perror("Writing Block Failed!\n");
                return -1;
            }
            write_blk = getNewBlockInBuffer(&buf);
            write_addr++;
            write_blk_idx = 0;
        }

        // ������һ��Ԫ��
        // �����ȡ�����ڵ����һ��Ԫ����
        if (new_idx[min_idx] == 6)
        {
            // �ж��Ƿ��ȡ���������һ��
            // ��������ж��Ƿ������һ�飬�Ӷ�֪�����ڿ������
            if (min_idx == group_num - 1)
            {
                // �����һ��
                if (block_idx[min_idx] == last_group_num-1)
                    new_idx[min_idx] = 7;
                else
                {
                    block_idx[min_idx]++;
                    new_idx[min_idx] = 0;
                    // ���ͷŵ�ǰ�飬�ٻ�ȡ�µĿ�
                    freeBlockInBuffer(sort_blk[min_idx], &buf);
                    sort_blk[min_idx] = readBlockFromDisk(write_base_addr + min_idx * 6 + block_idx[min_idx], &buf);
                }
            }
            // �������һ�飬������6��
            else
            {
                // �����һ��
                if (block_idx[min_idx] == 5)
                    new_idx[min_idx] = 7;
                else
                {
                    block_idx[min_idx]++;
                    new_idx[min_idx] = 0;
                    // ���ͷŵ�ǰ�飬�ٻ�ȡ�µĿ�
                    freeBlockInBuffer(sort_blk[min_idx], &buf);
                    sort_blk[min_idx] = readBlockFromDisk(write_base_addr + min_idx * 6 + block_idx[min_idx], &buf);
                }
            }

        }
        else
            new_idx[min_idx]++;

        // ���鲢�Ƿ����
        if (checkIdx(new_idx, group_num))
            break;
    }
     //�ͷſ�ռ��
    for (int i = 0; i < group_num; i++)
        freeBlockInBuffer(sort_blk[i], &buf);
    freeBlockInBuffer(write_blk, &buf);

    // �����м����ռ�ã���������ɵĽ��ת�Ƶ�write_base_addr��ʼ
    int num_blk = end_blk_addr - start_blk_addr + 1;
    int sort_blk_addr = write_addr - num_blk;
    for (int i = 0; i < num_blk; i++)
    {
        write_blk = readBlockFromDisk(sort_blk_addr + i, &buf);
        itoa(write_base_addr + i + 1, write_blk + 7 * 8, 10);
        if (writeBlockToDisk(write_blk, write_base_addr + i, &buf) != 0)
        {
            perror("Writing Block Failed!\n");
            return -1;
        }
        dropBlockOnDisk(sort_blk_addr + i);
    }
    int IOTimes = buf.numIO;
    freeBuffer(&buf);

    return IOTimes;
}

/// <summary>
/// Ϊ�Ѿ��ź����Ԫ�齨�������������������Ԫ��ĵ�һ������
/// ����Ϊÿ���ļ�����һ�������ķ�ʽ������ֵΪ���̺�*7+�����ڿ��
/// </summary>
/// <param name="start_addr">Ԫ����ʼ���̵�ַ</param>
/// <param name="end_addr">Ԫ����ֹ���̵�ַ</param>
/// <param name="index_start_addr">���������ʼ��ַ</param>
/// <returns>���������������󷵻�-1</returns>
int makeIndex(int start_addr, int end_addr, int index_start_addr)
{
    Buffer buf;
    char* blk;
    char* write_blk;
    int write_addr = index_start_addr;
    int write_blk_idx = 0;     // д����±꣬Ϊ7ʱд�����
    /* Initialize the buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    write_blk = getNewBlockInBuffer(&buf);
    // �������п齨������
    for (int addr = start_addr; addr <= end_addr; addr++)
    {
        blk = readBlockFromDisk(addr, &buf);
        // ���������ݶ�ȡ��tuple���鵱��
        tuple ts[7];
        getTuplesFromBlock(blk, 7, ts);
        int prev = ts[0].x;
        // �������̿��ڵ�Ԫ��Ѱ�Ҵ�����Ԫ��
        for (int i = 1; i < 7; i++)
        {
            int cur = ts[i].x;
            // �ڴ��̿���Ѱ���������ڵĲ�ͬ��Ԫ�أ�ȷ������ָ�����Ԫ��ĵ�һ��
            if (cur != prev)
            {
                int idx_num = addr * 7 + i;
                // д���ڴ�飬�ڴ����д����̿�
                char str[5];
                itoa(ts[i].x, str, 10);
                memcpy(write_blk + write_blk_idx * 8, str, 4);
                itoa(idx_num, str, 10);
                memcpy(write_blk + write_blk_idx * 8 + 4, str, 4);
                write_blk_idx++;
                // write_blk����Ҫд�����
                if (write_blk_idx == 7)
                {
                    itoa(write_addr + 1, write_blk + 7 * 8, 10);
                    if (writeBlockToDisk(write_blk, write_addr, &buf) != 0)
                    {
                        perror("Writing Block Failed!\n");
                        return -1;
                    }
                    // ����д���
                    write_blk = getNewBlockInBuffer(&buf);
                    write_addr++;
                    write_blk_idx = 0;
                }
                freeBlockInBuffer(blk, &buf);
                break;
            }
            prev = cur;
            if (i == 6)
                freeBlockInBuffer(blk, &buf);
        }
    }
    // ������д��黹��Ԫ����Ҫд�������
    if (write_blk_idx != 0)
    {
        memset(write_blk + write_blk_idx * 8, 0, 64 - write_blk_idx * 8);
        itoa(write_addr + 1, write_blk + 7 * 8, 10);
        if (writeBlockToDisk(write_blk, write_addr, &buf) != 0)
        {
            perror("Writing Block Failed!\n");
            return -1;
        }
        write_addr++;
    }

    return write_addr - index_start_addr;
}

/// <summary>
/// ͨ����������Ԫ��
/// </summary>
/// <param name="index_start_addr">��������ʼ��ַ</param>
/// <param name="index_num">����������</param>
/// <param name="pattern">������Ԫ��ֵ</param>
/// <param name="write_base_addr">�����ŵĴ�����ʼ��ַ</param>
/// <returns>IO����</returns>
int searchByIndex(int index_start_addr, int index_num, int pattern, int write_base_addr)
{
    Buffer buf;
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    char* blk, * write_blk;
    int start = 0, end = 0;     // ͨ���������ҵ���Ŀ�귶Χ
    // �������������Ĵ���Ѱ�ҷ�Χ
    for (int addr = index_start_addr; addr < index_start_addr+index_num; addr++)
    {
        blk = readBlockFromDisk(addr, &buf);
        tuple ts[7];
        getTuplesFromBlock(blk, 7, ts);
        for (int i = 0; i < 6; i++)
        {
            // ���ڵ�һ��Ԫ�����⴦����Ҫ�ٶ���ǰһ���жϺ����һ��Ԫ���Ƿ񹹳ɷ�Χ�����������ǵ�һ�����ҵ�
            if (i == 0 && ts[i].x > pattern)
            {
                if (addr == index_start_addr)
                {
                    start = 217 * 7;
                    end = ts[i].y;
                    break;
                }
                else
                {
                    // ����ǰһ��
                    char* blk_tmp = readBlockFromDisk(addr - 1, &buf);
                    tuple ts_tmp[7];
                    getTuplesFromBlock(blk_tmp, 7, ts_tmp);
                    if (ts_tmp[6].x <= pattern)
                    {
                        start = ts_tmp[6].y;
                        end = ts[i].y;
                        freeBlockInBuffer(blk_tmp, &buf);
                        break;
                    }
                }
            }

            if (ts[i].x <= pattern && ts[i + 1].x > pattern)
            {
                start = ts[i].y;
                end = ts[i + 1].y;
                break;
            }
            
        }
        freeBlockInBuffer(blk, &buf);
    }
    // ͨ���ж�start��end�Ƿ���Ȼ����ж�ͨ�����������Ƿ�ɹ�
    if (start == end)
    {
        printf("Can not find by index!\n");
        return -1;
    }
    // ͨ�������ż������źͿ������
    int start_disk = start / 7;
    int start_idx = start % 7;
    int end_disk = end / 7;
    int end_idx = end % 7;
    // �������õ��Ĵ��̿���Ѱ��
    write_blk = getNewBlockInBuffer(&buf);
    int write_blk_idx = 0;
    int write_addr = write_base_addr;
    for (int addr = start_disk; addr <= end_disk; addr++)
    {
        blk = readBlockFromDisk(addr, &buf);
        tuple ts[7];
        getTuplesFromBlock(blk, 7, ts);
        int idx = (addr == start_disk) ? start_idx : 0;
        int end = (addr == end_disk) ? end_idx : 7;
        for (; idx < end; idx++)
        {
            if (ts[idx].x == pattern)
            {
                // ��ӡ���
                printf("(X=%d, Y=%d)\n", ts[idx].x, ts[idx].y);
                // д���ڴ�飬�ڴ����д����̿�
                char str[5];
                itoa(ts[idx].x, str, 10);
                memcpy(write_blk + write_blk_idx * 8, str, 4);
                itoa(ts[idx].y, str, 10);
                memcpy(write_blk + write_blk_idx * 8 + 4, str, 4);
                write_blk_idx++;
                // write_blk����Ҫд�����
                if (write_blk_idx == 7)
                {
                    itoa(write_addr + 1, write_blk + 7 * 8, 10);
                    if (writeBlockToDisk(write_blk, write_addr, &buf) != 0)
                    {
                        perror("Writing Block Failed!\n");
                        return -1;
                    }
                    // ����д���
                    write_blk = getNewBlockInBuffer(&buf);
                    write_addr++;
                    write_blk_idx = 0;
                }
            }
        }
        freeBlockInBuffer(blk, &buf);
    }
    // ������д��黹��Ԫ����Ҫд�������
    if (write_blk_idx != 0)
    {
        memset(write_blk + write_blk_idx * 8, 0, 64 - write_blk_idx * 8);
        itoa(write_addr + 1, write_blk + 7 * 8, 10);
        if (writeBlockToDisk(write_blk, write_addr, &buf) != 0)
        {
            perror("Writing Block Failed!\n");
            return -1;
        }
        write_addr++;
    }

    int IOTimes = buf.numIO;
    freeBuffer(&buf);
    return IOTimes;
}

/// <summary>
/// ���Ѿ��ź����a��b������ϵ���л��ڵ�һ�����Ե����Ӳ���
/// </summary>
/// <param name="a_start_addr"></param>
/// <param name="a_num"></param>
/// <param name="b_start_addr"></param>
/// <param name="b_num"></param>
/// <returns></returns>
int join(int a_start_addr, int a_num, int b_start_addr, int b_num, int write_base_addr)
{
    Buffer buf;
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    int join_times = 0;
    char *blk_a, *blk_b, *write_blk;
    int write_blk_idx = 0, write_addr = write_base_addr;
    int a_disk_idx = 0, b_disk_idx = 0;
    int a_idx = 0, b_idx = 0;
    write_blk = getNewBlockInBuffer(&buf);
    blk_a = readBlockFromDisk(a_start_addr, &buf);
    blk_b = readBlockFromDisk(b_start_addr, &buf);
    tuple ts_a[7], ts_b[7];
    getTuplesFromBlock(blk_a, 7, ts_a);
    getTuplesFromBlock(blk_b, 7, ts_b);

    int prev_elem = 0;
    bool prev_conn = false, conn = false;    // ��¼ǰһ��Ԫ�غ͵�ǰԪ���Ƿ���������
    // ��RΪ������б�������
    // ѭ����������Ϊab������ϵ�������������еĴ���
    while (1)
    {
        // ���ֿ������ӵ�Ԫ��
        if (ts_a[a_idx].x == ts_b[b_idx].x)
        {
            printf("(%d, %d, %d, %d)\n", ts_b[b_idx].x, ts_b[b_idx].y, ts_a[a_idx].x, ts_a[a_idx].y);
            join_times++;
            conn = true;
            // д���ڴ�飬�ڴ����д����̿�
            char str[5];
            itoa(ts_b[b_idx].x, str, 10);
            memcpy(write_blk + write_blk_idx * 16, str, 4);
            itoa(ts_b[b_idx].y, str, 10);
            memcpy(write_blk + write_blk_idx * 16 + 4, str, 4);
            itoa(ts_a[a_idx].x, str, 10);
            memcpy(write_blk + write_blk_idx * 16 + 8, str, 4);
            itoa(ts_a[a_idx].y, str, 10);
            memcpy(write_blk + write_blk_idx * 16 + 12, str, 4);
            write_blk_idx++;
            // write_blk����Ҫд�����
            if (write_blk_idx == 3)
            {
                memset(write_blk + 48, 0, 16);
                itoa(write_addr + 1, write_blk + 7 * 8, 10);
                if (writeBlockToDisk(write_blk, write_addr, &buf) != 0)
                {
                    perror("Writing Block Failed!\n");
                    return -1;
                }
                // ����д���
                write_blk = getNewBlockInBuffer(&buf);
                write_addr++;
                write_blk_idx = 0;
            }
            b_idx++;
            if (b_idx == 7)
            {
                b_disk_idx++;
                // S�������ˣ�����Ҫ����ң���ʱR�����Ԫ�ض��޷�����
                if (b_disk_idx == b_num)
                    break;
                freeBlockInBuffer(blk_b, &buf);
                blk_b = readBlockFromDisk(b_start_addr + b_disk_idx, &buf);
                getTuplesFromBlock(blk_b, 7, ts_b);
                b_idx = 0;
            }
        }
        // R > S
        else if (ts_a[a_idx].x > ts_b[b_idx].x)
        {
            b_idx++;
            if (b_idx == 7)
            {
                b_disk_idx++;
                // S�������ˣ�����Ҫ����ң���ʱR�����Ԫ�ض��޷�����
                if (b_disk_idx == b_num)
                    break;
                freeBlockInBuffer(blk_b, &buf);
                blk_b = readBlockFromDisk(b_start_addr + b_disk_idx, &buf);
                getTuplesFromBlock(blk_b, 7, ts_b);
                b_idx = 0;
            }
        }
        // R < S
        else
        {
            // ��ǰ��ǰһ��Ԫ����ͬ��ǰһ��Ԫ�ط��������ӣ���Ҫ����S
            if (ts_a[a_idx].x == prev_elem && prev_conn && !conn)
            {
                if (b_idx == 0)
                {
                    // ���ݵ�ͷ��
                    if (b_disk_idx == 0)
                    {
                        break;
                    }
                    else
                    {
                        b_disk_idx--;
                        freeBlockInBuffer(blk_b, &buf);
                        blk_b = readBlockFromDisk(b_start_addr + b_disk_idx, &buf);
                        getTuplesFromBlock(blk_b, 7, ts_b);
                        b_idx = 6;
                    }
                }
                else
                    b_idx--;
                for (; ts_b[b_idx].x == ts_a[a_idx].x; b_idx--)
                {
                    // Ԫ��ĵ�һ��Ԫ�ض���ȣ���Ҫ����ǰһ���������
                    if (b_idx == 0)
                    {
                        // ���ݵ�ͷ��
                        if (b_disk_idx == 0)
                        {
                            break;
                        }
                        else 
                        {
                            b_disk_idx--;
                            freeBlockInBuffer(blk_b, &buf);
                            blk_b = readBlockFromDisk(b_start_addr + b_disk_idx, &buf);
                            getTuplesFromBlock(blk_b, 7, ts_b);
                            b_idx = 6;
                        }
                    }
                }
            }
            else
            {
                prev_elem = ts_a[a_idx].x;
                a_idx++;
                prev_conn = conn;
                conn = false;
                if (a_idx == 7)
                {
                    a_disk_idx++;
                    if (a_disk_idx == a_num)
                        break;
                    freeBlockInBuffer(blk_a, &buf);
                    blk_a = readBlockFromDisk(a_start_addr + a_disk_idx, &buf);
                    getTuplesFromBlock(blk_a, 7, ts_a);
                    a_idx = 0;
                }
            }
        }

    }
    // �ж�write_blk���Ƿ���Ԫ��û��д�����
    // ������д��黹��Ԫ����Ҫд�������
    if (write_blk_idx != 0)
    {
        memset(write_blk + write_blk_idx * 16, 0, 64 - write_blk_idx * 16);
        itoa(write_addr + 1, write_blk + 7 * 8, 10);
        if (writeBlockToDisk(write_blk, write_addr, &buf) != 0)
        {
            perror("Writing Block Failed!\n");
            return -1;
        }
        write_addr++;
    }
    freeBlockInBuffer(blk_a, &buf);
    freeBlockInBuffer(blk_b, &buf);
    freeBuffer(&buf);


    return join_times;
}

/// <summary>
/// ���Ѿ��ź����������ϵ���н�����
/// </summary>
/// <param name="a_start_addr">��ϵa��ʼ���̵�ַ</param>
/// <param name="a_num">��ϵa�Ĵ��̿�����</param>
/// <param name="b_start_addr">��ϵb��ʼ���̵�ַ</param>
/// <param name="b_num">��ϵb���̿�����</param>
/// <param name="write_base_addr">�������Ĵ��̿���ʼ��ַ</param>
/// <returns></returns>
int intersect(int a_start_addr, int a_num, int b_start_addr, int b_num, int write_base_addr)
{
    Buffer buf;
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    int intersect_times = 0;
    char* blk_a, * blk_b, * write_blk;
    int write_blk_idx = 0, write_addr = write_base_addr;
    int a_disk_idx = 0, b_disk_idx = 0;
    int a_idx = 0, b_idx = 0;
    write_blk = getNewBlockInBuffer(&buf);
    blk_a = readBlockFromDisk(a_start_addr, &buf);
    blk_b = readBlockFromDisk(b_start_addr, &buf);
    tuple ts_a[7], ts_b[7];
    getTuplesFromBlock(blk_a, 7, ts_a);
    getTuplesFromBlock(blk_b, 7, ts_b);

    int prev_elem = 0;      // ��¼S��ϵǰһ��Ԫ���x����
    // ��RΪ������б���ѡ����ͬ��Ԫ��
    // ѭ����������Ϊab������ϵ�������������еĴ���
    while (1)
    {
        // x������ͬ����һ���Ƚ�y����
        if (ts_a[a_idx].x == ts_b[b_idx].x)
        {
            // ���ȶ�S���л��ݣ��ҵ���һ������ts_a[a_idx].x == ts_b[b_idx].x��λ��
            if (b_idx == 0)
            {
                // ���ݵ�ͷ�ˣ���ǰ���ǵ�һ��
                if (b_disk_idx == 0);
                    
                else
                {                    
                    b_disk_idx--;
                    freeBlockInBuffer(blk_b, &buf);
                    blk_b = readBlockFromDisk(b_start_addr + b_disk_idx, &buf);
                    getTuplesFromBlock(blk_b, 7, ts_b);
                    b_idx = 6;
                }
            }
            else
                b_idx--;

            // ���ݵ�ͷ�ˣ���ǰ���ǵ�һ��
            if (b_disk_idx == 0 && b_idx == 0);
            else
            {
                for (; ts_b[b_idx].x == ts_a[a_idx].x; b_idx--)
                {
                    // Ԫ��ĵ�һ��Ԫ�ض���ȣ���Ҫ����ǰһ���������
                    if (b_idx == 0)
                    {
                        // ���ݵ�ͷ��
                        if (b_disk_idx == 0)
                        {
                            b_idx--;    // ��idx����Ϊ-1������ͨ��+1���0������©����һ����̵�һ��Ԫ��
                            break;
                        }
                        else
                        {
                            b_disk_idx--;
                            freeBlockInBuffer(blk_b, &buf);
                            blk_b = readBlockFromDisk(b_start_addr + b_disk_idx, &buf);
                            getTuplesFromBlock(blk_b, 7, ts_b);
                            b_idx = 7;          // ����ѭ��������b_idx�����Լ�һ��
                        }
                    }
                }
                b_idx++;

                if (b_idx == 7)
                {
                    b_disk_idx++;
                    // S�������ˣ�����Ҫ����ң���ʱR�����Ԫ�ض��޷�����
                    if (b_disk_idx == b_num)
                        break;
                    freeBlockInBuffer(blk_b, &buf);
                    blk_b = readBlockFromDisk(b_start_addr + b_disk_idx, &buf);
                    getTuplesFromBlock(blk_b, 7, ts_b);
                    b_idx = 0;
                }
            }
            
            // ��������ts_b[b_idx].x == ts_a[a_idx].x��Ѱ����ȵ�Ԫ��
            while(ts_b[b_idx].x == ts_a[a_idx].x)
            {
                // �ҵ����Ԫ��
                if (ts_b[b_idx].y == ts_a[a_idx].y)
                {
                    printf("(%d, %d)\n", ts_b[b_idx].x, ts_b[b_idx].y);
                    intersect_times++;
                    // д���ڴ�飬�ڴ����д����̿�
                    char str[5];
                    itoa(ts_b[b_idx].x, str, 10);
                    memcpy(write_blk + write_blk_idx * 8, str, 4);
                    itoa(ts_b[b_idx].y, str, 10);
                    memcpy(write_blk + write_blk_idx * 8 + 4, str, 4);
                    write_blk_idx++;
                    // write_blk����Ҫд�����
                    if (write_blk_idx == 7)
                    {
                        memset(write_blk + 56, 0, 8);
                        itoa(write_addr + 1, write_blk + 7 * 8, 10);
                        if (writeBlockToDisk(write_blk, write_addr, &buf) != 0)
                        {
                            perror("Writing Block Failed!\n");
                            return -1;
                        }
                        // ����д���
                        write_blk = getNewBlockInBuffer(&buf);
                        write_addr++;
                        write_blk_idx = 0;
                    }
                }
                b_idx++;
                if (b_idx == 7)
                {
                    b_disk_idx++;
                    // S�������ˣ�����Ҫ����ң���ʱR�����Ԫ�ض��޷�����
                    if (b_disk_idx == b_num)
                        break;
                    freeBlockInBuffer(blk_b, &buf);
                    blk_b = readBlockFromDisk(b_start_addr + b_disk_idx, &buf);
                    getTuplesFromBlock(blk_b, 7, ts_b);
                    b_idx = 0;
                }
            }
            prev_elem = ts_a[a_idx].x;    // ��ʱbָ��x���Դ���ts_a[a_idx].x�ĵ�һ��
            a_idx++;
            // �ж�a��b���±��Ƿ�Ϊ7
            if (a_idx == 7)
            {
                a_disk_idx++;
                // S�������ˣ�����Ҫ����ң���ʱR�����Ԫ�ض��޷�����
                if (a_disk_idx == a_num)
                    break;
                freeBlockInBuffer(blk_a, &buf);
                blk_a = readBlockFromDisk(a_start_addr + a_disk_idx, &buf);
                getTuplesFromBlock(blk_a, 7, ts_a);
                a_idx = 0;
            }
            if (b_idx == 7)
            {
                b_disk_idx++;
                // S�������ˣ�����Ҫ����ң���ʱR�����Ԫ�ض��޷�����
                if (b_disk_idx == b_num)
                    break;
                freeBlockInBuffer(blk_b, &buf);
                blk_b = readBlockFromDisk(b_start_addr + b_disk_idx, &buf);
                getTuplesFromBlock(blk_b, 7, ts_b);
                b_idx = 0;
            }
        }
        // R > S
        else if (ts_a[a_idx].x > ts_b[b_idx].x)
        {
            prev_elem = ts_b[b_idx].x;
            b_idx++;
            if (b_idx == 7)
            {
                b_disk_idx++;
                // S�������ˣ�����Ҫ����ң���ʱR�����Ԫ�ض��޷�����
                if (b_disk_idx == b_num)
                    break;
                freeBlockInBuffer(blk_b, &buf);
                blk_b = readBlockFromDisk(b_start_addr + b_disk_idx, &buf);
                getTuplesFromBlock(blk_b, 7, ts_b);
                b_idx = 0;
            }
        }
        // R < S
        else
        {
            // ���a�ĵ�ǰԪ���B��ǰһ��Ԫ���x������ȣ�����Ҫ��b����һ��λ�����ж�
            if(ts_a[a_idx].x==prev_elem)
            {
                if (b_idx == 0)
                {
                    // ���ݵ�ͷ�ˣ���ǰ���ǵ�һ��
                    if (b_disk_idx == 0);

                    else
                    {
                        b_disk_idx--;
                        freeBlockInBuffer(blk_b, &buf);
                        blk_b = readBlockFromDisk(b_start_addr + b_disk_idx, &buf);
                        getTuplesFromBlock(blk_b, 7, ts_b);
                        b_idx = 6;
                    }
                }
                else
                    b_idx--;
            }
            else
            {
                a_idx++;
                if (a_idx == 7)
                {
                    a_disk_idx++;
                    // S�������ˣ�����Ҫ����ң���ʱR�����Ԫ�ض��޷�����
                    if (a_disk_idx == a_num)
                        break;
                    freeBlockInBuffer(blk_a, &buf);
                    blk_a = readBlockFromDisk(a_start_addr + a_disk_idx, &buf);
                    getTuplesFromBlock(blk_a, 7, ts_a);
                    a_idx = 0;
                }
            }
                
        }
    }
    // �ж�write_blk���Ƿ���Ԫ��û��д�����
    // ������д��黹��Ԫ����Ҫд�������
    if (write_blk_idx != 0)
    {
        memset(write_blk + write_blk_idx * 16, 0, 64 - write_blk_idx * 16);
        itoa(write_addr + 1, write_blk + 7 * 8, 10);
        if (writeBlockToDisk(write_blk, write_addr, &buf) != 0)
        {
            perror("Writing Block Failed!\n");
            return -1;
        }
        write_addr++;
    }
    freeBlockInBuffer(blk_a, &buf);
    freeBlockInBuffer(blk_b, &buf);
    freeBuffer(&buf);


    return intersect_times;
}



/// <summary>
/// ���ڴ��blk�ж�ȡ���е�num��Ԫ�鵽tuple��
/// </summary>
/// <param name="blk">�ڴ���ַ</param>
/// <param name="ts">Ԫ������ָ��</param>
/// <returns></returns>
void getTuplesFromBlock(char* blk, int num, tuple* ts)
{
    for (int j = 0; j < num; j++)
    {
        char str[5];
        memcpy(str, blk + j * 8, 4);
        ts[j].x = atoi(str);
        memcpy(str, blk + j * 8 + 4, 4);
        ts[j].y = atoi(str);
    }
    return;
}

// ���num���±�idx�Ƿ�ָ�����һ����
bool checkIdx(int idx[], int num)
{
    for (int i = 0; i < num; i++)
    {
        if (idx[i] < 7)
            return false;
    }
    return true;
}


/// <summary>
/// �Ӵ��̿���ɵ������ж�ȡ�±�Ϊindex��Ӧ��Ԫ�飬��ȥÿ�����̿�������һ�����̿��ֵַ
/// </summary>
/// <param name="blks"></param>
/// <param name="index"></param>
/// <param name="length"></param>
/// <returns></returns>
tuple readTuple(char* blks[], int index, int length)
{
    int blk_num = index / 7;
    int blk_index = index % 7;
    char str[5];
    tuple t;
    memcpy(str, blks + blk_num * 64 + blk_index, 4);
    t.x = atoi(str);
    memcpy(str, blks + blk_num * 64 + blk_index + 4, 4);
    t.y = atoi(str);
    
    return t;
}

/// <summary>
/// ��Ԫ��tд��
/// </summary>
/// <param name="blks"></param>
/// <param name="index"></param>
/// <param name="t"></param>
void writeTuple(char* blks[], int index, tuple t)
{

}
