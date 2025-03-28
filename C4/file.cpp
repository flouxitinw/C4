#include "cainiao.h"

// ��������Ϣ���ļ�
void save_expresses_to_file(ExpressNode* head, const char* filename) {
    if (!filename) {
        printf("�����ļ�������Ϊ��\n");
        return;
    }

    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("�޷����ļ���������Ϣ");
        return;
    }

    // д��CSV��ͷ
    fprintf(fp, "tracking_num,sender_name,sender_phone,receiver_name,receiver_phone,"
        "weight,volume,package_type,method,region,is_insured,insured_value,"
        "express_type,fee,status,create_time,pickup_code,shelf_id,storage_time\n");

    // ��������д������
    ExpressNode* current = head;
    while (current != NULL) {
        Express* e = &(current->data); // ֱ�ӷ�����Ƕ��Express�ṹ��
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
    printf("�����Ϣ�ѱ��浽 %s\n", filename);
}

// ���ļ����ؿ����Ϣ
ExpressNode* load_expresses_from_file(const char* filename) {
    if (!filename) {
        return NULL;
    }

    FILE* fp = fopen(filename, "r");
    if (!fp) {
        // �ļ�������ʱ�������������״����У�
        return NULL;
    }

    ExpressNode* head = NULL;
    ExpressNode* tail = NULL;
    char line[1024];

    // ����CSV��ͷ
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        Express express_data; // ��ʱ�洢��ȡ������
        memset(&express_data, 0, sizeof(Express));

        // ����CSV�����ݣ�ע��ö�����͵�ǿ��ת����
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
            printf("���棺�����������ʧ�ܣ���������\n");
            continue;
        }

        // �����½ڵ㣨���Զ�����express_data��
        ExpressNode* new_node = create_express_node(&express_data);
        if (!new_node) {
            perror("������ݽڵ�ʧ��");
            continue;
        }

        // ��ӵ�����β��
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
//// ����Ա�̵���
//void admin_inventory(ExpressNode* head) {
//    int counts[6] = { 0 }; // ��Ӧ6��״̬
//    ExpressNode* curr = head;
//
//    while (curr) {
//        counts[curr->data.status]++;
//        curr = curr->next;
//    }
//
//    printf("\n=== ���ͳ�� ===");
//    printf("\n���ĳ�: %d", counts[STATUS_PENDING]);
//    printf("\n������: %d", counts[STATUS_TRANSPORT]);
//    // ...����״̬ͳ��...
//}
//
//// ���°���״̬
//void update_status(ExpressNode* head, const char* tracking_num, PackageStatus new_status) {
//    ExpressNode* curr = head;
//    while (curr) {
//        if (strcmp(curr->data.tracking_num, tracking_num) == 0) {
//            curr->data.status = new_status;
//            curr->data.create_time = time(NULL); // ����ʱ��
//            break;
//        }
//        curr = curr->next;
//    }
//    save_to_file(head);
//}
//
//// �û���¼֪ͨ
//void check_user_notifications(User* user, ExpressNode* head) {
//    ExpressNode* curr = head;
//    time_t now = time(NULL);
//
//    while (curr) {
//        if (strcmp(curr->data.receiver_phone, user->phone) == 0) {
//            // ��������
//            if (curr->data.package_type == FRESH &&
//                curr->data.status == STATUS_PENDING) {
//                printf("\n[����] �������ʰ�����ȡ������: %s", curr->data.tracking_num);
//            }
//
//            // �������� (����3��)
//            if (curr->data.status == STATUS_PENDING &&
//                (now - curr->data.storage_time) > 259200) {
//                printf("\n[����] ���� %s ����������3��", curr->data.tracking_num);
//            }
//
//            // Ͷ�ߴ�����
//            if (curr->data.status == STATUS_COMPLAINT_RESOLVED) {
//                printf("\n[֪ͨ] Ͷ�߰��� %s �Ѵ������", curr->data.tracking_num);
//                curr->data.status = STATUS_DELIVERED; // ���Ϊ�Ѷ�
//            }
//        }
//        curr = curr->next;
//    }
//}
//
//// ����Աɾ����¼
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