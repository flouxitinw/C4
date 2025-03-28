#pragma once
// cainiao.h
#ifndef CAINIAO_H
#define CAINIAO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include<math.h>
#include <ctype.h> 
#include <conio.h> // 用于 getch()
#include <stdbool.h>
//--------------------用户--------------------
// 用户等级枚举
typedef enum {
    USER_NORMAL,    // 普通用户
    USER_STUDENT,   // 学生用户
    USER_VIP,       // VIP用户
    USER_ENTERPRISE,// 企业用户
    USER_PARTNER    // 合作商户
} UserLevel;

// 支付方式枚举
typedef enum {
    PAY_CASH,       // 现付
    PAY_MONTHLY,    // 月结
    PAY_CONTRACT    // 周期结算
} PaymentMode;

// 用户结构体
typedef struct User {
    char username[50];
    char phone[12];
    char password[50];
    UserLevel level;
    PaymentMode payment_mode;
    float discount_rate;
    int free_quota;
    int priority_access;
    struct User* next;
} User;
//--------------------管理员--------------------
// 管理员权限级别枚举
typedef enum {
    NORMAL_ADMIN = 1,
    SUPER_ADMIN = 2
} AdminPrivilegeLevel;

// 管理员结构体
typedef struct Admin {
    char username[50];
    char phone[12];
    char password[50];
    AdminPrivilegeLevel privilege_level;
    struct Admin* next;
} Admin;


//--------------------快递--------------------
// 快递状态枚举
// 快递状态枚举
typedef enum {
    STATUS_UNSENT,      // 未寄出
    STATUS_SENT,        // 已寄出
    STATUS_UNPICKED,    // 未取件
    STATUS_PICKED,      // 已取件
    STATUS_OVERDUE,     // 滞留
    STATUS_ABNORMAL,    // 异常
    STATUS_ABNORMAL_RESOLVED // 异常已处理
} PackageStatus;

// 包裹类型枚举
typedef enum {
    DAILY_GOODS,    // 日用品
    FOOD,           // 食品
    DOCUMENT,       // 文件
    CLOTHING,       // 衣物
    DIGITAL,        // 数码产品
    ALCOHOL,        // 酒类（不可邮寄）
    ANIMAL,         // 动物（不可邮寄）
    WEAPON,         // 武器（不可邮寄）
    HAZARDOUS,      // 易燃/易爆/有毒物品（不可邮寄）
    VALUABLES,      // 贵重物品
    FRESH,          // 生鲜
    FRAGILE,        // 易碎品
    OTHER,          // 其他
    BANNED_ITEMS    // 不可邮寄物品标记（内部使用）
} PackageType;

// 运输方式枚举
typedef enum {
    SHIPPING_NORMAL = 0,   // 普通快递
    SHIPPING_BULK,         // 大件邮寄
    SHIPPING_COLD_CHAIN    // 冷链邮寄
} ShippingMethod;

// 地区类型枚举
typedef enum {
    REGION_PROVINCE = 0,  // 省内
    REGION_OUTOFPROVINCE,    // 省外
    REGION_INTERNATIONAL, // 国际
    REGION_HONGKONG_MACAO // 港澳台
} RegionType;
// 运输时效枚举
typedef enum {
    STANDARD_DELIVERY = 0,  // 标快（默认）
    EXPRESS_DELIVERY        // 特快
} DeliveryType;
typedef struct {
    double base_weight_fee;         // 首重重量(kg)
    double additional_weight_fee;   // 续重单价(元/kg)
} RateConfig;


// 快递信息结构体
typedef struct Express {
    char sender_name[50];
    char sender_phone[12];
    char receiver_name[50];
    char receiver_phone[12];
    double weight;         // 实际重量(kg)
    double volume;         // 体积(cm³)
    PackageType package_type;
    ShippingMethod method;
    RegionType region;
    int is_insured;       // 是否保价(0/1)
    double insured_value;  // 物品价值
    DeliveryType express_type;
    double fee;            // 运费
    PackageStatus status;      // 快递状态
    time_t create_time;   // 创建时间
    char pickup_code[15];       // 6位取件码+'\0'
    char tracking_num[20];	 // 快递单号
    char shelf_id[10];	 // 货架号
    time_t storage_time;   // 入库时间
    struct Express* next;
} Express;
typedef struct ExpressNode {
    Express data;
    struct ExpressNode* next;
} ExpressNode;
static ExpressNode* create_express_node(Express* data) {
    ExpressNode* node = (ExpressNode*)malloc(sizeof(ExpressNode));
    if (node) {
        node->data = *data;
        node->next = NULL;
    }
    return node;
}

// 货架结构体
typedef struct Shelf {
    char shelf_id[10];
    int max_capacity;
    int current_count;
    ExpressNode* express_list;
    struct Shelf* next;
} Shelf;

// 现在可以安全地声明全局变量
extern User* user_list;      // 用户链表头指针
extern int user_count;       // 用户数量
extern Admin* admin_list;    // 管理员链表头指针
extern int admin_count;      // 管理员数量

// 定义常量
#define MAX_USERNAME_LEN 50
#define MAX_PHONE_LEN 12
#define MAX_PASSWORD_LEN 50

// --------------------寄快递流程函数--------------------
const char* get_package_type_name(PackageType type);//获取物品类型名称
int is_valid_phone(const char* phone);// 检查是否是有效的 11 位手机号
int input_int_in_range(const char* prompt, int min, int max);// 获取用户输入范围内的整数
double input_positive_double(const char* prompt);// 输入一个正浮点数（必须 > 0）
void send_express(User* user_head, ExpressNode** express_head);//寄快递核心函数
// --------------------运费计算函数--------------------
int get_throw_ratio(ShippingMethod method, RegionType region);// 获取抛比系数
double calculate_insurance(double insured_value, PackageType type);// 保价费用计算
RateConfig get_base_rates(ShippingMethod method, RegionType region, PackageType type, DeliveryType express_type);//获取首重续重函数
double calculate_shipping_fee(Express* express);//运费计算核心函数
void send_express(User* user_head, ExpressNode** express_head);
// --------------------取快递流程函数--------------------
void pickup_express(User* current_user, ExpressNode** express_head);

// --------------------用户管理函数--------------------

void init_user_privilege(User* user);// 初始化用户特权
User* create_user(const char* phone, const char* pwd, const char* name, UserLevel level);// 创建新用户
User* find_user(User* head, const char* username);//寻找用户（区分大小写）
void update_user_password(User* user, const char* new_password);//更新密码
bool validate_phone(const char* phone);// 验证手机号格式
void save_users_to_file(User* head, const char* filename);// 保存用户信息到文件
User* load_users_from_file(const char* filename);// 从文件加载用户信息（尾插法
bool delete_user(User** head, const char* username);// 注销用户


void user_login();// 用户登录
void user_register();// 修改后的用户注册函数
bool modify_user_info(User* user);//修改用户信息

void user_menu(User* user);

// --------------------管理员管理函数--------------------
void admin_menu(Admin* admin);
Admin* create_admin(const char* username, const char* phone, const char* password, int privilege_level);
void add_admin(Admin** head, Admin* new_admin);
Admin* find_admin(Admin* head, const char* username);
void update_admin_password(Admin* admin, const char* new_password);
void save_admins_to_file(Admin* head, const char* filename);
Admin* load_admins_from_file(const char* filename);
void admin_change_privilege(Admin* executor, const char* target_username, int new_level);
bool is_super_admin(const Admin* admin);
bool has_permission(const Admin* admin, int required_level);
void admin_login();
void admin_register(Admin* executor);
void initialize_system();
bool delete_admin(Admin** head, const char* username);
void admin_delete_account(Admin* current_admin);
void modify_admin_info(Admin* current_admin);
void super_modify_admin_info(Admin* super_admin);
void get_password_input(char* buffer, int buffer_size);

Shelf* create_shelf(const char* shelf_id, int max_capacity);
void add_shelf(Shelf** head, Shelf* new_shelf);
Shelf* find_best_shelf(Shelf* shelves, Express* express);
void assign_express_to_shelf(Shelf** shelves, Express* express);
void save_shelves_to_file(Shelf* head, const char* filename);
Shelf* load_shelves_from_file(const char* filename);
void inventory_shelf(Shelf* shelf);
void full_inventory(Shelf* head);
void check_overdue_shelf(Shelf* shelf);
void check_shelf_capacity(Shelf* head);
void locate_express(Shelf* shelves, const char* tracking_num);

void clear_input_buffer();
void pause_program();
void clear_screen();
const char* get_status_text(PackageStatus status);
Express* find_express_by_tracking(Express* head, const char* tracking_num);

#endif // CAINIAO_H#pragma once
