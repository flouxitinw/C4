#include "cainiao.h"
#include <ctype.h>
#define _CRT_SECURE_NO_WARNINGS // 禁用安全警告

// 全局变量定义
Admin* admin_list = NULL;
int admin_count = 0;

// 清空输入缓冲区
void clear_input_buffer() {
    while (getchar() != '\n');
}

// 创建管理员
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

// 添加管理员（头插法）
void add_admin(Admin** head, Admin* new_admin) {
    if (!new_admin) return;
    new_admin->next = *head;
    *head = new_admin;
    admin_count++;
}

// 查找管理员
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

// 更新管理员密码
void update_admin_password(Admin* admin, const char* new_password) {
    if (!admin || !new_password) return;
    strncpy_s(admin->password, MAX_PASSWORD_LEN, new_password, _TRUNCATE);
    admin->password[MAX_PASSWORD_LEN - 1] = '\0';
}



// 保存管理员数据到文件
void save_admins_to_file(Admin* head, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to save admins");
        return;
    }

    // 写入列标题
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

// 从文件加载管理员数据（尾插法）
Admin* load_admins_from_file(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return NULL;

    Admin* head = NULL, * tail = NULL;
    char line[256];
    fgets(line, sizeof(line), fp); // 跳过表头

    while (fgets(line, sizeof(line), fp)) {
        char username[MAX_USERNAME_LEN], phone[MAX_PHONE_LEN], password[MAX_PASSWORD_LEN];
        int privilege_level;

        if (sscanf_s(line, "%49[^,],%11[^,],%49[^,],%d",
            username, (unsigned)_countof(username),
            phone, (unsigned)_countof(phone),
            password, (unsigned)_countof(password),
            &privilege_level) != 4) {
            continue; // 跳过格式错误行
        }

        // 去除可能的引号
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

// 修改管理员权限级别
void admin_change_privilege(Admin* executor, const char* target_username, int new_level) {
    if (!executor || executor->privilege_level != SUPER_ADMIN) {
        printf("\n权限不足！只有超级管理员可以修改权限级别。\n");
        return;
    }

    if (new_level != NORMAL_ADMIN && new_level != SUPER_ADMIN) {
        printf("\n无效的权限级别！\n");
        return;
    }

    Admin* target = find_admin(admin_list, target_username);
    if (!target) {
        printf("\n找不到指定管理员！\n");
        return;
    }

    if (target == executor) {
        printf("\n不能修改自己的权限级别！\n");
        return;
    }
    target->privilege_level = (AdminPrivilegeLevel)new_level;
    save_admins_to_file(admin_list, "admins.dat");
    printf("\n已成功修改 %s 的权限为%s管理员\n",
        target_username,
        new_level == SUPER_ADMIN ? "超级" : "普通");
}

// 检查是否是超级管理员
bool is_super_admin(const Admin* admin) {
    return admin && admin->privilege_level == SUPER_ADMIN;
}

// 权限验证
bool has_permission(const Admin* admin, int required_level) {
    if (!admin) return false;
    return admin->privilege_level >= required_level;
}

// 管理员登录（支持用户名或手机号登录）
void admin_login() {
    int attempts = 3;

    while (attempts > 0) {
        clear_screen();
        printf("\n========== 管理员登录 ==========\n");
        printf("剩余尝试次数: %d\n", attempts);

        char input[50], password[50];
        printf("用户名或手机号: ");
        scanf_s("%49s", input, (unsigned)_countof(input));
        printf("密码: ");
        scanf_s("%49s", password, (unsigned)_countof(password));

        Admin* admin = NULL;
        Admin* curr = admin_list;

        // 遍历查找匹配用户名或手机号的管理员
        while (curr) {
            if (strcmp(curr->username, input) == 0 ||
                strcmp(curr->phone, input) == 0) {
                admin = curr;
                break;
            }
            curr = curr->next;
        }

        // 验证登录
        if (!admin || strcmp(admin->password, password) != 0) {
            attempts--;
            printf("\n用户名/手机号或密码错误！");
            if (attempts > 0) {
                printf(" 请重试。\n");
            }
            else {
                printf("\n尝试次数已用完，系统将退出。\n");
                exit(0);
            }
            pause_program();
            continue;
        }

        // 显示权限级别
        printf("\n登录成功！欢迎回来，%s %s\n",
            admin->privilege_level == SUPER_ADMIN ? "[超级管理员]" : "[普通管理员]",
            admin->username);
        pause_program();

        admin_menu(admin);
        return;
    }
}

// 管理员注册
void admin_register(Admin* executor) {
    // 权限检查
    if (executor && executor->privilege_level != SUPER_ADMIN) {
        printf("\n只有超级管理员可以注册新管理员！\n");
        pause_program();
        return;
    }

    clear_screen();
    printf("\n========== 管理员注册 ==========\n");

    char username[MAX_USERNAME_LEN], phone[MAX_PHONE_LEN], password[MAX_PASSWORD_LEN], confirm_pwd[MAX_PASSWORD_LEN];
    int privilege_level = NORMAL_ADMIN;  // 默认注册普通管理员

    // 输入用户名
    printf("用户名: ");
    scanf_s("%49s", username, (unsigned)_countof(username));

    if (find_admin(admin_list, username)) {
        printf("\n用户名已存在！\n");
        pause_program();
        return;
    }

    // 输入手机号
    printf("手机号: ");
    scanf_s("%11s", phone, (unsigned)_countof(phone));
    while (!(phone)) {
        printf("手机号格式不正确！请重新输入: ");
        scanf_s("%11s", phone, (unsigned)_countof(phone));
    }

    // 密码输入及确认
    do {
        printf("密码: ");
        scanf_s("%49s", password, (unsigned)_countof(password));
        printf("确认密码: ");
        scanf_s("%49s", confirm_pwd, (unsigned)_countof(confirm_pwd));

        if (strcmp(password, confirm_pwd) != 0) {
            printf("\n两次输入的密码不一致！请重新输入。\n\n");
        }
    } while (strcmp(password, confirm_pwd) != 0);

    // 如果是超级管理员执行注册，可以选择权限级别
    if (executor && executor->privilege_level == SUPER_ADMIN) {
        printf("权限等级 (1-普通管理员, 2-超级管理员)[默认1]: ");
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
        printf("\n注册失败！\n");
        pause_program();
        return;
    }

    add_admin(&admin_list, new_admin);
    save_admins_to_file(admin_list, "admins.dat");
    printf("\n成功注册%s管理员: %s\n",
        privilege_level == SUPER_ADMIN ? "超级" : "普通",
        username);
    pause_program();
}

// 系统初始化时创建默认超级管理员（在程序启动时调用）
void initialize_system() {
    // 检查是否已有管理员
    if (admin_count == 0) {
        printf("检测到系统首次运行，正在创建默认超级管理员...\n");

        // 创建默认超级管理员
        Admin* default_admin = create_admin(
            "admin",         // 默认用户名
            "13800000000",   // 默认手机号
            "admin123",      // 默认密码
            SUPER_ADMIN      // 超级管理员权限
        );

        if (default_admin) {
            add_admin(&admin_list, default_admin);
            save_admins_to_file(admin_list, "admins.dat");
            printf("已创建默认超级管理员账号:\n");
            printf("用户名: admin\n密码: admin123\n");
            printf("请首次登录后立即修改密码！\n");
            pause_program();
        }
    }
}

//注销管理员（超级管理员不能删除自己的账户，普通管理员只能注销自己，超级管理员可以删除其他管理员）
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
    printf("\n========== 管理员账户注销 ==========\n");
    printf("当前账户: %s (%s)\n",
        current_admin->username,
        current_admin->privilege_level == SUPER_ADMIN ? "超级管理员" : "普通管理员");

    // 安全限制：超级管理员不能自我注销
    if (current_admin->privilege_level == SUPER_ADMIN) {
        printf("\n安全限制：超级管理员不能自我注销！\n");
        printf("请让其他超级管理员执行此操作。\n");
        pause_program();
        return;
    }

    char confirm[20];
    printf("\n警告：此操作将永久删除您的管理员账户！\n");
    printf("输入\"CONFIRM\"确认注销: ");
    scanf_s("%19s", confirm, (unsigned)_countof(confirm));
    clear_input_buffer();

    if (strcmp(confirm, "CONFIRM") == 0) {
        if (delete_admin(&admin_list, current_admin->username)) {
            save_admins_to_file(admin_list, "admins.dat");
            printf("\n账户已成功注销！系统将退出...\n");
            pause_program();
            exit(0);
        }
        else {
            printf("\n注销失败！\n");
        }
    }
    else {
        printf("\n取消注销操作。\n");
    }
    pause_program();
}

// 修改管理员信息
void modify_admin_info(Admin* current_admin) {
    clear_screen();
    printf("\n========== 修改管理员信息 ==========\n");

    // 显示当前信息
    printf("当前信息:\n");
    printf("1. 用户名: %s\n", current_admin->username);
    printf("2. 手机号: %s\n", current_admin->phone);
    printf("3. 密码: ********\n");

    int choice;
    printf("\n请选择要修改的项目 (1-3, 0取消): ");
    scanf_s("%d", &choice);
    clear_input_buffer();

    switch (choice) {
    case 0:
        return;

    case 1: {
        // 修改用户名
        char new_username[MAX_USERNAME_LEN];
        printf("请输入新用户名: ");
        fgets(new_username, sizeof(new_username), stdin);
        new_username[strcspn(new_username, "\n")] = '\0'; // 去除换行符

        if (strlen(new_username) < 3) {
            printf("用户名至少需要3个字符！\n");
            break;
        }

        if (find_admin(admin_list, new_username)) {
            printf("该用户名已存在！\n");
            break;
        }

        strncpy_s(current_admin->username, MAX_USERNAME_LEN, new_username, _TRUNCATE);
        printf("用户名修改成功！\n");
        break;
    }

    case 2: {
        // 修改手机号
        char new_phone[MAX_PHONE_LEN];
        printf("请输入新手机号: ");
        fgets(new_phone, sizeof(new_phone), stdin);
        new_phone[strcspn(new_phone, "\n")] = '\0';

        if (!(new_phone)) {
            printf("手机号格式不正确！\n");
            break;
        }

        strncpy_s(current_admin->phone, MAX_PHONE_LEN, new_phone, _TRUNCATE);
        printf("手机号修改成功！\n");
        break;
    }

    case 3: {
        // 修改密码
        char current_pwd[MAX_PASSWORD_LEN];
        char new_pwd[MAX_PASSWORD_LEN];
        char confirm_pwd[MAX_PASSWORD_LEN];

        printf("请输入当前密码: ");
        get_password_input(current_pwd, sizeof(current_pwd));

        if (strcmp(current_admin->password, current_pwd) != 0) {
            printf("当前密码不正确！\n");
            break;
        }

        printf("请输入新密码: ");
        get_password_input(new_pwd, sizeof(new_pwd));

        printf("确认新密码: ");
        get_password_input(confirm_pwd, sizeof(confirm_pwd));

        if (strcmp(new_pwd, confirm_pwd) != 0) {
            printf("两次输入的密码不一致！\n");
            break;
        }

        if (strlen(new_pwd) < 6) {
            printf("密码长度至少需要6位！\n");
            break;
        }

        strncpy_s(current_admin->password, MAX_PASSWORD_LEN, new_pwd, _TRUNCATE);
        printf("密码修改成功！\n");
        break;
    }

    default:
        printf("无效的选择！\n");
    }

    save_admins_to_file(admin_list, "admins.dat");
    pause_program();
}

// 超级管理员修改其他管理员信息
void super_modify_admin_info(Admin* super_admin) {
    if (!is_super_admin(super_admin)) {
        printf("\n权限不足！只有超级管理员可以修改其他管理员信息。\n");
        pause_program();
        return;
    }

    clear_screen();
    printf("\n========== 修改管理员信息 ==========\n");

    // 显示所有管理员列表
    printf("管理员列表:\n");
    Admin* curr = admin_list;
    int count = 1;
    while (curr) {
        printf("%d. %s (%s)\n", count++, curr->username,
            curr->privilege_level == SUPER_ADMIN ? "超级管理员" : "普通管理员");
        curr = curr->next;
    }

    char target_username[MAX_USERNAME_LEN];
    printf("\n请输入要修改的管理员用户名: ");
    scanf_s("%49s", target_username, (unsigned)_countof(target_username));

    Admin* target = find_admin(admin_list, target_username);
    if (!target) {
        printf("找不到指定管理员！\n");
        pause_program();
        return;
    }

    if (target == super_admin) {
        printf("不能修改自己的信息，请使用个人资料修改功能！\n");
        pause_program();
        return;
    }

    clear_screen();
    printf("\n正在修改管理员 %s 的信息\n", target->username);
    printf("1. 修改手机号\n");
    printf("2. 修改密码\n");
    printf("3. 修改权限级别\n");
    printf("0. 取消\n");

    int choice;
    printf("\n请选择操作: ");
    scanf_s("%d", &choice);
    clear_input_buffer();

    switch (choice) {
    case 0:
        return;

    case 1: {
        // 修改手机号
        char new_phone[MAX_PHONE_LEN];
        printf("请输入新手机号: ");
        fgets(new_phone, sizeof(new_phone), stdin);
        new_phone[strcspn(new_phone, "\n")] = '\0';

        if (!(new_phone)) {
            printf("手机号格式不正确！\n");
            break;
        }

        strncpy_s(target->phone, MAX_PHONE_LEN, new_phone, _TRUNCATE);
        printf("手机号修改成功！\n");
        break;
    }

    case 2: {
        // 修改密码
        char new_pwd[MAX_PASSWORD_LEN];
        char confirm_pwd[MAX_PASSWORD_LEN];

        printf("请输入新密码: ");
        get_password_input(new_pwd, sizeof(new_pwd));

        printf("确认新密码: ");
        get_password_input(confirm_pwd, sizeof(confirm_pwd));

        if (strcmp(new_pwd, confirm_pwd) != 0) {
            printf("两次输入的密码不一致！\n");
            break;
        }

        if (strlen(new_pwd) < 6) {
            printf("密码长度至少需要6位！\n");
            break;
        }

        strncpy_s(target->password, MAX_PASSWORD_LEN, new_pwd, _TRUNCATE);
        printf("密码修改成功！\n");
        break;
    }

    case 3: {
        // 修改权限级别
        int new_level;
        printf("当前权限: %s\n",
            target->privilege_level == SUPER_ADMIN ? "超级管理员" : "普通管理员");
        printf("设置新权限 (1-普通管理员, 2-超级管理员): ");
        scanf_s("%d", &new_level);

        if (new_level != NORMAL_ADMIN && new_level != SUPER_ADMIN) {
            printf("无效的权限级别！\n");
            break;
        }
        target->privilege_level = (AdminPrivilegeLevel)new_level;
        printf("权限级别修改成功！\n");
        break;
    }

    default:
        printf("无效的选择！\n");
    }

    save_admins_to_file(admin_list, "admins.dat");
    pause_program();
}

// 安全获取密码输入（显示*代替实际字符）
void get_password_input(char* buffer, int buffer_size) {
    int i = 0;
    char ch;

    while (1) {
        ch = _getch();  // 修改为使用 _getch

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


