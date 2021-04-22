/*
*   name : qzylalala
*/
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <errno.h>
#include "cachelab.h"

#define CACHE_SIZE 32

extern char *optarg;

int hit_count = 0, miss_count = 0, eviction_count = 0; // 初始化输出值
int s, E, b; // 组索引位数  每组包含的缓存行数  内存内地址位数
char* trace_filename;   // 内存访问轨迹文件名
int set_mask = 0, block_mask = 0, set_bits;
unsigned int tag_mask;;

struct cache_line {
    unsigned long tag;
    int valid;
    long lru;
}cache[CACHE_SIZE][CACHE_SIZE];

void set_masks();
void visit_cache(unsigned long long addr);

int main(int argc, char* argv[]) {
      // 参数处理
    int c = 0;
    while(EOF != (c = getopt(argc, argv, "hvs:E:b:t:"))) {
        switch (c) {
            case 'h':   // 显示帮助信息 (可选)
                break;
            case 'v':   // 显示轨迹信息 (可选)
                break;
            case 's':   // 组索引位数
                s = atoi(optarg);
                break;
            case 'E':   // 关联度(每组包含的缓存行数)
                E = atoi(optarg);
                break;
            case 'b':   // 内存块内地址位数
                b = atoi(optarg);
                break;
            case 't':   // 内存访问轨迹文件名
                trace_filename = optarg;
                break;
            default:
                printf("wrong argument input : %s", optarg);    
                break;
        }
    }

    // 初始化 cache
	for(int i = 0; i < CACHE_SIZE; i++) {
		for(int j = 0; j < CACHE_SIZE; j++) {
			cache[i][j].valid = 0;
			cache[i][j].tag = 0;
			cache[i][j].lru = 0;
        }
    }

    set_masks();

    // trace 文件输入 初始化处理
    FILE *fp = fopen(trace_filename, "r" );
    if (fp) {
        char op;    // 操作符
        unsigned long long addr;  // 64-bit十六进制内存地址
        int size;   // 访问的内存字节数量
        while((fscanf(fp," %c %llx,%d\n", &op, &addr, &size))!=EOF) {
            // op : I - 指令装载,  L - 数据装载, S - 数据存储, M - 数据修改
            if (op == 'I') continue; // 指令相关不考虑
            else if (op == 'L') {
                visit_cache(addr);
            }
            else if (op == 'S') {
                visit_cache(addr);
            } 
            else if (op == 'M') {
                visit_cache(addr);
                visit_cache(addr);
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


void set_masks() {
	int maskbit=1, maskbit1=1;
    int tmp_s = s, tmp_b = b;
	while(tmp_s != 0) {
		set_mask=set_mask+maskbit;
		maskbit=maskbit<<1;
		tmp_s = tmp_s-1;
	}
	while(tmp_b != 0) {
		block_mask=block_mask+maskbit1;
		maskbit1=maskbit1<<1;
		tmp_b = tmp_b-1;
	}

	tag_mask=~(set_mask+block_mask);
	set_mask = set_mask << b;
}

void visit_cache(unsigned long long addr) {
	int boolean = 0, save;
	long tag, max = 0, min = 1 << 30;
	tag = addr & tag_mask;
	set_bits = addr & set_mask;                   
	set_bits = set_bits >> b;
	tag = tag >> (s + b);			       
	
    // 遍历每一行
	for(int i = 0; i < E; i++) {
        // hit
		if((cache[set_bits][i].tag == tag) && (cache[set_bits][i].valid == 1)) {	
			hit_count++;	
			cache[set_bits][i].lru = max + 1;
			max = max + 1;
			boolean = 1;
			break;
		}
        // miss
		else if(cache[set_bits][i].valid == 0)  {
			miss_count ++;
			cache[set_bits][i].valid = 1;
			cache[set_bits][i].tag = tag;
			cache[set_bits][i].lru = max + 1;
			max ++;
			boolean = 1;
			break;
		}
		else {   //else find the element with lease LRU value and save it
			if(cache[set_bits][i].lru < min) {
				min = cache[set_bits][i].lru;
				save = i;
			}
		}
    }
	// Evict (miss)
	if (boolean == 0) {
		miss_count ++;
		eviction_count ++;
		cache[set_bits][save].tag = tag;
		cache[set_bits][save].lru = max + 1;
		max ++;
	}
}