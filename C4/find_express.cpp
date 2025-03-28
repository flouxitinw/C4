#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cainiao.h"
// ����ַ����Ƿ�ȫΪ����
bool is_digits(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return false;
        }
    }
    return true;
}

// ͨ����ݵ��Ų���ȡ����
const char* find_pickup_code(ExpressNode* express_head, const char* tracking_num) {
    // ��֤��ݵ��Ÿ�ʽ��12λ���֣�
    if (strlen(tracking_num) != 12 || !is_digits(tracking_num)) {
        printf("���󣺿�ݵ��ű���Ϊ12λ����\n");
        return NULL;
    }

    ExpressNode* current = express_head;
    while (current != NULL) {
        // ����ExpressNode�е�expressָ���Ա
        if (strcmp(current->data.tracking_num, tracking_num) == 0) {
            return current->data.pickup_code;  // ����ȡ����
        }
        current = current->next;
    }

    printf("δ�ҵ���ݵ��� %s ��Ӧ�ļ�¼\n", tracking_num);
    return NULL;
}

