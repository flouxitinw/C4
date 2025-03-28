#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cainiao.h"
void pickup_express(User* current_user, ExpressNode** express_head) {
    if (!current_user || !express_head) {
        printf("ϵͳ���󣺲�����Ч\n");
        return;
    }

    // 1. ��ȡ��ǰ�û���ȡ����б�
    ExpressNode* curr = *express_head;
    int express_count = 0;

    printf("\n=== %s�Ĵ�ȡ��� ===\n", current_user->username);
    while (curr != NULL) {
        // ƥ�䵱ǰ�û���״̬Ϊ"��ȡ��"
        if (strcmp(curr->data.receiver_phone, current_user->phone) == 0 &&
            curr->data.status == STATUS_UNPICKED) { // ʹ��ö��ֵ�Ƚ�

            // ��ʾ�����Ϣ
            printf("%d. ��ݵ��ţ�%s\n", ++express_count, curr->data.tracking_num);
            printf("   ��Ʒ���ͣ�%s\n", get_package_type_name(curr->data.package_type));
            printf("   ��վʱ�䣺%s", ctime(&curr->data.storage_time));
            printf("   -----------------\n");
        }
        curr = curr->next;
    }

    if (express_count == 0) {
        printf("��ǰû�����Ĵ�ȡ���\n");
        return;
    }

    // 2. ѡ��Ҫȡ�Ŀ��
    char tracking_num[20]; // ƥ�� tracking_num �Ĵ�С
    printf("\n������Ҫȡ�Ŀ�ݵ��ţ�");
    scanf_s("%19s", tracking_num);
    getchar(); // ������뻺��

    // 3. ���ҿ��
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
        printf("δ�ҵ�ƥ��Ŀ�ݼ�¼��\n");
        return;
    }

    // 4. ��֤ȡ����
    char input_code[15]; // ƥ�� pickup_code �Ĵ�С
    printf("������ȡ���룺");
    scanf("%14s", input_code);
    getchar();

    if (strcmp(target_express->pickup_code, input_code) != 0) {
        printf("ȡ�������\n");
        return;
    }

    // 5. ȷ��ȡ��
    char confirm;
    printf("ȷ��ȡ�߿�� %s ��(y/n): ", tracking_num);
    scanf("%c", &confirm);

    if (tolower(confirm) == 'y') {
        // 6. ���¿��״̬
        target_express->status = STATUS_PICKED;
        target_express->create_time = time(NULL); // ����Ϊǩ��ʱ��

        printf("\n=== ȡ���ɹ� ===\n");
        printf("��ݵ��ţ�%s\n", tracking_num);
        printf("ǩ��ʱ�䣺%s", ctime(&target_express->create_time));

        // ������Ʒ��ʾ
        if (target_express->package_type == VALUABLES && target_express->is_insured) {
            printf("����������Ʒ�Ƿ���ã�����ǩ�յ���ǩ��ȷ�ϣ�\n");
        }
    }
    else {
        printf("��ȡ��ȡ������\n");
    }
}