// cainiao.c
#include "cainiao.h"


//--------------------��������--------------------
//��ȡ��Ʒ��������
const char* get_package_type_name(PackageType type) {
    const char* names[] = {
        "����Ʒ", "ʳƷ", "�ļ�", "����",
        "�����Ʒ", "��", "����", "����",
        "��ȼ/�ױ�/�ж���Ʒ", "������Ʒ", "����",
        "����Ʒ", "����"
    };
    return names[type];
}
// ����Ƿ�����Ч�� 11 λ�ֻ���
int is_valid_phone(const char* phone) {
    if (strlen(phone) != 11) {
        return 0;
    }
    for (int i = 0; i < 11; i++) {
        if (!isdigit(phone[i])) {
            return 0;
        }
    }
    return 1;
}
// ��ȡ�û����뷶Χ�ڵ�����
int input_int_in_range(const char* prompt, int min, int max) {
    int choice;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &choice) != 1) {  // ����Ƿ�ɹ���ȡ����
            printf("������Ч�����������룡\n");
            while (getchar() != '\n');  // ������뻺����
            continue;
        }
        if (choice < min || choice > max) {  // ����Ƿ��ڷ�Χ��
            printf("���������� %d �� %d ֮�䣬���������룡\n", min, max);
            continue;
        }
        break;  // ������ȷ���˳�ѭ��
    }
    return choice;
}
// ����һ���������������� > 0��
double input_positive_double(const char* prompt) {
    double value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%lf", &value) != 1) {  // ����Ƿ�ɹ���ȡ������
            printf("��������Ч�����֣�");
            while (getchar() != '\n');  // ������뻺����
            continue;
        }
        if (value <= 0) {  // ����Ƿ� > 0
            printf("���������� 0�����������룺");
            continue;
        }
        break;  // ������ȷ���˳�ѭ��
    }
    return value;
}


//--------------------�Ŀ�ݺ��ĺ���--------------------
void send_express(User* user_head, ExpressNode** express_head) {
    Express* new_express = (Express*)malloc(sizeof(Express));
    if (!new_express) {
        perror("�ڴ����ʧ��");
        return;
    }
    memset(new_express, 0, sizeof(Express)); // ��ʼ������


    printf("== = �Ŀ����Ϣ¼�� == =");
    // �ļ�����Ϣ
    printf("�ļ���������");
    scanf("%s", new_express->sender_name);;
    while (1) {
        printf("�ļ��˵绰��");
        fgets(new_express->sender_phone, 11, stdin);
        new_express->sender_phone[strcspn(new_express->sender_phone, "\n")] = '\0';  // ȥ�����з�

        if (is_valid_phone(new_express->sender_phone)) {
            break;  // ������ȷ���˳�ѭ��
        }
        else {
            printf("��������ȷ�ֻ��ţ�");
        }
    }
    // �ռ�����Ϣ
    printf("�ռ���������");
    scanf("%s", new_express->receiver_name);
    while (1) {
        printf("�ռ��˵绰��");
        fgets(new_express->receiver_phone, 11, stdin);
        new_express->receiver_phone[strcspn(new_express->receiver_phone, "\n")] = '\0';  // ȥ�����з�

        if (is_valid_phone(new_express->receiver_phone)) {
            break;  // ������ȷ���˳�ѭ��
        }
        else {
            printf("��������ȷ�ֻ��ţ�");
        }
    }


    // ������Ϣ
    //�������������
    new_express->weight = input_positive_double("����������kg)��");
    new_express->volume = input_positive_double("�������(cm^3)��");
    if (new_express->weight > 50 || new_express->volume >= 3000) {
        printf("�������󲻿��ʼ�");
        return;
    }
    // ��Ʒ����ѡ��
    printf("��ѡ����Ʒ���ͣ�");
    for (int i = 0; i < 10; i++) {
        printf("%d. %s", i, get_package_type_name((PackageType)i));
    }
    int type_choice = input_int_in_range("������ѡ�0-9����", 0, 9);
    if (type_choice >= 5 && type_choice <= 8) {
        printf("���ݹ������������ֹ�ʼ�");
        return;
    }
    new_express->package_type = (PackageType)(type_choice);
    if (new_express->package_type == FRESH) {
        printf(" ����ѡ����������\n ");
    }
    if (new_express->package_type == VALUABLES || new_express->package_type == DIGITAL) {
        printf("���鿪�����۷���\n");
    }
    if (new_express->weight > 20.0) {
        printf("����ѡ�����ʼķ�ʽ\n");
    }


    // ���䷽ʽѡ��
    printf("��ѡ�����䷽ʽ");
    printf("0. ��ͨ���\n1. ����ʼ�\n 2. �����ʼ�\n");
    int method_choice = input_int_in_range("������ѡ�0-2����", 0, 2);
    new_express->method = (ShippingMethod)method_choice;


    // ����ѡ��
    printf(" ��ѡ��������ͣ�");
    printf("0. ʡ��\n1. ʡ��\n 2. ����\n 3. �۰�̨\n");
    int region_choice = input_int_in_range("������ѡ�0-3����", 0, 3);
    new_express->region = (RegionType)region_choice;


    // ������Ϣ
    new_express->insured_value = input_positive_double("��Ʒ��ֵ��");
    new_express->is_insured = input_int_in_range("�Ƿ񱣼ۣ�0-�� 1-�ǣ���", 0, 1);



    //����ʱЧ
    printf("\n��ѡ������ʱЧ��\n");
    printf("0. ��죨��ͨʱЧ��\n");
    printf("1. �ؿ죨�Ӽ���\n");
    int choice = input_int_in_range("������ѡ�0-1����", 0, 1);
    new_express->express_type = (DeliveryType)choice;
    // ����Ĭ��ֵ
    new_express->status = STATUS_UNSENT;//���Ҫ����;û�гɹ���ô������᲻����ʾ



    // ��������ڵ�
    ExpressNode* new_node = create_express_node(new_express); // �Զ���������
    if (!new_node) {
        perror("�ڴ����ʧ��");
        return;
    }

    // ���뵽����ͷ��
    new_node->next = *express_head;  // �½ڵ�ָ��ԭͷ�ڵ�
    *express_head = new_node;        // ����ͷָ��ָ���½ڵ�

    printf("����ѳɹ�����!��Ӧ��%.2fԪ\n", new_node->data.fee); // ͨ���ڵ��������
    
}