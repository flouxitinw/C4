// cainiao.c
#include "cainiao.h"


//--------------------辅助函数--------------------
//获取物品类型名称
const char* get_package_type_name(PackageType type) {
    const char* names[] = {
        "日用品", "食品", "文件", "衣物",
        "数码产品", "酒", "动物", "武器",
        "易燃/易爆/有毒物品", "贵重物品", "生鲜",
        "易碎品", "其他"
    };
    return names[type];
}
// 检查是否是有效的 11 位手机号
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
// 获取用户输入范围内的整数
int input_int_in_range(const char* prompt, int min, int max) {
    int choice;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &choice) != 1) {  // 检查是否成功读取整数
            printf("输入无效，请重新输入！\n");
            while (getchar() != '\n');  // 清空输入缓冲区
            continue;
        }
        if (choice < min || choice > max) {  // 检查是否在范围内
            printf("输入必须介于 %d 和 %d 之间，请重新输入！\n", min, max);
            continue;
        }
        break;  // 输入正确，退出循环
    }
    return choice;
}
// 输入一个正浮点数（必须 > 0）
double input_positive_double(const char* prompt) {
    double value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%lf", &value) != 1) {  // 检查是否成功读取浮点数
            printf("请输入有效的数字：");
            while (getchar() != '\n');  // 清空输入缓冲区
            continue;
        }
        if (value <= 0) {  // 检查是否 > 0
            printf("输入必须大于 0，请重新输入：");
            continue;
        }
        break;  // 输入正确，退出循环
    }
    return value;
}


//--------------------寄快递核心函数--------------------
void send_express(User* user_head, ExpressNode** express_head) {
    Express* new_express = (Express*)malloc(sizeof(Express));
    if (!new_express) {
        perror("内存分配失败");
        return;
    }
    memset(new_express, 0, sizeof(Express)); // 初始化清零


    printf("== = 寄快递信息录入 == =");
    // 寄件人信息
    printf("寄件人姓名：");
    scanf("%s", new_express->sender_name);;
    while (1) {
        printf("寄件人电话：");
        fgets(new_express->sender_phone, 11, stdin);
        new_express->sender_phone[strcspn(new_express->sender_phone, "\n")] = '\0';  // 去掉换行符

        if (is_valid_phone(new_express->sender_phone)) {
            break;  // 输入正确，退出循环
        }
        else {
            printf("请输入正确手机号：");
        }
    }
    // 收件人信息
    printf("收件人姓名：");
    scanf("%s", new_express->receiver_name);
    while (1) {
        printf("收件人电话：");
        fgets(new_express->receiver_phone, 11, stdin);
        new_express->receiver_phone[strcspn(new_express->receiver_phone, "\n")] = '\0';  // 去掉换行符

        if (is_valid_phone(new_express->receiver_phone)) {
            break;  // 输入正确，退出循环
        }
        else {
            printf("请输入正确手机号：");
        }
    }


    // 包裹信息
    //包裹重量和体积
    new_express->weight = input_positive_double("包裹重量（kg)：");
    new_express->volume = input_positive_double("包裹体积(cm^3)：");
    if (new_express->weight > 50 || new_express->volume >= 3000) {
        printf("包裹过大不可邮寄");
        return;
    }
    // 物品类型选择
    printf("请选择物品类型：");
    for (int i = 0; i < 10; i++) {
        printf("%d. %s", i, get_package_type_name((PackageType)i));
    }
    int type_choice = input_int_in_range("请输入选项（0-9）：", 0, 9);
    if (type_choice >= 5 && type_choice <= 8) {
        printf("根据国家邮政法规禁止邮寄");
        return;
    }
    new_express->package_type = (PackageType)(type_choice);
    if (new_express->package_type == FRESH) {
        printf(" 建议选择冷链运输\n ");
    }
    if (new_express->package_type == VALUABLES || new_express->package_type == DIGITAL) {
        printf("建议开启保价服务\n");
    }
    if (new_express->weight > 20.0) {
        printf("建议选择大件邮寄方式\n");
    }


    // 运输方式选择
    printf("请选择运输方式");
    printf("0. 普通快递\n1. 大件邮寄\n 2. 冷链邮寄\n");
    int method_choice = input_int_in_range("请输入选项（0-2）：", 0, 2);
    new_express->method = (ShippingMethod)method_choice;


    // 地区选择
    printf(" 请选择地区类型：");
    printf("0. 省内\n1. 省外\n 2. 国际\n 3. 港澳台\n");
    int region_choice = input_int_in_range("请输入选项（0-3）：", 0, 3);
    new_express->region = (RegionType)region_choice;


    // 保价信息
    new_express->insured_value = input_positive_double("物品价值：");
    new_express->is_insured = input_int_in_range("是否保价（0-否 1-是）：", 0, 1);



    //运输时效
    printf("\n请选择运输时效：\n");
    printf("0. 标快（普通时效）\n");
    printf("1. 特快（加急）\n");
    int choice = input_int_in_range("请输入选项（0-1）：", 0, 1);
    new_express->express_type = (DeliveryType)choice;
    // 设置默认值
    new_express->status = STATUS_UNSENT;//如果要是中途没有成功怎么办包裹会不会显示



    // 创建链表节点
    ExpressNode* new_node = create_express_node(new_express); // 自动复制数据
    if (!new_node) {
        perror("内存分配失败");
        return;
    }

    // 插入到链表头部
    new_node->next = *express_head;  // 新节点指向原头节点
    *express_head = new_node;        // 更新头指针指向新节点

    printf("快递已成功创建!您应付%.2f元\n", new_node->data.fee); // 通过节点访问数据
    
}