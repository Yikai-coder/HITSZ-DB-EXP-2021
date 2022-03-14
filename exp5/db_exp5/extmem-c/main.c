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
    /*格式化输出*/
    printf("==================================\n");
    printf("基于线性搜索的选择算法S_C=130\n");
    printf("==================================\n");
    LinearSelect();
    return;
}

void task2(void)
{
    printf("==================================\n");
    printf("两趟扫描排序算法\n");
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
    printf("IO次数为%d\n", IOTimes);
    return;
}

void task3(void)
{
    printf("==================================\n");
    printf("基于索引的搜索算法\n");
    printf("==================================\n");
    //LinearSelect();
    printf("==================================\n");
    printf("建立索引\n");
    printf("==================================\n");
    int index_num = makeIndex(217, 248, 301);
    printf("==================================\n");
    printf("基于索引进行搜索\n");
    printf("==================================\n");

    int IOTimes = searchByIndex(301, index_num, 130, 350);
    printf("IOTimes: %d\n", IOTimes);

    return;
}

void task4(void)
{
    printf("==================================\n");
    printf("基于排序的连接算法\n");
    printf("==================================\n");
    int joinTimes = join(201, 16, 217, 32, 401);
    printf("连接操作次数:%d\n", joinTimes);
}

void task5(void)
{
    printf("==================================\n");
    printf("基于排序的关系交算法\n");
    printf("==================================\n");
    int intersectTimes = intersect(201, 16, 217, 32, 601);
    printf("交集元组数量:%d\n", intersectTimes);
}

/// <summary>
/// 基于线性搜索的关系选择算法，选择S.C=130的元组
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
    int read_blk_addr = 17;     // S关系第一个块地址为17
    int write_blk_addr = 100;   // 结果存放的块初始地址
    int count = 0;              // 统计找到的数量
    int write_blk_count = 0;    // 统计已写入的块数

    while(1)
    {
        // 读取S关系一个块
        if ((read_blk = readBlockFromDisk(read_blk_addr, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        // 输出
        printf("读取磁盘块%d\n", read_blk_addr);

        // 依次读取7个元组
        for (int i = 0; i < 7; i++)
        {
            char str[5];
            memcpy(str, read_blk + i * 8, 4);
            int S_C = atoi(str);
            memcpy(str, read_blk + i * 8 + 4, 4);
            int S_D = atoi(str);
            if (S_C == 130)
            {
                // 输出
                printf("(%d, %d)\n", S_C, S_D);
                // 写入写入块，并判断是否存满，需要写入磁盘
                memcpy(write_blk + count * 8, read_blk + i * 8, 8);
                count++;
                if (count==7)
                {
                    // 在块最后写入下一块的地址
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
        // 读取最后一个int获取下一个块地址
        char str[5];
        memcpy(str, read_blk + 7 * 8, 4);
        int addr = atoi(str);
        if (addr < 17 || addr > 48)
            break;
        freeBlockInBuffer(read_blk, &buf);
        read_blk_addr = addr;
    }

    // 如果最后写入块内还有数据，需要将结果写入磁盘
    if (count != 0)
    {
        if (writeBlockToDisk(write_blk, write_blk_addr + write_blk_count, &buf) != 0)
        {
            perror("Writing Block Failed!\n");
            return -1;
        }
        write_blk_count++;
    }
    // 输出数据存放位置
    printf("数据存放在:");
    for (int i = 0; i < write_blk_count; i++)
    {
        printf("%d", write_blk_addr + i);
    }
    printf("\n");
    // 输出元组个数和IO次数
    printf("搜索得到的元组个数:%d\n", (write_blk_count-1) * 7 + count);
    printf("IO次数:%d\n", buf.numIO);
    freeBuffer(&buf);
    return 0;
}

/// <summary>
/// 两阶段多路并归排序，首先将关系划分为小的元组进行排序，然后再对有序的子集进行归并排序
/// </summary>
/// <param name="start_blk_addr">开始磁盘地址</param>
/// <param name="end_blk_addr">结束磁盘地址</param>
/// <param name="write_base_addr">写入磁盘基地址</param>
/// <returns>IO读写次数</returns>
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

    // 每次读入6块进行排序，直到7块全部排序完
    int rd_blk_ptr = start_blk_addr;

    unsigned char* sort_blk[6]; // 存储待排序数据块
    unsigned char* write_blk;   // 存储待写入数据块
    for (int i = 0; i < 6; i++)
        sort_blk[i] = NULL;
    write_blk = NULL;
    int write_blk_idx = 0;      // 记录带写入块的写入地址下标，为7时需要写入磁盘
    while (1)
    {
        // 设置写入块
        write_blk = getNewBlockInBuffer(&buf);
        // 计算本次排序的块数量num
        int num = (end_blk_addr - rd_blk_ptr + 1 < 7) ? end_blk_addr - rd_blk_ptr + 1: 6;
        // 读入num个块
        for (int i = 0; i < num; i++)
        {
            /* Read the block from the hard disk */
            if ((sort_blk[i] = readBlockFromDisk(rd_blk_ptr+i, &buf)) == NULL)
            {
                perror("Reading Block Failed!\n");
                return -1;
            }
        }
        // 首先在每个块内排序
        for (int i = 0; i < num; i++)
        {
            // 将块内数据读取到tuple数组当中
            tuple ts[7];
            for (int j = 0; j < 7; j++)
            {
                char str[5];
                memcpy(str, sort_blk[i] + j*8, 4);
                ts[j].x = atoi(str);
                memcpy(str, sort_blk[i] + j * 8 + 4, 4);
                ts[j].y = atoi(str);
            }
            // 采用选择排序进行块内排序
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
                // 将idx对应的元组换到位置j上
                tuple tmp = ts[j];
                ts[j] = ts[min_idx];
                ts[min_idx] = tmp;
            }
            // 将排序后的元组写回块内
            for (int j = 0; j < 7; j++)
            {
                char str[5];
                itoa(ts[j].x, str, 10);
                memcpy(sort_blk[i] + j * 8, str, 4);
                itoa(ts[j].y, str, 10);
                memcpy(sort_blk[i] + j * 8 + 4, str, 4);
            }
        }
        // 采用归并排序将排序好的6个块写入write_blk,若write_blk满则写入磁盘
        int idx[6];        // num个下标指向num个待排序块
        for (int i = 0; i < num; i++)
            idx[i] = 0;
        while (!checkIdx(idx, num))
        {
            // 将每个块内第一个元素读出来到tuple中
            tuple element[6];
            for (int i = 0; i < num; i++)
            {
                // 尚未排序完块内所有元素
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
            // 找出num个元素中最小的一个写入write_blk
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
            // 写入write_blk
            char str[5];
            itoa(element[min_idx].x, str, 10);
            memcpy(write_blk + write_blk_idx * 8, str, 4);
            itoa(element[min_idx].y, str, 10);
            memcpy(write_blk + write_blk_idx * 8 + 4, str, 4);
            write_blk_idx++;
            // write_blk满需要写入磁盘
            if (write_blk_idx == 7)
            {
                itoa(write_addr + 1, write_blk + 7 * 8, 10);
                if (writeBlockToDisk(write_blk, write_addr, &buf) != 0)
                {
                    perror("Writing Block Failed!\n");
                    return -1;
                }
                // 设置写入块
                write_blk = getNewBlockInBuffer(&buf);
                write_addr++;
                write_blk_idx = 0;
            }
            idx[min_idx] = (idx[min_idx] == 7) ? 7 : idx[min_idx]+1;
        }
        rd_blk_ptr += num;
        // 释放num个块的内存
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

    // 对已经分好组的块再进行归并排序
    // 首先计算分了多少组，最后一组多少块
    int group_num = (end_blk_addr - start_blk_addr + 1) / 6;    // 组数量
    int last_group_num;                                         // 最后一组内块数量
    if ((end_blk_addr - start_blk_addr + 1) % 6 == 0)
    {
        last_group_num = 0;
    }
    else
    {
        last_group_num = (end_blk_addr - start_blk_addr + 1) % 6;
        group_num++;
    }

    // 不考虑组数>排序块数量(7)的情况
    int new_idx[7];    // 块内指针,指向块内元组，范围是0<=new_idx<7
    int block_idx[6];  // 组内指针,指向组内块，范围是0<=block_idx<6 or last_group_num
    for (int i = 0; i < group_num; i++)
    {
        new_idx[i] = 0;
        block_idx[i] = 0;
    }
    // 读入所有组的第一块
    for (int i = 0; i < group_num; i++)
    {
        if ((sort_blk[i] = readBlockFromDisk(write_base_addr+i*6, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
    }
    // 设置写入块
    write_blk = getNewBlockInBuffer(&buf);
    // 开始归并排序
    while (1)
    {
        tuple element[7];
        // 读取每个排序块对应的下标元组
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
        // 找出最小元素
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
        // 将最小元素写入write_blk
        // 写入write_blk
        char str[5];
        itoa(element[min_idx].x, str, 10);
        memcpy(write_blk + write_blk_idx * 8, str, 4);
        itoa(element[min_idx].y, str, 10);
        memcpy(write_blk + write_blk_idx * 8 + 4, str, 4);
        write_blk_idx++;
        // 写入块满时需要写入磁盘
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

        // 读入下一个元组
        // 如果读取到块内的最后一个元组了
        if (new_idx[min_idx] == 6)
        {
            // 判断是否读取到组内最后一块
            // 根据组号判断是否是最后一块，从而知道组内块的数量
            if (min_idx == group_num - 1)
            {
                // 是最后一块
                if (block_idx[min_idx] == last_group_num-1)
                    new_idx[min_idx] = 7;
                else
                {
                    block_idx[min_idx]++;
                    new_idx[min_idx] = 0;
                    // 先释放当前块，再获取新的块
                    freeBlockInBuffer(sort_blk[min_idx], &buf);
                    sort_blk[min_idx] = readBlockFromDisk(write_base_addr + min_idx * 6 + block_idx[min_idx], &buf);
                }
            }
            // 不是最后一组，组内有6块
            else
            {
                // 是最后一块
                if (block_idx[min_idx] == 5)
                    new_idx[min_idx] = 7;
                else
                {
                    block_idx[min_idx]++;
                    new_idx[min_idx] = 0;
                    // 先释放当前块，再获取新的块
                    freeBlockInBuffer(sort_blk[min_idx], &buf);
                    sort_blk[min_idx] = readBlockFromDisk(write_base_addr + min_idx * 6 + block_idx[min_idx], &buf);
                }
            }

        }
        else
            new_idx[min_idx]++;

        // 检查归并是否完成
        if (checkIdx(new_idx, group_num))
            break;
    }
     //释放块占用
    for (int i = 0; i < group_num; i++)
        freeBlockInBuffer(sort_blk[i], &buf);
    freeBlockInBuffer(write_blk, &buf);

    // 清理中间磁盘占用，将排序完成的结果转移到write_base_addr开始
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
/// 为已经排好序的元组建立索引，这里假设索引元组的第一个属性
/// 采用为每个文件建立一个索引的方式，索引值为磁盘号*7+磁盘内块号
/// </summary>
/// <param name="start_addr">元组起始磁盘地址</param>
/// <param name="end_addr">元组终止磁盘地址</param>
/// <param name="index_start_addr">索引存放起始地址</param>
/// <returns>索引块数量，错误返回-1</returns>
int makeIndex(int start_addr, int end_addr, int index_start_addr)
{
    Buffer buf;
    char* blk;
    char* write_blk;
    int write_addr = index_start_addr;
    int write_blk_idx = 0;     // 写入块下标，为7时写入磁盘
    /* Initialize the buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    write_blk = getNewBlockInBuffer(&buf);
    // 遍历所有块建立索引
    for (int addr = start_addr; addr <= end_addr; addr++)
    {
        blk = readBlockFromDisk(addr, &buf);
        // 将块内数据读取到tuple数组当中
        tuple ts[7];
        getTuplesFromBlock(blk, 7, ts);
        int prev = ts[0].x;
        // 遍历磁盘块内的元素寻找待索引元组
        for (int i = 1; i < 7; i++)
        {
            int cur = ts[i].x;
            // 在磁盘块中寻找两个相邻的不同的元素，确保索引指向相等元组的第一个
            if (cur != prev)
            {
                int idx_num = addr * 7 + i;
                // 写入内存块，内存块满写入磁盘块
                char str[5];
                itoa(ts[i].x, str, 10);
                memcpy(write_blk + write_blk_idx * 8, str, 4);
                itoa(idx_num, str, 10);
                memcpy(write_blk + write_blk_idx * 8 + 4, str, 4);
                write_blk_idx++;
                // write_blk满需要写入磁盘
                if (write_blk_idx == 7)
                {
                    itoa(write_addr + 1, write_blk + 7 * 8, 10);
                    if (writeBlockToDisk(write_blk, write_addr, &buf) != 0)
                    {
                        perror("Writing Block Failed!\n");
                        return -1;
                    }
                    // 设置写入块
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
    // 最后如果写入块还有元组需要写入磁盘中
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
/// 通过索引查找元组
/// </summary>
/// <param name="index_start_addr">索引块起始地址</param>
/// <param name="index_num">索引块数量</param>
/// <param name="pattern">待查找元素值</param>
/// <param name="write_base_addr">结果存放的磁盘起始地址</param>
/// <returns>IO次数</returns>
int searchByIndex(int index_start_addr, int index_num, int pattern, int write_base_addr)
{
    Buffer buf;
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    char* blk, * write_blk;
    int start = 0, end = 0;     // 通过索引查找到的目标范围
    // 遍历存放索引块的磁盘寻找范围
    for (int addr = index_start_addr; addr < index_start_addr+index_num; addr++)
    {
        blk = readBlockFromDisk(addr, &buf);
        tuple ts[7];
        getTuplesFromBlock(blk, 7, ts);
        for (int i = 0; i < 6; i++)
        {
            // 块内第一个元组特殊处理，需要再读入前一块判断和最后一个元组是否构成范围，如果本身就是第一块则找到
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
                    // 读入前一块
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
    // 通过判断start和end是否依然相等判断通过索引查找是否成功
    if (start == end)
    {
        printf("Can not find by index!\n");
        return -1;
    }
    // 通过索引号计算出块号和块内序号
    int start_disk = start / 7;
    int start_idx = start % 7;
    int end_disk = end / 7;
    int end_idx = end % 7;
    // 到索引得到的磁盘块中寻找
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
                // 打印输出
                printf("(X=%d, Y=%d)\n", ts[idx].x, ts[idx].y);
                // 写入内存块，内存块满写入磁盘块
                char str[5];
                itoa(ts[idx].x, str, 10);
                memcpy(write_blk + write_blk_idx * 8, str, 4);
                itoa(ts[idx].y, str, 10);
                memcpy(write_blk + write_blk_idx * 8 + 4, str, 4);
                write_blk_idx++;
                // write_blk满需要写入磁盘
                if (write_blk_idx == 7)
                {
                    itoa(write_addr + 1, write_blk + 7 * 8, 10);
                    if (writeBlockToDisk(write_blk, write_addr, &buf) != 0)
                    {
                        perror("Writing Block Failed!\n");
                        return -1;
                    }
                    // 设置写入块
                    write_blk = getNewBlockInBuffer(&buf);
                    write_addr++;
                    write_blk_idx = 0;
                }
            }
        }
        freeBlockInBuffer(blk, &buf);
    }
    // 最后如果写入块还有元组需要写入磁盘中
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
/// 对已经排好序的a和b两个关系进行基于第一个属性的连接操作
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
    bool prev_conn = false, conn = false;    // 记录前一个元素和当前元素是否发生了连接
    // 以R为主体进行遍历连接
    // 循环结束条件为ab两个关系都遍历完了所有的磁盘
    while (1)
    {
        // 发现可以连接的元组
        if (ts_a[a_idx].x == ts_b[b_idx].x)
        {
            printf("(%d, %d, %d, %d)\n", ts_b[b_idx].x, ts_b[b_idx].y, ts_a[a_idx].x, ts_a[a_idx].y);
            join_times++;
            conn = true;
            // 写入内存块，内存块满写入磁盘块
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
            // write_blk满需要写入磁盘
            if (write_blk_idx == 3)
            {
                memset(write_blk + 48, 0, 16);
                itoa(write_addr + 1, write_blk + 7 * 8, 10);
                if (writeBlockToDisk(write_blk, write_addr, &buf) != 0)
                {
                    perror("Writing Block Failed!\n");
                    return -1;
                }
                // 设置写入块
                write_blk = getNewBlockInBuffer(&buf);
                write_addr++;
                write_blk_idx = 0;
            }
            b_idx++;
            if (b_idx == 7)
            {
                b_disk_idx++;
                // S遍历完了，还需要向后找，此时R后面的元素都无法连接
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
                // S遍历完了，还需要向后找，此时R后面的元素都无法连接
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
            // 当前和前一个元素相同且前一个元素发生了连接，需要回溯S
            if (ts_a[a_idx].x == prev_elem && prev_conn && !conn)
            {
                if (b_idx == 0)
                {
                    // 回溯到头了
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
                    // 元组的第一个元素都相等，需要读入前一块继续回溯
                    if (b_idx == 0)
                    {
                        // 回溯到头了
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
    // 判断write_blk中是否还有元素没有写入磁盘
    // 最后如果写入块还有元组需要写入磁盘中
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
/// 将已经排好序的两个关系进行交操作
/// </summary>
/// <param name="a_start_addr">关系a起始磁盘地址</param>
/// <param name="a_num">关系a的磁盘块数量</param>
/// <param name="b_start_addr">关系b起始磁盘地址</param>
/// <param name="b_num">关系b磁盘块数量</param>
/// <param name="write_base_addr">结果保存的磁盘块起始地址</param>
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

    int prev_elem = 0;      // 记录S关系前一个元组的x属性
    // 以R为主体进行遍历选择相同的元组
    // 循环结束条件为ab两个关系都遍历完了所有的磁盘
    while (1)
    {
        // x属性相同，进一步比较y属性
        if (ts_a[a_idx].x == ts_b[b_idx].x)
        {
            // 首先对S进行回溯，找到第一个满足ts_a[a_idx].x == ts_b[b_idx].x的位置
            if (b_idx == 0)
            {
                // 回溯到头了，当前就是第一个
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

            // 回溯到头了，当前就是第一个
            if (b_disk_idx == 0 && b_idx == 0);
            else
            {
                for (; ts_b[b_idx].x == ts_a[a_idx].x; b_idx--)
                {
                    // 元组的第一个元素都相等，需要读入前一块继续回溯
                    if (b_idx == 0)
                    {
                        // 回溯到头了
                        if (b_disk_idx == 0)
                        {
                            b_idx--;    // 将idx设置为-1，后面通过+1变成0，避免漏掉第一块磁盘第一个元组
                            break;
                        }
                        else
                        {
                            b_disk_idx--;
                            freeBlockInBuffer(blk_b, &buf);
                            blk_b = readBlockFromDisk(b_start_addr + b_disk_idx, &buf);
                            getTuplesFromBlock(blk_b, 7, ts_b);
                            b_idx = 7;          // 本次循环结束，b_idx还会自减一次
                        }
                    }
                }
                b_idx++;

                if (b_idx == 7)
                {
                    b_disk_idx++;
                    // S遍历完了，还需要向后找，此时R后面的元素都无法连接
                    if (b_disk_idx == b_num)
                        break;
                    freeBlockInBuffer(blk_b, &buf);
                    blk_b = readBlockFromDisk(b_start_addr + b_disk_idx, &buf);
                    getTuplesFromBlock(blk_b, 7, ts_b);
                    b_idx = 0;
                }
            }
            
            // 遍历所有ts_b[b_idx].x == ts_a[a_idx].x，寻找相等的元组
            while(ts_b[b_idx].x == ts_a[a_idx].x)
            {
                // 找到相等元组
                if (ts_b[b_idx].y == ts_a[a_idx].y)
                {
                    printf("(%d, %d)\n", ts_b[b_idx].x, ts_b[b_idx].y);
                    intersect_times++;
                    // 写入内存块，内存块满写入磁盘块
                    char str[5];
                    itoa(ts_b[b_idx].x, str, 10);
                    memcpy(write_blk + write_blk_idx * 8, str, 4);
                    itoa(ts_b[b_idx].y, str, 10);
                    memcpy(write_blk + write_blk_idx * 8 + 4, str, 4);
                    write_blk_idx++;
                    // write_blk满需要写入磁盘
                    if (write_blk_idx == 7)
                    {
                        memset(write_blk + 56, 0, 8);
                        itoa(write_addr + 1, write_blk + 7 * 8, 10);
                        if (writeBlockToDisk(write_blk, write_addr, &buf) != 0)
                        {
                            perror("Writing Block Failed!\n");
                            return -1;
                        }
                        // 设置写入块
                        write_blk = getNewBlockInBuffer(&buf);
                        write_addr++;
                        write_blk_idx = 0;
                    }
                }
                b_idx++;
                if (b_idx == 7)
                {
                    b_disk_idx++;
                    // S遍历完了，还需要向后找，此时R后面的元素都无法连接
                    if (b_disk_idx == b_num)
                        break;
                    freeBlockInBuffer(blk_b, &buf);
                    blk_b = readBlockFromDisk(b_start_addr + b_disk_idx, &buf);
                    getTuplesFromBlock(blk_b, 7, ts_b);
                    b_idx = 0;
                }
            }
            prev_elem = ts_a[a_idx].x;    // 此时b指向x属性大于ts_a[a_idx].x的第一块
            a_idx++;
            // 判断a和b的下标是否为7
            if (a_idx == 7)
            {
                a_disk_idx++;
                // S遍历完了，还需要向后找，此时R后面的元素都无法连接
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
                // S遍历完了，还需要向后找，此时R后面的元素都无法连接
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
                // S遍历完了，还需要向后找，此时R后面的元素都无法连接
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
            // 如果a的当前元组和B的前一个元组的x属性相等，则需要将b回溯一个位置再判断
            if(ts_a[a_idx].x==prev_elem)
            {
                if (b_idx == 0)
                {
                    // 回溯到头了，当前就是第一个
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
                    // S遍历完了，还需要向后找，此时R后面的元素都无法连接
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
    // 判断write_blk中是否还有元素没有写入磁盘
    // 最后如果写入块还有元组需要写入磁盘中
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
/// 从内存块blk中读取其中的num个元组到tuple中
/// </summary>
/// <param name="blk">内存块地址</param>
/// <param name="ts">元组数组指针</param>
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

// 检查num个下标idx是否都指向最后一个数
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
/// 从磁盘块组成的数组中读取下表为index对应的元组，略去每个磁盘块最后的下一个磁盘块地址值
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
/// 将元组t写入
/// </summary>
/// <param name="blks"></param>
/// <param name="index"></param>
/// <param name="t"></param>
void writeTuple(char* blks[], int index, tuple t)
{

}
