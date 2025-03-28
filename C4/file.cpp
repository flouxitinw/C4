#include "cainiao.h"

// 保存快递信息到文件
void save_expresses_to_file(ExpressNode* head, const char* filename) {
    if (!filename) {
        printf("错误：文件名不能为空\n");
        return;
    }

    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("无法打开文件保存快递信息");
        return;
    }

    // 写入CSV表头
    fprintf(fp, "tracking_num,sender_name,sender_phone,receiver_name,receiver_phone,"
        "weight,volume,package_type,method,region,is_insured,insured_value,"
        "express_type,fee,status,create_time,pickup_code,shelf_id,storage_time\n");

    // 遍历链表并写入数据
    ExpressNode* current = head;
    while (current != NULL) {
        Express* e = &(current->data); // 直接访问内嵌的Express结构体
        fprintf(fp, "%s,%s,%s,%s,%s,%.2f,%.2f,%d,%d,%d,%d,%.2f,%d,%.2f,%d,%ld,%s,%s,%ld\n",
            e->tracking_num,
            e->sender_name,
            e->sender_phone,
            e->receiver_name,
            e->receiver_phone,
            e->weight,
            e->volume,
            e->package_type,
            e->method,
            e->region,
            e->is_insured,
            e->insured_value,
            e->express_type,
            e->fee,
            e->status,
            e->create_time,
            e->pickup_code,
            e->shelf_id,
            e->storage_time);
        current = current->next;
    }

    fclose(fp);
    printf("快递信息已保存到 %s\n", filename);
}

// 从文件加载快递信息
ExpressNode* load_expresses_from_file(const char* filename) {
    if (!filename) {
        return NULL;
    }

    FILE* fp = fopen(filename, "r");
    if (!fp) {
        // 文件不存在时不报错（可能是首次运行）
        return NULL;
    }

    ExpressNode* head = NULL;
    ExpressNode* tail = NULL;
    char line[1024];

    // 跳过CSV表头
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        Express express_data; // 临时存储读取的数据
        memset(&express_data, 0, sizeof(Express));

        // 解析CSV行数据（注意枚举类型的强制转换）
        if (sscanf(line, "%19[^,],%49[^,],%11[^,],%49[^,],%11[^,],%lf,%lf,%d,%d,%d,%d,%lf,%d,%lf,%d,%ld,%14[^,],%9[^,],%ld",
            express_data.tracking_num,
            express_data.sender_name,
            express_data.sender_phone,
            express_data.receiver_name,
            express_data.receiver_phone,
            &express_data.weight,
            &express_data.volume,
            (int*)&express_data.package_type,
            (int*)&express_data.method,
            (int*)&express_data.region,
            &express_data.is_insured,
            &express_data.insured_value,
            (int*)&express_data.express_type,
            &express_data.fee,
            (int*)&express_data.status,
            &express_data.create_time,
            express_data.pickup_code,
            express_data.shelf_id,
            &express_data.storage_time) != 19) {
            printf("警告：解析快递数据失败，跳过此行\n");
            continue;
        }

        // 创建新节点（会自动复制express_data）
        ExpressNode* new_node = create_express_node(&express_data);
        if (!new_node) {
            perror("创建快递节点失败");
            continue;
        }

        // 添加到链表尾部
        if (!head) {
            head = tail = new_node;
        }
        else {
            tail->next = new_node;
            tail = new_node;
        }
    }

    fclose(fp);
    return head;
}
//// 管理员盘点库存
//void admin_inventory(ExpressNode* head) {
//    int counts[6] = { 0 }; // 对应6种状态
//    ExpressNode* curr = head;
//
//    while (curr) {
//        counts[curr->data.status]++;
//        curr = curr->next;
//    }
//
//    printf("\n=== 库存统计 ===");
//    printf("\n待寄出: %d", counts[STATUS_PENDING]);
//    printf("\n运输中: %d", counts[STATUS_TRANSPORT]);
//    // ...其他状态统计...
//}
//
//// 更新包裹状态
//void update_status(ExpressNode* head, const char* tracking_num, PackageStatus new_status) {
//    ExpressNode* curr = head;
//    while (curr) {
//        if (strcmp(curr->data.tracking_num, tracking_num) == 0) {
//            curr->data.status = new_status;
//            curr->data.create_time = time(NULL); // 更新时间
//            break;
//        }
//        curr = curr->next;
//    }
//    save_to_file(head);
//}
//
//// 用户登录通知
//void check_user_notifications(User* user, ExpressNode* head) {
//    ExpressNode* curr = head;
//    time_t now = time(NULL);
//
//    while (curr) {
//        if (strcmp(curr->data.receiver_phone, user->phone) == 0) {
//            // 生鲜提醒
//            if (curr->data.package_type == FRESH &&
//                curr->data.status == STATUS_PENDING) {
//                printf("\n[紧急] 您有生鲜包裹待取！单号: %s", curr->data.tracking_num);
//            }
//
//            // 滞留提醒 (超过3天)
//            if (curr->data.status == STATUS_PENDING &&
//                (now - curr->data.storage_time) > 259200) {
//                printf("\n[提醒] 包裹 %s 已滞留超过3天", curr->data.tracking_num);
//            }
//
//            // 投诉处理结果
//            if (curr->data.status == STATUS_COMPLAINT_RESOLVED) {
//                printf("\n[通知] 投诉包裹 %s 已处理完成", curr->data.tracking_num);
//                curr->data.status = STATUS_DELIVERED; // 标记为已读
//            }
//        }
//        curr = curr->next;
//    }
//}
//
//// 管理员删除记录
//void admin_delete_record(ExpressNode** head, const char* tracking_num) {
//    ExpressNode* curr = *head, * prev = NULL;
//
//    while (curr) {
//        if (strcmp(curr->data.tracking_num, tracking_num) == 0) {
//            if (prev) prev->next = curr->next;
//            else *head = curr->next;
//
//            free(curr->express);
//            free(curr);
//            save_to_file(*head);
//            return;
//        }
//        prev = curr;
//        curr = curr->next;
//    }
//}