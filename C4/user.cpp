#include "cainiao.h"

// ��ʼ���û���Ȩ
void init_user_privilege(User* user) {
    switch (user->level) {
    case USER_NORMAL:
        user->discount_rate = 1.0f;    // 100% �ۿ���
        user->payment_mode = PAY_CASH; // �ָ�
        break;

    case USER_STUDENT:
        user->discount_rate = 0.85f;   // 85% �ۿ���
        user->payment_mode = PAY_CASH; // �ָ�
        break;

    case USER_VIP:
        user->discount_rate = 0.9f;    // 90% �ۿ���
        user->payment_mode = PAY_MONTHLY;//�½�
        break;

    case USER_ENTERPRISE:
        user->discount_rate = 0.75f;   // 75% �ۿ���
        user->payment_mode = PAY_MONTHLY; // �½�
        break;

    case USER_PARTNER:
        user->discount_rate = 0.6f;    // Э��ۻ�׼60%
        user->payment_mode = PAY_CONTRACT; // ���ڽ���
        break;
    }

    // ͳһ��ʼ�������ֶ�
    user->free_quota = 0;
    user->priority_access = 0;
}

// �������û�
User* create_user(const char* phone, const char* pwd, const char* name, UserLevel level) {
    // �����ڴ�
    User* new_user = (User*)malloc(sizeof(User));
    if (!new_user) {
        perror("Failed to create user");
        return NULL;
    }

    // ������Ϣ
    strncpy_s(new_user->phone, sizeof(new_user->phone), phone, 11);
    new_user->phone[11] = '\0';
    strncpy_s(new_user->password, sizeof(new_user->password), pwd, 49);
    new_user->password[49] = '\0';
    strncpy_s(new_user->username, sizeof(new_user->username), name, 49);
    new_user->username[49] = '\0';
    new_user->level = level;

    // ��ʼ����Ȩ
    init_user_privilege(new_user);
    new_user->next = NULL;

    return new_user;
}

//����û���ͷ�巨��
void add_user(User** head, User* new_user) {
    if (!new_user) return;
    new_user->next = *head;
    *head = new_user;
}

//Ѱ���û������ִ�Сд��
User* find_user(User* head, const char* username) {
    //��� username �Ƿ�Ϊ NULL
    if (!username) return NULL;
    User* curr = head;
    while (curr) {
        if (strcmp(curr->username, username) == 0)//�ַ�����ȫƥ��
            return curr;
        curr = curr->next;
    }
    return NULL;
}

//��������
void update_user_password(User* user, const char* new_password) {
    if (!user || !new_password) return;
    strncpy_s(user->password, sizeof(user->password), new_password, 49);
    user->password[49] = '\0';
}

// ��֤�ֻ��Ÿ�ʽ
bool validate_phone(const char* phone) {
    if (strlen(phone) != 11) return false;

    for (int i = 0; i < 11; i++) {
        if (!isdigit(phone[i])) return false;
    }

    return true;
}

// �����û���Ϣ���ļ�
void save_users_to_file(User* head, const char* filename) {
    //д��ģʽ���ļ�
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to save users");
        return;
    }

    // д���б���
    fprintf(fp, "username,phone,password,level,payment_mode,discount_rate\n");

    User* curr = head;
    while (curr) {
        fprintf(fp, "%s,%s,%s,%d,%d,%.2f\n",
            curr->username,
            curr->phone,
            curr->password,
            curr->level,
            curr->payment_mode,
            curr->discount_rate);
        curr = curr->next;
    }

    fclose(fp);
}

// ���ļ������û���Ϣ��β�巨��
User* load_users_from_file(const char* filename) {
    //ֻ��ģʽ���ļ�
    FILE* fp = fopen(filename, "r");
    if (!fp) return NULL;

    User* head = NULL, * tail = NULL;
    char line[256];

    // ��ȡ��������ͷ
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        char username[50], phone[12], password[50];
        int level, payment_mode;
        float discount_rate;

        // ����ÿ������
        if (sscanf_s(line, "%49[^,],%11[^,],%49[^,],%d,%d,%f",
            username, (unsigned)_countof(username),
            phone, (unsigned)_countof(phone),
            password, (unsigned)_countof(password),
            &level, &payment_mode, &discount_rate) != 6) {
            continue; // ������ʽ����ȷ����
        }

        // �����û�
        User* user = (User*)malloc(sizeof(User));
        if (!user) continue;

        // ������Ϣ
        strncpy_s(user->username, sizeof(user->username), username, 49);
        user->username[49] = '\0';
        strncpy_s(user->phone, sizeof(user->phone), phone, 11);
        user->phone[11] = '\0';
        strncpy_s(user->password, sizeof(user->password), password, 49);
        user->password[49] = '\0';

        // �û��ȼ�����Ȩ
        user->level = (UserLevel)level;
        user->payment_mode = (PaymentMode)payment_mode;
        user->discount_rate = discount_rate;
        user->free_quota = 0;
        user->priority_access = 0;
        user->next = NULL;

        // ��ӵ�����
        if (!head) {
            head = tail = user;
        }
        else {
            tail->next = user;
            tail = user;
        }
    }

    fclose(fp);
    return head;
}

// ע���û�
bool delete_user(User** head, const char* username) {
    if (!head || !username) return false;

    User* prev = NULL, * curr = *head;

    // ����Ҫɾ�����û�
    while (curr) {
        if (strcmp(curr->username, username) == 0) {
            // �ҵ��û�����ʼɾ��
            if (prev) {
                prev->next = curr->next;
            }
            else {
                *head = curr->next;
            }
            free(curr);
            return true;
        }
        prev = curr;
        curr = curr->next;
    }

    return false; // δ�ҵ��û�
}

// �û���¼
void user_login() {
    int attempts = 3;  // ʣ�ೢ�Դ���

    while (attempts > 0) {
        clear_screen();
        printf("\n========== �û���¼ ==========\n");
        printf("ʣ�ೢ�Դ���: %d\n", attempts);

        char input[50], password[50];
        printf("�û������ֻ���: ");
        scanf_s("%49s", input, (unsigned)_countof(input));
        printf("����: ");
        scanf_s("%49s", password, (unsigned)_countof(password));

        // �����û������û������ֻ��ţ�
        User* user = NULL;
        for (int i = 0; i < user_count; i++) {
            if (strcmp(user_list[i].username, input) == 0 ||
                strcmp(user_list[i].phone, input) == 0) {
                user = &user_list[i];
                break;
            }
        }

        if (!user || strcmp(user->password, password) != 0) {
            attempts--;
            printf("\n�û���/�ֻ��Ż��������");
            if (attempts > 0) {
                printf(" �����ԡ�\n");
            }
            else {
                printf("\n���Դ��������꣬ϵͳ���˳���\n");
                exit(0);  // �˳�ϵͳ
            }
            pause_program();
            continue;
        }

        printf("\n��¼�ɹ�����ӭ������%s\n", user->username);
        pause_program();
        user_menu(user);
        return;
    }
}

// �޸ĺ���û�ע�ắ��
void user_register() {
    clear_screen();
    printf("\n========== �û�ע�� ==========\n");

    char username[50], phone[12], password[50], confirm_pwd[50];
    int user_type;

    // �����û���
    printf("�û���: ");
    scanf_s("%49s", username, (unsigned)_countof(username));

    // ����û����Ƿ��Ѵ���
    if (find_user(user_list, username)) {
        printf("\n�û����Ѵ��ڣ�\n");
        pause_program();
        return;
    }

    // �����ֻ���
    printf("�ֻ���: ");
    scanf_s("%11s", phone, (unsigned)_countof(phone));
    while (!validate_phone(phone)) {
        printf("�ֻ��Ÿ�ʽ����ȷ������������: ");
        scanf_s("%11s", phone, (unsigned)_countof(phone));
    }

    // �������뼰ȷ��
    do {
        printf("����: ");
        scanf_s("%49s", password, (unsigned)_countof(password));
        printf("ȷ������: ");
        scanf_s("%49s", confirm_pwd, (unsigned)_countof(confirm_pwd));

        if (strcmp(password, confirm_pwd) != 0) {
            printf("\n������������벻һ�£����������롣\n\n");
        }
    } while (strcmp(password, confirm_pwd) != 0);

    // ѡ���û�����
    printf("\n��ѡ���û�����:\n");
    printf("1. ��ͨ�û�\n");
    printf("2. ѧ���û�\n");
    printf("3. VIP�û�\n");
    printf("4. ��ҵ�û�\n");
    printf("5. �����̻�\n");
    printf("������ѡ��(1-5): ");

    while (1) {
        scanf_s("%d", &user_type);
        if (user_type >= 1 && user_type <= 5) break;
        printf("��Чѡ������������(1-5): ");
    }

    // �����û�
    User* new_user = create_user(phone, password, username, (UserLevel)(user_type - 1));
    if (!new_user) {
        printf("\nע��ʧ�ܣ�\n");
        pause_program();
        return;
    }

    // �����û�����
    add_user(&user_list, new_user);
    user_count++;
    save_users_to_file(user_list, "users.dat");

    printf("\nע��ɹ�����ӭ���������վ��%s\n", username);
    printf("�����˻�����: ");
    switch (new_user->level) {
    case USER_NORMAL:    printf("��ͨ�û�"); break;
    case USER_STUDENT:  printf("ѧ���û�(85��)"); break;
    case USER_VIP:      printf("VIP�û�(9��)"); break;
    case USER_ENTERPRISE: printf("��ҵ�û�(75��)"); break;
    case USER_PARTNER:  printf("�����̻�(Э���)"); break;
    }
    printf("\n");

    pause_program();
}

//�޸��û���Ϣ
bool modify_user_info(User* user) {
    if (!user) return false;

    clear_screen();
    printf("\n========== �޸��û���Ϣ ==========\n");
    printf("��ǰ��Ϣ��\n");
    printf("1. �û���: %s\n", user->username);
    printf("2. �ֻ���: %s\n", user->phone);
    printf("3. ����: ******\n");
    printf("0. ����\n");
    printf("��ѡ��Ҫ�޸ĵ���Ŀ: ");

    int choice;
    scanf_s("%d", &choice);
    clear_input_buffer();

    switch (choice) {
    case 1: { // �޸��û���
        char new_username[50];
        printf("�������û���: ");
        scanf_s("%49s", new_username, (unsigned)_countof(new_username));

        // ����û����Ƿ��Ѵ���
        if (find_user(user_list, new_username)) {
            printf("�û����Ѵ��ڣ�\n");
            pause_program();
            return false;
        }

        strncpy_s(user->username, sizeof(user->username), new_username, 49);
        user->username[49] = '\0';
        printf("�û����޸ĳɹ���\n");
        break;
    }

    case 2: { // �޸��ֻ���
        char new_phone[12];
        printf("�������ֻ���: ");
        scanf_s("%11s", new_phone, (unsigned)_countof(new_phone));

        if (!validate_phone(new_phone)) {
            printf("�ֻ��Ÿ�ʽ����ȷ��\n");
            pause_program();
            return false;
        }

        strncpy_s(user->phone, sizeof(user->phone), new_phone, 11);
        user->phone[11] = '\0';
        printf("�ֻ����޸ĳɹ���\n");
        break;
    }

    case 3: { // �޸�����
        char new_password[50], confirm_pwd[50];
        printf("����������: ");
        scanf_s("%49s", new_password, (unsigned)_countof(new_password));
        printf("ȷ��������: ");
        scanf_s("%49s", confirm_pwd, (unsigned)_countof(confirm_pwd));

        if (strcmp(new_password, confirm_pwd) != 0) {
            printf("������������벻һ�£�\n");
            pause_program();
            return false;
        }

        strncpy_s(user->password, sizeof(user->password), new_password, 49);
        user->password[49] = '\0';
        printf("�����޸ĳɹ���\n");
        break;
    }

    case 0:
        return false;

    default:
        printf("��Ч��ѡ��\n");
        pause_program();
        return false;
    }

    // �����޸ĵ��ļ�
    save_users_to_file(user_list, "users.dat");
    pause_program();
    return true;
}

// �����û��ȼ��������
float calculate_fee(User* user, float base_price) {
    // �����̻��ж���Э���ۿ�
    if (user->level == USER_PARTNER) {
        return base_price * user->discount_rate * 0.9f; // Э����ٴ�9��
    }
    return base_price * user->discount_rate;
}

// �����û�֧��
void process_payment(User* user, float amount) {
    printf("�û�[%s] �ȼ�[%d] Ӧ����: ?%.2f\n",
        user->username, user->level, amount);

    switch (user->payment_mode) {
    case PAY_CASH:
        printf("���ֳ�֧��: ?%.2f\n", amount);
        break;

    case PAY_MONTHLY:
        printf("�Ѽ����½��˻� (�����ۼ�: ?%.2f)\n", amount);
        break;

    case PAY_CONTRACT:
        printf("����Э�����ڽ��� (���: ?%.2f)\n", amount);
        break;
    }
}