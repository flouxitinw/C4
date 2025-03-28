#include "cainiao.h"
#include <ctype.h>
#define _CRT_SECURE_NO_WARNINGS // ���ð�ȫ����

// ȫ�ֱ�������
Admin* admin_list = NULL;
int admin_count = 0;

// ������뻺����
void clear_input_buffer() {
    while (getchar() != '\n');
}

// ��������Ա
Admin* create_admin(const char* username, const char* phone, const char* password, int privilege_level) {
    Admin* new_admin = (Admin*)malloc(sizeof(Admin));
    if (!new_admin) {
        perror("Failed to create admin");
        return NULL;
    }

    strncpy_s(new_admin->username, MAX_USERNAME_LEN, username, _TRUNCATE);
    new_admin->username[MAX_USERNAME_LEN - 1] = '\0';

    strncpy_s(new_admin->phone, MAX_PHONE_LEN, phone, _TRUNCATE);
    new_admin->phone[MAX_PHONE_LEN - 1] = '\0';

    strncpy_s(new_admin->password, MAX_PASSWORD_LEN, password, _TRUNCATE);
    new_admin->password[MAX_PASSWORD_LEN - 1] = '\0';

    new_admin->privilege_level = (AdminPrivilegeLevel)privilege_level;
    new_admin->next = NULL;

    return new_admin;
}

// ��ӹ���Ա��ͷ�巨��
void add_admin(Admin** head, Admin* new_admin) {
    if (!new_admin) return;
    new_admin->next = *head;
    *head = new_admin;
    admin_count++;
}

// ���ҹ���Ա
Admin* find_admin(Admin* head, const char* username) {
    if (!username) return NULL;

    Admin* curr = head;
    while (curr) {
        if (strcmp(curr->username, username) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

// ���¹���Ա����
void update_admin_password(Admin* admin, const char* new_password) {
    if (!admin || !new_password) return;
    strncpy_s(admin->password, MAX_PASSWORD_LEN, new_password, _TRUNCATE);
    admin->password[MAX_PASSWORD_LEN - 1] = '\0';
}



// �������Ա���ݵ��ļ�
void save_admins_to_file(Admin* head, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to save admins");
        return;
    }

    // д���б���
    fprintf(fp, "username,phone,password,privilege_level\n");

    Admin* curr = head;
    while (curr) {
        fprintf(fp, "\"%s\",\"%s\",\"%s\",%d\n",
            curr->username,
            curr->phone,
            curr->password,
            curr->privilege_level);
        curr = curr->next;
    }

    fclose(fp);
}

// ���ļ����ع���Ա���ݣ�β�巨��
Admin* load_admins_from_file(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return NULL;

    Admin* head = NULL, * tail = NULL;
    char line[256];
    fgets(line, sizeof(line), fp); // ������ͷ

    while (fgets(line, sizeof(line), fp)) {
        char username[MAX_USERNAME_LEN], phone[MAX_PHONE_LEN], password[MAX_PASSWORD_LEN];
        int privilege_level;

        if (sscanf_s(line, "%49[^,],%11[^,],%49[^,],%d",
            username, (unsigned)_countof(username),
            phone, (unsigned)_countof(phone),
            password, (unsigned)_countof(password),
            &privilege_level) != 4) {
            continue; // ������ʽ������
        }

        // ȥ�����ܵ�����
        if (username[0] == '"') {
            memmove(username, username + 1, strlen(username));
            username[strlen(username) - 1] = '\0';
        }
        if (phone[0] == '"') {
            memmove(phone, phone + 1, strlen(phone));
            phone[strlen(phone) - 1] = '\0';
        }
        if (password[0] == '"') {
            memmove(password, password + 1, strlen(password));
            password[strlen(password) - 1] = '\0';
        }

        Admin* admin = create_admin(username, phone, password, privilege_level);
        if (!admin) continue;

        if (!head) {
            head = tail = admin;
        }
        else {
            tail->next = admin;
            tail = admin;
        }
        admin_count++;
    }

    fclose(fp);
    return head;
}

// �޸Ĺ���ԱȨ�޼���
void admin_change_privilege(Admin* executor, const char* target_username, int new_level) {
    if (!executor || executor->privilege_level != SUPER_ADMIN) {
        printf("\nȨ�޲��㣡ֻ�г�������Ա�����޸�Ȩ�޼���\n");
        return;
    }

    if (new_level != NORMAL_ADMIN && new_level != SUPER_ADMIN) {
        printf("\n��Ч��Ȩ�޼���\n");
        return;
    }

    Admin* target = find_admin(admin_list, target_username);
    if (!target) {
        printf("\n�Ҳ���ָ������Ա��\n");
        return;
    }

    if (target == executor) {
        printf("\n�����޸��Լ���Ȩ�޼���\n");
        return;
    }
    target->privilege_level = (AdminPrivilegeLevel)new_level;
    save_admins_to_file(admin_list, "admins.dat");
    printf("\n�ѳɹ��޸� %s ��Ȩ��Ϊ%s����Ա\n",
        target_username,
        new_level == SUPER_ADMIN ? "����" : "��ͨ");
}

// ����Ƿ��ǳ�������Ա
bool is_super_admin(const Admin* admin) {
    return admin && admin->privilege_level == SUPER_ADMIN;
}

// Ȩ����֤
bool has_permission(const Admin* admin, int required_level) {
    if (!admin) return false;
    return admin->privilege_level >= required_level;
}

// ����Ա��¼��֧���û������ֻ��ŵ�¼��
void admin_login() {
    int attempts = 3;

    while (attempts > 0) {
        clear_screen();
        printf("\n========== ����Ա��¼ ==========\n");
        printf("ʣ�ೢ�Դ���: %d\n", attempts);

        char input[50], password[50];
        printf("�û������ֻ���: ");
        scanf_s("%49s", input, (unsigned)_countof(input));
        printf("����: ");
        scanf_s("%49s", password, (unsigned)_countof(password));

        Admin* admin = NULL;
        Admin* curr = admin_list;

        // ��������ƥ���û������ֻ��ŵĹ���Ա
        while (curr) {
            if (strcmp(curr->username, input) == 0 ||
                strcmp(curr->phone, input) == 0) {
                admin = curr;
                break;
            }
            curr = curr->next;
        }

        // ��֤��¼
        if (!admin || strcmp(admin->password, password) != 0) {
            attempts--;
            printf("\n�û���/�ֻ��Ż��������");
            if (attempts > 0) {
                printf(" �����ԡ�\n");
            }
            else {
                printf("\n���Դ��������꣬ϵͳ���˳���\n");
                exit(0);
            }
            pause_program();
            continue;
        }

        // ��ʾȨ�޼���
        printf("\n��¼�ɹ�����ӭ������%s %s\n",
            admin->privilege_level == SUPER_ADMIN ? "[��������Ա]" : "[��ͨ����Ա]",
            admin->username);
        pause_program();

        admin_menu(admin);
        return;
    }
}

// ����Աע��
void admin_register(Admin* executor) {
    // Ȩ�޼��
    if (executor && executor->privilege_level != SUPER_ADMIN) {
        printf("\nֻ�г�������Ա����ע���¹���Ա��\n");
        pause_program();
        return;
    }

    clear_screen();
    printf("\n========== ����Աע�� ==========\n");

    char username[MAX_USERNAME_LEN], phone[MAX_PHONE_LEN], password[MAX_PASSWORD_LEN], confirm_pwd[MAX_PASSWORD_LEN];
    int privilege_level = NORMAL_ADMIN;  // Ĭ��ע����ͨ����Ա

    // �����û���
    printf("�û���: ");
    scanf_s("%49s", username, (unsigned)_countof(username));

    if (find_admin(admin_list, username)) {
        printf("\n�û����Ѵ��ڣ�\n");
        pause_program();
        return;
    }

    // �����ֻ���
    printf("�ֻ���: ");
    scanf_s("%11s", phone, (unsigned)_countof(phone));
    while (!(phone)) {
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

    // ����ǳ�������Աִ��ע�ᣬ����ѡ��Ȩ�޼���
    if (executor && executor->privilege_level == SUPER_ADMIN) {
        printf("Ȩ�޵ȼ� (1-��ͨ����Ա, 2-��������Ա)[Ĭ��1]: ");
        char level_input[10];
        scanf_s("%9s", level_input, (unsigned)_countof(level_input));
        if (strlen(level_input) > 0) {
            privilege_level = atoi(level_input);
            if (privilege_level < NORMAL_ADMIN || privilege_level > SUPER_ADMIN) {
                privilege_level = NORMAL_ADMIN;
            }
        }
    }

    Admin* new_admin = create_admin(username, phone, password, privilege_level);
    if (!new_admin) {
        printf("\nע��ʧ�ܣ�\n");
        pause_program();
        return;
    }

    add_admin(&admin_list, new_admin);
    save_admins_to_file(admin_list, "admins.dat");
    printf("\n�ɹ�ע��%s����Ա: %s\n",
        privilege_level == SUPER_ADMIN ? "����" : "��ͨ",
        username);
    pause_program();
}

// ϵͳ��ʼ��ʱ����Ĭ�ϳ�������Ա���ڳ�������ʱ���ã�
void initialize_system() {
    // ����Ƿ����й���Ա
    if (admin_count == 0) {
        printf("��⵽ϵͳ�״����У����ڴ���Ĭ�ϳ�������Ա...\n");

        // ����Ĭ�ϳ�������Ա
        Admin* default_admin = create_admin(
            "admin",         // Ĭ���û���
            "13800000000",   // Ĭ���ֻ���
            "admin123",      // Ĭ������
            SUPER_ADMIN      // ��������ԱȨ��
        );

        if (default_admin) {
            add_admin(&admin_list, default_admin);
            save_admins_to_file(admin_list, "admins.dat");
            printf("�Ѵ���Ĭ�ϳ�������Ա�˺�:\n");
            printf("�û���: admin\n����: admin123\n");
            printf("���״ε�¼�������޸����룡\n");
            pause_program();
        }
    }
}

//ע������Ա����������Ա����ɾ���Լ����˻�����ͨ����Աֻ��ע���Լ�����������Ա����ɾ����������Ա��
bool delete_admin(Admin** head, const char* username) {
    if (!head || !username) return false;

    Admin* prev = NULL, * curr = *head;

    while (curr) {
        if (strcmp(curr->username, username) == 0) {
            if (prev) {
                prev->next = curr->next;
            }
            else {
                *head = curr->next;
            }
            free(curr);
            admin_count--;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }

    return false;
}

void admin_delete_account(Admin* current_admin) {
    clear_screen();
    printf("\n========== ����Ա�˻�ע�� ==========\n");
    printf("��ǰ�˻�: %s (%s)\n",
        current_admin->username,
        current_admin->privilege_level == SUPER_ADMIN ? "��������Ա" : "��ͨ����Ա");

    // ��ȫ���ƣ���������Ա��������ע��
    if (current_admin->privilege_level == SUPER_ADMIN) {
        printf("\n��ȫ���ƣ���������Ա��������ע����\n");
        printf("����������������Աִ�д˲�����\n");
        pause_program();
        return;
    }

    char confirm[20];
    printf("\n���棺�˲���������ɾ�����Ĺ���Ա�˻���\n");
    printf("����\"CONFIRM\"ȷ��ע��: ");
    scanf_s("%19s", confirm, (unsigned)_countof(confirm));
    clear_input_buffer();

    if (strcmp(confirm, "CONFIRM") == 0) {
        if (delete_admin(&admin_list, current_admin->username)) {
            save_admins_to_file(admin_list, "admins.dat");
            printf("\n�˻��ѳɹ�ע����ϵͳ���˳�...\n");
            pause_program();
            exit(0);
        }
        else {
            printf("\nע��ʧ�ܣ�\n");
        }
    }
    else {
        printf("\nȡ��ע��������\n");
    }
    pause_program();
}

// �޸Ĺ���Ա��Ϣ
void modify_admin_info(Admin* current_admin) {
    clear_screen();
    printf("\n========== �޸Ĺ���Ա��Ϣ ==========\n");

    // ��ʾ��ǰ��Ϣ
    printf("��ǰ��Ϣ:\n");
    printf("1. �û���: %s\n", current_admin->username);
    printf("2. �ֻ���: %s\n", current_admin->phone);
    printf("3. ����: ********\n");

    int choice;
    printf("\n��ѡ��Ҫ�޸ĵ���Ŀ (1-3, 0ȡ��): ");
    scanf_s("%d", &choice);
    clear_input_buffer();

    switch (choice) {
    case 0:
        return;

    case 1: {
        // �޸��û���
        char new_username[MAX_USERNAME_LEN];
        printf("���������û���: ");
        fgets(new_username, sizeof(new_username), stdin);
        new_username[strcspn(new_username, "\n")] = '\0'; // ȥ�����з�

        if (strlen(new_username) < 3) {
            printf("�û���������Ҫ3���ַ���\n");
            break;
        }

        if (find_admin(admin_list, new_username)) {
            printf("���û����Ѵ��ڣ�\n");
            break;
        }

        strncpy_s(current_admin->username, MAX_USERNAME_LEN, new_username, _TRUNCATE);
        printf("�û����޸ĳɹ���\n");
        break;
    }

    case 2: {
        // �޸��ֻ���
        char new_phone[MAX_PHONE_LEN];
        printf("���������ֻ���: ");
        fgets(new_phone, sizeof(new_phone), stdin);
        new_phone[strcspn(new_phone, "\n")] = '\0';

        if (!(new_phone)) {
            printf("�ֻ��Ÿ�ʽ����ȷ��\n");
            break;
        }

        strncpy_s(current_admin->phone, MAX_PHONE_LEN, new_phone, _TRUNCATE);
        printf("�ֻ����޸ĳɹ���\n");
        break;
    }

    case 3: {
        // �޸�����
        char current_pwd[MAX_PASSWORD_LEN];
        char new_pwd[MAX_PASSWORD_LEN];
        char confirm_pwd[MAX_PASSWORD_LEN];

        printf("�����뵱ǰ����: ");
        get_password_input(current_pwd, sizeof(current_pwd));

        if (strcmp(current_admin->password, current_pwd) != 0) {
            printf("��ǰ���벻��ȷ��\n");
            break;
        }

        printf("������������: ");
        get_password_input(new_pwd, sizeof(new_pwd));

        printf("ȷ��������: ");
        get_password_input(confirm_pwd, sizeof(confirm_pwd));

        if (strcmp(new_pwd, confirm_pwd) != 0) {
            printf("������������벻һ�£�\n");
            break;
        }

        if (strlen(new_pwd) < 6) {
            printf("���볤��������Ҫ6λ��\n");
            break;
        }

        strncpy_s(current_admin->password, MAX_PASSWORD_LEN, new_pwd, _TRUNCATE);
        printf("�����޸ĳɹ���\n");
        break;
    }

    default:
        printf("��Ч��ѡ��\n");
    }

    save_admins_to_file(admin_list, "admins.dat");
    pause_program();
}

// ��������Ա�޸���������Ա��Ϣ
void super_modify_admin_info(Admin* super_admin) {
    if (!is_super_admin(super_admin)) {
        printf("\nȨ�޲��㣡ֻ�г�������Ա�����޸���������Ա��Ϣ��\n");
        pause_program();
        return;
    }

    clear_screen();
    printf("\n========== �޸Ĺ���Ա��Ϣ ==========\n");

    // ��ʾ���й���Ա�б�
    printf("����Ա�б�:\n");
    Admin* curr = admin_list;
    int count = 1;
    while (curr) {
        printf("%d. %s (%s)\n", count++, curr->username,
            curr->privilege_level == SUPER_ADMIN ? "��������Ա" : "��ͨ����Ա");
        curr = curr->next;
    }

    char target_username[MAX_USERNAME_LEN];
    printf("\n������Ҫ�޸ĵĹ���Ա�û���: ");
    scanf_s("%49s", target_username, (unsigned)_countof(target_username));

    Admin* target = find_admin(admin_list, target_username);
    if (!target) {
        printf("�Ҳ���ָ������Ա��\n");
        pause_program();
        return;
    }

    if (target == super_admin) {
        printf("�����޸��Լ�����Ϣ����ʹ�ø��������޸Ĺ��ܣ�\n");
        pause_program();
        return;
    }

    clear_screen();
    printf("\n�����޸Ĺ���Ա %s ����Ϣ\n", target->username);
    printf("1. �޸��ֻ���\n");
    printf("2. �޸�����\n");
    printf("3. �޸�Ȩ�޼���\n");
    printf("0. ȡ��\n");

    int choice;
    printf("\n��ѡ�����: ");
    scanf_s("%d", &choice);
    clear_input_buffer();

    switch (choice) {
    case 0:
        return;

    case 1: {
        // �޸��ֻ���
        char new_phone[MAX_PHONE_LEN];
        printf("���������ֻ���: ");
        fgets(new_phone, sizeof(new_phone), stdin);
        new_phone[strcspn(new_phone, "\n")] = '\0';

        if (!(new_phone)) {
            printf("�ֻ��Ÿ�ʽ����ȷ��\n");
            break;
        }

        strncpy_s(target->phone, MAX_PHONE_LEN, new_phone, _TRUNCATE);
        printf("�ֻ����޸ĳɹ���\n");
        break;
    }

    case 2: {
        // �޸�����
        char new_pwd[MAX_PASSWORD_LEN];
        char confirm_pwd[MAX_PASSWORD_LEN];

        printf("������������: ");
        get_password_input(new_pwd, sizeof(new_pwd));

        printf("ȷ��������: ");
        get_password_input(confirm_pwd, sizeof(confirm_pwd));

        if (strcmp(new_pwd, confirm_pwd) != 0) {
            printf("������������벻һ�£�\n");
            break;
        }

        if (strlen(new_pwd) < 6) {
            printf("���볤��������Ҫ6λ��\n");
            break;
        }

        strncpy_s(target->password, MAX_PASSWORD_LEN, new_pwd, _TRUNCATE);
        printf("�����޸ĳɹ���\n");
        break;
    }

    case 3: {
        // �޸�Ȩ�޼���
        int new_level;
        printf("��ǰȨ��: %s\n",
            target->privilege_level == SUPER_ADMIN ? "��������Ա" : "��ͨ����Ա");
        printf("������Ȩ�� (1-��ͨ����Ա, 2-��������Ա): ");
        scanf_s("%d", &new_level);

        if (new_level != NORMAL_ADMIN && new_level != SUPER_ADMIN) {
            printf("��Ч��Ȩ�޼���\n");
            break;
        }
        target->privilege_level = (AdminPrivilegeLevel)new_level;
        printf("Ȩ�޼����޸ĳɹ���\n");
        break;
    }

    default:
        printf("��Ч��ѡ��\n");
    }

    save_admins_to_file(admin_list, "admins.dat");
    pause_program();
}

// ��ȫ��ȡ�������루��ʾ*����ʵ���ַ���
void get_password_input(char* buffer, int buffer_size) {
    int i = 0;
    char ch;

    while (1) {
        ch = _getch();  // �޸�Ϊʹ�� _getch

        if (ch == '\r' || ch == '\n') {
            printf("\n");
            buffer[i] = '\0';
            break;
        }
        else if (ch == '\b' && i > 0) {
            printf("\b \b");
            i--;
        }
        else if (isprint(ch) && i < buffer_size - 1) {
            buffer[i++] = ch;
            printf("*");
        }
    }
}


