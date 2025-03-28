#include "cainiao.h"
#include <stdlib.h> // Ϊ�� _countof ��

// ȫ�ֱ���
User* user_list = NULL;
Shelf* shelf_list = NULL;
Express* express_list = NULL;
int user_count = 0;

// ���˵�
void main_menu() {
    while (1) {
        clear_screen();
        printf("\n========== ������վ����ϵͳ ==========\n");
        printf("1. �û���¼\n");
        printf("2. �û�ע��\n");
        printf("3. ����Ա��¼\n");
        printf("4. �˳�ϵͳ\n");
        printf("��ѡ�����: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1:
            user_login();
            break;
        case 2:
            user_register();
            break;
        case 3:
            admin_login();
            break;
        case 4:
            printf("\n��лʹ�ò�����վ����ϵͳ���ټ���\n");
            exit(0);
        default:
            printf("\n��Ч��ѡ�����������룡\n");
            pause_program();
        }
    }
}

// �û��˵�
void user_menu(User* user) {
    while (1) {
        clear_screen();
        printf("\n========== �û����� ==========\n");
        printf("��ӭ��%s (%s)\n", user->username,
            user->level == USER_NORMAL ? "��ͨ�û�" :
            user->level == USER_STUDENT ? "ѧ���û�" :
            user->level == USER_VIP ? "VIP�û�" :
            user->level == USER_ENTERPRISE ? "��ҵ�û�" : "�����̻�");
        printf("1. �޸ĸ�����Ϣ\n");
        printf("2. ��ݷ���\n");
        printf("3. �鿴�˵�\n");
        printf("4. �˳���¼\n");
        printf("��ѡ�����: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1:
            modify_user_info(user);
            break;
        case 2:
            printf("\n��ݷ�������δʵ��\n");
            pause_program();
            break;
        case 3:
            printf("\n�鿴�˵�������δʵ��\n");
            pause_program();
            break;
        case 4:
            return;
        default:
            printf("\n��Ч��ѡ�����������룡\n");
            pause_program();
        }
    }
}

// ����Ա�˵�
void admin_menu(Admin* admin) {
    while (1) {
        clear_screen();
        printf("\n========== ����Ա��� ==========\n");
        printf("��ӭ��%s (%s)\n", admin->username,
            admin->privilege_level == SUPER_ADMIN ? "��������Ա" : "��ͨ����Ա");
        printf("1. �û�����\n");
        printf("2. ��ݹ���\n");
        printf("3. ���ܹ���\n");
        if (is_super_admin(admin)) {
            printf("4. ����Ա����\n");
            printf("5. ϵͳ����\n");
            printf("6. �˳���¼\n");
            printf("��ѡ�����: ");
        }
        else {
            printf("4. �˳���¼\n");
            printf("��ѡ�����: ");
        }

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1:
            printf("\n�û���������δʵ��\n");
            pause_program();
            break;
        case 2:
            printf("\n��ݹ�������δʵ��\n");
            pause_program();
            break;
        case 3:
            printf("\n���ܹ�������δʵ��\n");
            pause_program();
            break;
        case 4:
            if (is_super_admin(admin)) {
                printf("\n����Ա��������δʵ��\n");
                pause_program();
            }
            else {
                return;
            }
            break;
        case 5:
            if (is_super_admin(admin)) {
                printf("\nϵͳ���ù�����δʵ��\n");
                pause_program();
            }
            else {
                printf("\n��Ч��ѡ�����������룡\n");
                pause_program();
            }
            break;
        case 6:
            if (is_super_admin(admin)) {
                return;
            }
            else {
                printf("\n��Ч��ѡ�����������룡\n");
                pause_program();
            }
            break;
        default:
            printf("\n��Ч��ѡ�����������룡\n");
            pause_program();
        }
    }
}

// ��������ʵ��


void pause_program() {
    printf("\n�����������...");
    getchar();
}

void clear_screen() {
    system("cls || clear");
}



Express* find_express_by_tracking(Express* head, const char* tracking_num) {
    while (head) {
        if (strcmp(head->tracking_num, tracking_num) == 0) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

// ������
int main() {
    // ��ʼ��ϵͳ
    initialize_system();

    // ��������
    user_list = load_users_from_file("users.dat");
    admin_list = load_admins_from_file("admins.dat");
    shelf_list = load_shelves_from_file("shelves.dat");

    // ͳ���û�����
    User* curr = user_list;
    while (curr) {
        user_count++;
        curr = curr->next;
    }

    // ��ʾ���˵�
    main_menu();

    // �����˳�ǰ��������
    save_users_to_file(user_list, "users.dat");
    save_admins_to_file(admin_list, "admins.dat");
    save_shelves_to_file(shelf_list, "shelves.dat");

    return 0;
}