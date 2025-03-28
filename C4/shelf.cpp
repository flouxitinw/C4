#include "cainiao.h"
#include <string.h>
#include <time.h>
#define _CRT_SECURE_NO_WARNINGS

// ��������
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

// ���ӻ��ܣ�ͷ�巨��
void add_shelf(Shelf** head, Shelf* new_shelf) {
    if (!new_shelf) return;
    new_shelf->next = *head;
    *head = new_shelf;
}

// �ҵ�����ʻ��ܣ���������С�Ļ�����⣩
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

// �����ݵ�����
void assign_express_to_shelf(Shelf** shelves, Express* express) {
    if (!express) return;

    Shelf* target = find_best_shelf(*shelves, express);
    if (!target) {
        printf("���л����������޷���ſ�ݡ�\n");
        return;
    }

    // ����ȡ����
    snprintf(express->pickup_code, sizeof(express->pickup_code), "%s-%03d",
        target->shelf_id, target->current_count + 1);

    // ��¼��ݵ�����
    ExpressNode* new_node = (ExpressNode*)malloc(sizeof(ExpressNode));
    if (!new_node) {
        perror("Failed to allocate express node");
        return;
    }
    memcpy(&new_node->data, express, sizeof(Express));
    new_node->next = target->express_list;
    target->express_list = new_node;

    target->current_count++;
    strncpy_s(express->shelf_id, sizeof(express->shelf_id), target->shelf_id, _TRUNCATE);  // ʹ�� strncpy_s �����ַ���
}

// ���������Ϣ���ļ�
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

// ���ļ��л�ȡ������Ϣ
Shelf* load_shelves_from_file(const char* filename, Express* express_list) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return NULL;

    Shelf* head = NULL, * tail = NULL;
    char line[256];
    fgets(line, sizeof(line), fp); // ������ͷ

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

        // �������
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

// ��ȡ״̬�ı�
const char* get_status_text(int status) {
    static const char* texts[] = {
        "��ȡ��", "��ȡ��", "������", "�쳣��"
    };
    return (status >= 0 && status < 4) ? texts[status] : "δ֪״̬";
}

// �̵㵥������
void inventory_shelf(Shelf* shelf) {
    if (!shelf) return;

    printf("\n====== ���� %s �̵� ======\n", shelf->shelf_id);
    printf("����: %d/%d\n", shelf->current_count, shelf->max_capacity);

    int actual_count = 0;
    ExpressNode* curr = shelf->express_list;

    while (curr) {
        Express* exp = &(curr->data);
        printf("[%s] %s | ״̬: %s\n",
            exp->pickup_code,
            exp->tracking_num,
            get_status_text(exp->status));

        if (exp->status == 2) { // EXPRESS_OVERDUE
            printf(" !!! �������账��\n");
        }
        actual_count++;
        curr = curr->next;
    }

    if (actual_count != shelf->current_count) {
        printf("\n���棺ϵͳ��¼%d����ʵ��ɨ��%d��\n",
            shelf->current_count, actual_count);
    }
    printf("----------------------------\n");
}

// ȫվ�̵�
void full_inventory(Shelf* head) {
    int total_system = 0, total_actual = 0;

    printf("\n======== ȫվ����̵� ========\n");
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

    printf("\n���ܼơ�ϵͳ��¼: %d�� | ʵ��ɨ��: %d��\n",
        total_system, total_actual);
}

// ���������
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
            printf("[����] %s �Ѵ��%.1f��\n",
                exp->tracking_num, days);
        }
        curr = curr->next;
    }

    if (overdue_count > 0) {
        printf("���� %s ���� %d ��������\n",
            shelf->shelf_id, overdue_count);
    }
}

// ��������Ԥ��
void check_shelf_capacity(Shelf* head) {
    printf("\n===== ��������Ԥ�� =====\n");
    while (head) {
        float ratio = (float)head->current_count / head->max_capacity;
        if (ratio > 0.8) {
            printf("[����] ���� %s ���� %.0f%%\n",
                head->shelf_id, ratio * 100);
        }
        head = head->next;
    }
}

// ��ݶ�λ��ѯ
void locate_express(Shelf* shelves, const char* tracking_num) {
    while (shelves) {
        ExpressNode* curr = shelves->express_list;
        while (curr) {
            if (strcmp(curr->data.tracking_num, tracking_num) == 0) {
                printf("��� %s λ�ڻ��� %s\n",
                    tracking_num, shelves->shelf_id);
                return;
            }
            curr = curr->next;
        }
        shelves = shelves->next;
    }
    printf("δ�ҵ���� %s\n", tracking_num);
}

//���������Ϣ���ļ���������ݵ��ţ�


/*����Ӧ�޸Ŀ�ݵĽṹ��������
  typedef struct Express {
    char tracking_num[20];      // ��ݵ��ţ��� "YT123456789"��
    char receiver_phone[12];    // �ռ����ֻ���
    char pickup_code[15];       // ȡ���루�� "A01-102"��
    time_t storage_time;        // ���ʱ�䣨time_t����ʱ�����
    int status;                 // ״̬��0-��ȡ��, 1-��ȡ��, 2-������, 3-�쳣����
    Shelf* shelf;               // ��������ָ�루����������Ϣ��
    struct Express* next;       // ����ָ��
} Express;
*/
