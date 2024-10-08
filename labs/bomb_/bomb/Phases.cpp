using namespace std;
#include<string>

#include<string.h>


void explode_bomb(){};

//  phase_1
int main(){
    std::string string_ = read();
    bool check = strings_not_equal(string_,"对应参数");
    if (!check){
        explode_bomb();
    }
    return 0;
}


//  phase_2
int main(){
    int array[7];

    read_six_numbers(array);

    // 检查第一个数字是否为1
    if(array[0] == 1){
        explode_bomb();
    }

    // 检查剩余数字是否满足条件
    for(int i = 1; i <= 6; i++){
        if(array[i] != array[i-1] * 2){
            explode_bomb();
        }
    }

    return 0;
}

//  phase_3
int main(char * output){
    int x,y;
    int  check = sscanf(output,"%d %d",&x,&y); // 四个元素

    if (check <= 1){
         explode_bomb();
    }

    if(x > 7){
        explode_bomb();
    }

    int ret;
    switch (x) {
        case 0:
            ret = 0xcf; // corresponding to 0x400f7c
            break;
        case 1:
            ret = 0x2c3; // corresponding to 0x400f83
            break;
        case 2:
            ret = 0x100; // corresponding to 0x400f8a
            break;
        case 3:
            ret = 0x185; // corresponding to 0x400f91
            break;
        case 4:
            ret = 0xce; // corresponding to 0x400f98
            break;
        case 5:
            ret = 0x2aa; // corresponding to 0x400f9f
            break;
        case 6:
            ret = 0x147; // corresponding to 0x400fa6
            break;
        case 7:
            explode_bomb(); // corresponding to 0x400fad
            break;
        case 8:
            ret = 0x0; // corresponding to 0x400fb2
            break;
        case 9:
            ret = 0x137; // corresponding to 0x400fb9
            break;
    }
    if (ret != y){
        explode_bomb();
    }

    return 0;
}

// 
int func_4(int target, int low, int high) {
    int range_size = high - low;    // 当前搜索范围的大小
    range_size = (range_size + (range_size >> 31)) >> 1;   // 处理符号位并计算中点偏移量
    int mid = range_size + low;     // 计算当前中间点的值

    if(mid > target) {
        return 2 * func_4(target, low, mid - 1); // 在左半部分继续搜索
    } else if(mid < target) {
        return 2 * func_4(target, mid + 1, high) + 1; // 在右半部分继续搜索
    } else {
        return 0; // 找到目标值，返回0
    }
}



int main(char *output) {
    int x, y;
    int check = sscanf(output, "%d %d", &x, &y); // 解析用户输入
    if (check != 2) {
        explode_bomb(); // 如果输入格式不对，触发炸弹
    }

    if (x > 14) {
        explode_bomb(); // 如果 x 超过 14，触发炸弹
    }
    // x 小于等于 14

    check = func_4(x, 0, 14); // 调用 func_4 进行计算
    
    if (check != 0 || y != 0) {
        explode_bomb(); // 如果返回值不正确，触发炸弹 
    }

    return 0;
}

// phase_5


int string_length(string * str){
    if(!(*str)[0]){
        return 0;
    }
    string * str_offset_addr = str;
    int str_len = 0;

    do{
        str_offset_addr += 1;
        str_len = str_offset_addr - str;
    }
    while((*str_offset_addr)[0]);

    return str_len;
}

bool strings_not_equal(string * str1, string * str2){
    if(string_length(str1) != string_length(str2)){
        return true;
    }

    if(!(*str1)[0]){
        return false;
    }
    
    int i = 0;

    while((*str1)[i] == (*str2)[i]){
        i++;
        if(!(*str1)[i]){
            return false;
        }

        if((*str1)[i] != (*str2)[i]){
            return true;
        }
    }
    return true;
}

void phase_5(string* output){
    int len = string_length(output);
    if(len != 6){
        explode_bomb();
    }
    string *other;
    for(int i = 0;i < 6;i++){
        char c = (*output)[i]; 
        c = c & 0xf;
        (*other)[i] = *(char*)(0x4024b0 + c);
    }
    other[6] = '\0';
    
    string target = "flyers";
    bool check = strings_not_equal(other, &target);
    if(check){
        explode_bomb();
    }
}



// 确保六个数都不相同  (并且保证都小于7) 
//          1 -> 2~5     
//          2 -> 3~5 
//          ...
void check_six_numbers(int * arr){
    int i = 0;

    while(i < 5){
        if(arr[i] > 6){
            explode_bomb();
        } 
        i++;
        check_array(arr, i);
    }
}

void check_array(int * arr, int n){
    for(int i = n;i < 5;i++){
        if(arr[i] == arr[n - 1]){
            explode_bomb();
        }
    }
}

typedef struct {
    int val;
    Node* node_next;
}Node;

void phase_6(char * output){
    int array[6];
    Node* Node_array[6];
    read_six_numbers(output, array);
    
    check_six_numbers(array);

    // 调整数组元素
    for(int i = 0;i < 6;i++){
        array[i] = 7 - array[i];
    }

    for(int i = 0;i < 6;i++){
        int elem = array[i];
        Node* head = (Node*)0x6032d0;
        if(elem > 1){
            for(int j = 1;j != elem;j++){
                head = head->node_next;
            }
        }
        Node_array[i] = head;
    }

    for(int i = 0;i < 5;i++){
        Node_array[i]->node_next = Node_array[i + 1];
    }
    Node_array[5]->node_next = nullptr;
    

    for(int i = 0;i < 5;i++){
        if(Node_array[i]->val < Node_array[i]->node_next->val){
            explode_bomb();
        }
    }


}