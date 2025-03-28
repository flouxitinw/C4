#include "cainiao.h"
#include <string.h>
#include <time.h>
#define _CRT_SECURE_NO_WARNINGS

// 创建货架
Shelf* create_shelf(const char* shelf_id, int max_capacity) {
    Shelf* new_shelf = (Shelf*)malloc(sizeof(Shelf));
    if (!new_shelf) {
        perror("Failed to create shelf");
        return NULL;
    }
    strncpy_s(new_shelf->shelf_id, sizeof(new_shelf->shelf_id), shelf_id, _TRUNCATE);
    new_shelf->shelf_id[sizeof(new_shelf->shelf_id) - 1] = '\0';
    new_shelf->max_capacity = max_capacity;
    new_shelf->current_count = 0;
    new_shelf->express_list = NULL;
    new_shelf->next = NULL;
    return new_shelf;
}

// 增加货架（头插法）
void add_shelf(Shelf** head, Shelf* new_shelf) {
    if (!new_shelf) return;
    new_shelf->next = *head;
    *head = new_shelf;
}

// 找到最合适货架（找容量最小的货架入库）
Shelf* find_best_shelf(Shelf* shelves, Express* express) {
    Shelf* curr = shelves;
    Shelf* best = NULL;
    while (curr) {
        if (curr->current_count < curr->max_capacity) {
            if (!best || curr->current_count < best->current_count) {
                best = curr;
            }
        }
        curr = curr->next;
    }
    return best;
}

// 分配快递到货架
void assign_express_to_shelf(Shelf** shelves, Express* express) {
    if (!express) return;

    Shelf* target = find_best_shelf(*shelves, express);
    if (!target) {
        printf("所有货架已满！无法存放快递。\n");
        return;
    }

    // 生成取件码
    snprintf(express->pickup_code, sizeof(express->pickup_code), "%s-%03d",
        target->shelf_id, target->current_count + 1);

    // 记录快递到货架
    ExpressNode* new_node = (ExpressNode*)malloc(sizeof(ExpressNode));
    if (!new_node) {
        perror("Failed to allocate express node");
        return;
    }
    memcpy(&new_node->data, express, sizeof(Express));
    new_node->next = target->express_list;
    target->express_list = new_node;

    target->current_count++;
    strncpy_s(express->shelf_id, sizeof(express->shelf_id), target->shelf_id, _TRUNCATE);  // 使用 strncpy_s 复制字符串
}

// 保存货架信息到文件
void save_shelves_to_file(Shelf* head, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to save shelves");
        return;
    }

    fprintf(fp, "shelf_id,max_capacity,current_count,express_list\n");

    Shelf* curr_shelf = head;
    while (curr_shelf) {
        fprintf(fp, "%s,%d,%d,",
            curr_shelf->shelf_id,
            curr_shelf->max_capacity,
            curr_shelf->current_count);

        ExpressNode* curr_express = curr_shelf->express_list;
        while (curr_express) {
            fprintf(fp, "%s;", curr_express->data.tracking_num);
            curr_express = curr_express->next;
        }
        fprintf(fp, "\n");
        curr_shelf = curr_shelf->next;
    }
    fclose(fp);
}

// 从文件中获取货架信息
Shelf* load_shelves_from_file(const char* filename, Express* express_list) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return NULL;

    Shelf* head = NULL, * tail = NULL;
    char line[256];
    fgets(line, sizeof(line), fp); // 跳过表头

    while (fgets(line, sizeof(line), fp)) {
        char shelf_id[10], express_items[256];
        int max_cap, curr_cnt;

        if (sscanf_s(line, "%9[^,],%d,%d,%255[^\n]",
            shelf_id, (unsigned)_countof(shelf_id),
            &max_cap, &curr_cnt,
            express_items, (unsigned)_countof(express_items)) < 3) {
            continue;
        }

        Shelf* shelf = create_shelf(shelf_id, max_cap);
        if (!shelf) continue;

        shelf->current_count = curr_cnt;

        // 关联快递
        char* next_token = NULL;
        char* token = strtok_s(express_items, ";", &next_token);
        while (token) {
            Express* exp = find_express_by_tracking(express_list, token);
            if (exp) {
                ExpressNode* node = (ExpressNode*)malloc(sizeof(ExpressNode));
                if (node) {
                    node->data = *exp;
                    node->next = shelf->express_list;
                    shelf->express_list = node;
                    strncpy_s(exp->shelf_id, sizeof(exp->shelf_id), shelf->shelf_id, _TRUNCATE);
                }
            }
            token = strtok_s(NULL, ";", &next_token);
        }

        if (!head) head = tail = shelf;
        else {
            tail->next = shelf;
            tail = shelf;
        }
    }
    fclose(fp);
    return head;
}

// 获取状态文本
const char* get_status_text(int status) {
    static const char* texts[] = {
        "待取件", "已取件", "滞留件", "异常件"
    };
    return (status >= 0 && status < 4) ? texts[status] : "未知状态";
}

// 盘点单个货架
void inventory_shelf(Shelf* shelf) {
    if (!shelf) return;

    printf("\n====== 货架 %s 盘点 ======\n", shelf->shelf_id);
    printf("容量: %d/%d\n", shelf->current_count, shelf->max_capacity);

    int actual_count = 0;
    ExpressNode* curr = shelf->express_list;

    while (curr) {
        Express* exp = &(curr->data);
        printf("[%s] %s | 状态: %s\n",
            exp->pickup_code,
            exp->tracking_num,
            get_status_text(exp->status));

        if (exp->status == 2) { // EXPRESS_OVERDUE
            printf(" !!! 滞留件需处理\n");
        }
        actual_count++;
        curr = curr->next;
    }

    if (actual_count != shelf->current_count) {
        printf("\n警告：系统记录%d件，实际扫描%d件\n",
            shelf->current_count, actual_count);
    }
    printf("----------------------------\n");
}

// 全站盘点
void full_inventory(Shelf* head) {
    int total_system = 0, total_actual = 0;

    printf("\n======== 全站库存盘点 ========\n");
    while (head) {
        inventory_shelf(head);
        total_system += head->current_count;

        ExpressNode* curr = head->express_list;
        while (curr) {
            total_actual++;
            curr = curr->next;
        }

        head = head->next;
    }

    printf("\n【总计】系统记录: %d件 | 实际扫描: %d件\n",
        total_system, total_actual);
}

// 滞留件检测
void check_overdue_shelf(Shelf* shelf) {
    if (!shelf) return;

    time_t now = time(NULL);
    ExpressNode* curr = shelf->express_list;
    int overdue_count = 0;

    while (curr) {
        Express* exp = &(curr->data);
        double days = difftime(now, exp->storage_time) / 86400.0;

        if (days > 3 ) { // EXPRESS_NORMAL
            exp->status = STATUS_OVERDUE; // EXPRESS_OVERDUE
            overdue_count++;
            printf("[滞留] %s 已存放%.1f天\n",
                exp->tracking_num, days);
        }
        curr = curr->next;
    }

    if (overdue_count > 0) {
        printf("货架 %s 共有 %d 个滞留件\n",
            shelf->shelf_id, overdue_count);
    }
}

// 货架容量预警
void check_shelf_capacity(Shelf* head) {
    printf("\n===== 货架容量预警 =====\n");
    while (head) {
        float ratio = (float)head->current_count / head->max_capacity;
        if (ratio > 0.8) {
            printf("[警告] 货架 %s 已用 %.0f%%\n",
                head->shelf_id, ratio * 100);
        }
        head = head->next;
    }
}

// 快递定位查询
void locate_express(Shelf* shelves, const char* tracking_num) {
    while (shelves) {
        ExpressNode* curr = shelves->express_list;
        while (curr) {
            if (strcmp(curr->data.tracking_num, tracking_num) == 0) {
                printf("快递 %s 位于货架 %s\n",
                    tracking_num, shelves->shelf_id);
                return;
            }
            curr = curr->next;
        }
        shelves = shelves->next;
    }
    printf("未找到快递 %s\n", tracking_num);
}

//保存货架信息到文件（包含快递单号）


/*其中应修改快递的结构包含下面
  typedef struct Express {
    char tracking_num[20];      // 快递单号（如 "YT123456789"）
    char receiver_phone[12];    // 收件人手机号
    char pickup_code[15];       // 取件码（如 "A01-102"）
    time_t storage_time;        // 入库时间（time_t类型时间戳）
    int status;                 // 状态（0-待取件, 1-已取件, 2-滞留件, 3-异常件）
    Shelf* shelf;               // 所属货架指针（关联货架信息）
    struct Express* next;       // 链表指针
} Express;
*/
