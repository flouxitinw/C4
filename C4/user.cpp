#include "cainiao.h"

// 初始化用户特权
void init_user_privilege(User* user) {
    switch (user->level) {
    case USER_NORMAL:
        user->discount_rate = 1.0f;    // 100% 折扣率
        user->payment_mode = PAY_CASH; // 现付
        break;

    case USER_STUDENT:
        user->discount_rate = 0.85f;   // 85% 折扣率
        user->payment_mode = PAY_CASH; // 现付
        break;

    case USER_VIP:
        user->discount_rate = 0.9f;    // 90% 折扣率
        user->payment_mode = PAY_MONTHLY;//月结
        break;

    case USER_ENTERPRISE:
        user->discount_rate = 0.75f;   // 75% 折扣率
        user->payment_mode = PAY_MONTHLY; // 月结
        break;

    case USER_PARTNER:
        user->discount_rate = 0.6f;    // 协议价基准60%
        user->payment_mode = PAY_CONTRACT; // 周期结算
        break;
    }

    // 统一初始化其他字段
    user->free_quota = 0;
    user->priority_access = 0;
}

// 创建新用户
User* create_user(const char* phone, const char* pwd, const char* name, UserLevel level) {
    // 分配内存
    User* new_user = (User*)malloc(sizeof(User));
    if (!new_user) {
        perror("Failed to create user");
        return NULL;
    }

    // 基础信息
    strncpy_s(new_user->phone, sizeof(new_user->phone), phone, 11);
    new_user->phone[11] = '\0';
    strncpy_s(new_user->password, sizeof(new_user->password), pwd, 49);
    new_user->password[49] = '\0';
    strncpy_s(new_user->username, sizeof(new_user->username), name, 49);
    new_user->username[49] = '\0';
    new_user->level = level;

    // 初始化特权
    init_user_privilege(new_user);
    new_user->next = NULL;

    return new_user;
}

//添加用户（头插法）
void add_user(User** head, User* new_user) {
    if (!new_user) return;
    new_user->next = *head;
    *head = new_user;
}

//寻找用户（区分大小写）
User* find_user(User* head, const char* username) {
    //检查 username 是否为 NULL
    if (!username) return NULL;
    User* curr = head;
    while (curr) {
        if (strcmp(curr->username, username) == 0)//字符串完全匹配
            return curr;
        curr = curr->next;
    }
    return NULL;
}

//更新密码
void update_user_password(User* user, const char* new_password) {
    if (!user || !new_password) return;
    strncpy_s(user->password, sizeof(user->password), new_password, 49);
    user->password[49] = '\0';
}

// 验证手机号格式
bool validate_phone(const char* phone) {
    if (strlen(phone) != 11) return false;

    for (int i = 0; i < 11; i++) {
        if (!isdigit(phone[i])) return false;
    }

    return true;
}

// 保存用户信息到文件
void save_users_to_file(User* head, const char* filename) {
    //写入模式打开文件
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to save users");
        return;
    }

    // 写入列标题
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

// 从文件加载用户信息（尾插法）
User* load_users_from_file(const char* filename) {
    //只读模式打开文件
    FILE* fp = fopen(filename, "r");
    if (!fp) return NULL;

    User* head = NULL, * tail = NULL;
    char line[256];

    // 读取并跳过表头
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        char username[50], phone[12], password[50];
        int level, payment_mode;
        float discount_rate;

        // 解析每行数据
        if (sscanf_s(line, "%49[^,],%11[^,],%49[^,],%d,%d,%f",
            username, (unsigned)_countof(username),
            phone, (unsigned)_countof(phone),
            password, (unsigned)_countof(password),
            &level, &payment_mode, &discount_rate) != 6) {
            continue; // 跳过格式不正确的行
        }

        // 创建用户
        User* user = (User*)malloc(sizeof(User));
        if (!user) continue;

        // 基本信息
        strncpy_s(user->username, sizeof(user->username), username, 49);
        user->username[49] = '\0';
        strncpy_s(user->phone, sizeof(user->phone), phone, 11);
        user->phone[11] = '\0';
        strncpy_s(user->password, sizeof(user->password), password, 49);
        user->password[49] = '\0';

        // 用户等级和特权
        user->level = (UserLevel)level;
        user->payment_mode = (PaymentMode)payment_mode;
        user->discount_rate = discount_rate;
        user->free_quota = 0;
        user->priority_access = 0;
        user->next = NULL;

        // 添加到链表
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

// 注销用户
bool delete_user(User** head, const char* username) {
    if (!head || !username) return false;

    User* prev = NULL, * curr = *head;

    // 查找要删除的用户
    while (curr) {
        if (strcmp(curr->username, username) == 0) {
            // 找到用户，开始删除
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

    return false; // 未找到用户
}

// 用户登录
void user_login() {
    int attempts = 3;  // 剩余尝试次数

    while (attempts > 0) {
        clear_screen();
        printf("\n========== 用户登录 ==========\n");
        printf("剩余尝试次数: %d\n", attempts);

        char input[50], password[50];
        printf("用户名或手机号: ");
        scanf_s("%49s", input, (unsigned)_countof(input));
        printf("密码: ");
        scanf_s("%49s", password, (unsigned)_countof(password));

        // 查找用户（按用户名或手机号）
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
            printf("\n用户名/手机号或密码错误！");
            if (attempts > 0) {
                printf(" 请重试。\n");
            }
            else {
                printf("\n尝试次数已用完，系统将退出。\n");
                exit(0);  // 退出系统
            }
            pause_program();
            continue;
        }

        printf("\n登录成功！欢迎回来，%s\n", user->username);
        pause_program();
        user_menu(user);
        return;
    }
}

// 修改后的用户注册函数
void user_register() {
    clear_screen();
    printf("\n========== 用户注册 ==========\n");

    char username[50], phone[12], password[50], confirm_pwd[50];
    int user_type;

    // 输入用户名
    printf("用户名: ");
    scanf_s("%49s", username, (unsigned)_countof(username));

    // 检查用户名是否已存在
    if (find_user(user_list, username)) {
        printf("\n用户名已存在！\n");
        pause_program();
        return;
    }

    // 输入手机号
    printf("手机号: ");
    scanf_s("%11s", phone, (unsigned)_countof(phone));
    while (!validate_phone(phone)) {
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

    // 选择用户类型
    printf("\n请选择用户类型:\n");
    printf("1. 普通用户\n");
    printf("2. 学生用户\n");
    printf("3. VIP用户\n");
    printf("4. 企业用户\n");
    printf("5. 合作商户\n");
    printf("请输入选择(1-5): ");

    while (1) {
        scanf_s("%d", &user_type);
        if (user_type >= 1 && user_type <= 5) break;
        printf("无效选择，请重新输入(1-5): ");
    }

    // 创建用户
    User* new_user = create_user(phone, password, username, (UserLevel)(user_type - 1));
    if (!new_user) {
        printf("\n注册失败！\n");
        pause_program();
        return;
    }

    // 保存用户数据
    add_user(&user_list, new_user);
    user_count++;
    save_users_to_file(user_list, "users.dat");

    printf("\n注册成功！欢迎加入菜鸟驿站，%s\n", username);
    printf("您的账户类型: ");
    switch (new_user->level) {
    case USER_NORMAL:    printf("普通用户"); break;
    case USER_STUDENT:  printf("学生用户(85折)"); break;
    case USER_VIP:      printf("VIP用户(9折)"); break;
    case USER_ENTERPRISE: printf("企业用户(75折)"); break;
    case USER_PARTNER:  printf("合作商户(协议价)"); break;
    }
    printf("\n");

    pause_program();
}

//修改用户信息
bool modify_user_info(User* user) {
    if (!user) return false;

    clear_screen();
    printf("\n========== 修改用户信息 ==========\n");
    printf("当前信息：\n");
    printf("1. 用户名: %s\n", user->username);
    printf("2. 手机号: %s\n", user->phone);
    printf("3. 密码: ******\n");
    printf("0. 返回\n");
    printf("请选择要修改的项目: ");

    int choice;
    scanf_s("%d", &choice);
    clear_input_buffer();

    switch (choice) {
    case 1: { // 修改用户名
        char new_username[50];
        printf("输入新用户名: ");
        scanf_s("%49s", new_username, (unsigned)_countof(new_username));

        // 检查用户名是否已存在
        if (find_user(user_list, new_username)) {
            printf("用户名已存在！\n");
            pause_program();
            return false;
        }

        strncpy_s(user->username, sizeof(user->username), new_username, 49);
        user->username[49] = '\0';
        printf("用户名修改成功！\n");
        break;
    }

    case 2: { // 修改手机号
        char new_phone[12];
        printf("输入新手机号: ");
        scanf_s("%11s", new_phone, (unsigned)_countof(new_phone));

        if (!validate_phone(new_phone)) {
            printf("手机号格式不正确！\n");
            pause_program();
            return false;
        }

        strncpy_s(user->phone, sizeof(user->phone), new_phone, 11);
        user->phone[11] = '\0';
        printf("手机号修改成功！\n");
        break;
    }

    case 3: { // 修改密码
        char new_password[50], confirm_pwd[50];
        printf("输入新密码: ");
        scanf_s("%49s", new_password, (unsigned)_countof(new_password));
        printf("确认新密码: ");
        scanf_s("%49s", confirm_pwd, (unsigned)_countof(confirm_pwd));

        if (strcmp(new_password, confirm_pwd) != 0) {
            printf("两次输入的密码不一致！\n");
            pause_program();
            return false;
        }

        strncpy_s(user->password, sizeof(user->password), new_password, 49);
        user->password[49] = '\0';
        printf("密码修改成功！\n");
        break;
    }

    case 0:
        return false;

    default:
        printf("无效的选择！\n");
        pause_program();
        return false;
    }

    // 保存修改到文件
    save_users_to_file(user_list, "users.dat");
    pause_program();
    return true;
}

// 根据用户等级计算费用
float calculate_fee(User* user, float base_price) {
    // 合作商户有额外协议折扣
    if (user->level == USER_PARTNER) {
        return base_price * user->discount_rate * 0.9f; // 协议价再打9折
    }
    return base_price * user->discount_rate;
}

// 处理用户支付
void process_payment(User* user, float amount) {
    printf("用户[%s] 等级[%d] 应付款: ?%.2f\n",
        user->username, user->level, amount);

    switch (user->payment_mode) {
    case PAY_CASH:
        printf("请现场支付: ?%.2f\n", amount);
        break;

    case PAY_MONTHLY:
        printf("已计入月结账户 (本月累计: ?%.2f)\n", amount);
        break;

    case PAY_CONTRACT:
        printf("将按协议周期结算 (金额: ?%.2f)\n", amount);
        break;
    }
}