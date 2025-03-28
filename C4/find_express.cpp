#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cainiao.h"
// 检查字符串是否全为数字
bool is_digits(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return false;
        }
    }
    return true;
}

// 通过快递单号查找取件码
const char* find_pickup_code(ExpressNode* express_head, const char* tracking_num) {
    // 验证快递单号格式（12位数字）
    if (strlen(tracking_num) != 12 || !is_digits(tracking_num)) {
        printf("错误：快递单号必须为12位数字\n");
        return NULL;
    }

    ExpressNode* current = express_head;
    while (current != NULL) {
        // 访问ExpressNode中的express指针成员
        if (strcmp(current->data.tracking_num, tracking_num) == 0) {
            return current->data.pickup_code;  // 返回取件码
        }
        current = current->next;
    }

    printf("未找到快递单号 %s 对应的记录\n", tracking_num);
    return NULL;
}

