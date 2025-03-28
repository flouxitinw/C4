#include "cainiao.h"
#include <stdlib.h> // 为了 _countof 宏

// 全局变量
User* user_list = NULL;
Shelf* shelf_list = NULL;
Express* express_list = NULL;
int user_count = 0;

// 主菜单
void main_menu() {
    while (1) {
        clear_screen();
        printf("\n========== 菜鸟驿站管理系统 ==========\n");
        printf("1. 用户登录\n");
        printf("2. 用户注册\n");
        printf("3. 管理员登录\n");
        printf("4. 退出系统\n");
        printf("请选择操作: ");

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
            printf("\n感谢使用菜鸟驿站管理系统，再见！\n");
            exit(0);
        default:
            printf("\n无效的选择，请重新输入！\n");
            pause_program();
        }
    }
}

// 用户菜单
void user_menu(User* user) {
    while (1) {
        clear_screen();
        printf("\n========== 用户中心 ==========\n");
        printf("欢迎，%s (%s)\n", user->username,
            user->level == USER_NORMAL ? "普通用户" :
            user->level == USER_STUDENT ? "学生用户" :
            user->level == USER_VIP ? "VIP用户" :
            user->level == USER_ENTERPRISE ? "企业用户" : "合作商户");
        printf("1. 修改个人信息\n");
        printf("2. 快递服务\n");
        printf("3. 查看账单\n");
        printf("4. 退出登录\n");
        printf("请选择操作: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1:
            modify_user_info(user);
            break;
        case 2:
            printf("\n快递服务功能暂未实现\n");
            pause_program();
            break;
        case 3:
            printf("\n查看账单功能暂未实现\n");
            pause_program();
            break;
        case 4:
            return;
        default:
            printf("\n无效的选择，请重新输入！\n");
            pause_program();
        }
    }
}

// 管理员菜单
void admin_menu(Admin* admin) {
    while (1) {
        clear_screen();
        printf("\n========== 管理员面板 ==========\n");
        printf("欢迎，%s (%s)\n", admin->username,
            admin->privilege_level == SUPER_ADMIN ? "超级管理员" : "普通管理员");
        printf("1. 用户管理\n");
        printf("2. 快递管理\n");
        printf("3. 货架管理\n");
        if (is_super_admin(admin)) {
            printf("4. 管理员管理\n");
            printf("5. 系统设置\n");
            printf("6. 退出登录\n");
            printf("请选择操作: ");
        }
        else {
            printf("4. 退出登录\n");
            printf("请选择操作: ");
        }

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1:
            printf("\n用户管理功能暂未实现\n");
            pause_program();
            break;
        case 2:
            printf("\n快递管理功能暂未实现\n");
            pause_program();
            break;
        case 3:
            printf("\n货架管理功能暂未实现\n");
            pause_program();
            break;
        case 4:
            if (is_super_admin(admin)) {
                printf("\n管理员管理功能暂未实现\n");
                pause_program();
            }
            else {
                return;
            }
            break;
        case 5:
            if (is_super_admin(admin)) {
                printf("\n系统设置功能暂未实现\n");
                pause_program();
            }
            else {
                printf("\n无效的选择，请重新输入！\n");
                pause_program();
            }
            break;
        case 6:
            if (is_super_admin(admin)) {
                return;
            }
            else {
                printf("\n无效的选择，请重新输入！\n");
                pause_program();
            }
            break;
        default:
            printf("\n无效的选择，请重新输入！\n");
            pause_program();
        }
    }
}

// 辅助函数实现


void pause_program() {
    printf("\n按任意键继续...");
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

// 主函数
int main() {
    // 初始化系统
    initialize_system();

    // 加载数据
    user_list = load_users_from_file("users.dat");
    admin_list = load_admins_from_file("admins.dat");
    shelf_list = load_shelves_from_file("shelves.dat");

    // 统计用户数量
    User* curr = user_list;
    while (curr) {
        user_count++;
        curr = curr->next;
    }

    // 显示主菜单
    main_menu();

    // 程序退出前保存数据
    save_users_to_file(user_list, "users.dat");
    save_admins_to_file(admin_list, "admins.dat");
    save_shelves_to_file(shelf_list, "shelves.dat");

    return 0;
}