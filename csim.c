#include <stdio.h>
#include <unistd.h>
#include "cachelab.h"

extern char *optarg;

int s, E, b;
char* trace_filename;

int main(int argc, char* argv[]) {
    // 初始化输出值
    int hit_count = 0, miss_count = 0, eviction_count = 0;
    
      // 参数处理
    int c = 0;
    while(EOF != (c = getopt(argc, argv, "hvs:E:b:t:"))) {
        switch (c) {
            case 'h':   // 显示帮助信息 (可选)
                break;
            case 'v':   // 显示轨迹信息 (可选)
                break;
            case 's':   // 组索引位数
                s = stoi(optarg);
                break;
            case 'E':   // 关联度(每组包含的缓存行数)
                E = stoi(optarg);
                break;
            case 'b':   // 内存块内地址位数
                b = stoi(optarg);
                break;
            case 't':   // 内存访问轨迹文件名
                trace_filename = optarg;
                break;
            default:
                printf("wrong argument input : %s", optarg);    
                break;
        }
    }

    // trace 文件输入 初始化处理
    FILE *fp = fopen (trace_filename, "r" );
    if (fp) {
        char op;    // 操作符
        long addr;  // 64-bit十六进制内存地址
        int size;   // 访问的内存字节数量
        while((fscanf(fp," %c %lx,%d\n", &op, &addr, &size))!=EOF) {
            // op : I - 指令装载,  L - 数据装载, S - 数据存储, M - 数据修改
            if (op == 'I') continue; // 指令相关不考虑
            else if (op == 'L') {

            }
            else if (op == 'S') {

            } 
            else if (op == 'M') {

            }
            else {
                printf("ERROR: trace file has wrong input line, please check!\n");
            }
        }

        fclose(fp);
    }
    else {
        printf("ERROR: cannt open file correctly, please check!\n");
    }


    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
