#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cainiao.h"
void pickup_express(User* current_user, ExpressNode** express_head) {
    if (!current_user || !express_head) {
        printf("系统错误：参数无效\n");
        return;
    }

    // 1. 获取当前用户待取快递列表
    ExpressNode* curr = *express_head;
    int express_count = 0;

    printf("\n=== %s的待取快递 ===\n", current_user->username);
    while (curr != NULL) {
        // 匹配当前用户且状态为"待取件"
        if (strcmp(curr->data.receiver_phone, current_user->phone) == 0 &&
            curr->data.status == STATUS_UNPICKED) { // 使用枚举值比较

            // 显示快递信息
            printf("%d. 快递单号：%s\n", ++express_count, curr->data.tracking_num);
            printf("   物品类型：%s\n", get_package_type_name(curr->data.package_type));
            printf("   到站时间：%s", ctime(&curr->data.storage_time));
            printf("   -----------------\n");
        }
        curr = curr->next;
    }

    if (express_count == 0) {
        printf("当前没有您的待取快递\n");
        return;
    }

    // 2. 选择要取的快递
    char tracking_num[20]; // 匹配 tracking_num 的大小
    printf("\n请输入要取的快递单号：");
    scanf_s("%19s", tracking_num);
    getchar(); // 清除输入缓冲

    // 3. 查找快递
    curr = *express_head;
    Express* target_express = NULL;
    while (curr != NULL) {
        if (strcmp(curr->data.tracking_num, tracking_num) == 0 &&
            strcmp(curr->data.receiver_phone, current_user->phone) == 0) {
            target_express = &(curr->data);
            break;
        }
        curr = curr->next;
    }

    if (!target_express) {
        printf("未找到匹配的快递记录！\n");
        return;
    }

    // 4. 验证取件码
    char input_code[15]; // 匹配 pickup_code 的大小
    printf("请输入取件码：");
    scanf("%14s", input_code);
    getchar();

    if (strcmp(target_express->pickup_code, input_code) != 0) {
        printf("取件码错误！\n");
        return;
    }

    // 5. 确认取件
    char confirm;
    printf("确认取走快递 %s 吗？(y/n): ", tracking_num);
    scanf("%c", &confirm);

    if (tolower(confirm) == 'y') {
        // 6. 更新快递状态
        target_express->status = STATUS_PICKED;
        target_express->create_time = time(NULL); // 更新为签收时间

        printf("\n=== 取件成功 ===\n");
        printf("快递单号：%s\n", tracking_num);
        printf("签收时间：%s", ctime(&target_express->create_time));

        // 特殊物品提示
        if (target_express->package_type == VALUABLES && target_express->is_insured) {
            printf("请查验贵重物品是否完好，并在签收单上签字确认！\n");
        }
    }
    else {
        printf("已取消取件操作\n");
    }
}